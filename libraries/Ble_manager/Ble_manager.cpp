/* -*- mode: c++ -*-
 * kaleidoscope::plugin::Ble_manager -- Manage Bluetooth low energy status and functions in wireless devices
 * Copyright (C) 2020  Dygma Lab S.L.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Alejandro Parcet, @alexpargon
 *
 */

#include "Ble_manager.h"

#include "Ble_composite_dev.h"
#include "Communications.h"
#include "Kaleidoscope-FocusSerial.h"
#include "LEDEffect-Bluetooth-Pairing-Defy.h"
#include "cstdio"
#include "kaleidoscope/key_events.h"
#include "kaleidoscope/plugin/LEDControlDefy.h"

void device_name_evt_handler(void);

namespace kaleidoscope
{
namespace plugin
{

#define BLE_MANAGER_DEBUG_LOG   1

void BleManager::init(void)
{
    TinyUSBDevice.detach();

    // Init BLE and activate BLE advertising.
    ble_module_init();
    ble_goto_white_list_advertising_mode();
    uint32_t peerCount = pm_peer_count();

#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_INFO("Ble_manager: %i devices in flash.", peerCount);
#endif

    pm_peer_id_t peer_list[peerCount];
    pm_peer_id_list(peer_list,
                    &peerCount,
                    PM_PEER_ID_INVALID,
                    PM_PEER_ID_LIST_ALL_ID);

    // For all devices in the FDS memory region, if they exist, add it to the flashStorage memory region.
    for (uint32_t i = 0; i < peerCount; i++)
    {
        bool found = false;
        for (auto &paired_device : ble_flash_data.ble_connections)
        {
            if (peer_list[i] != PM_PEER_ID_INVALID &&
                peer_list[i] == paired_device.get_peer_id())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            // Take the first invalid and set it has valid
            for (auto &paired_device : ble_flash_data.ble_connections)
            {
                if (paired_device.get_peer_id() == PM_PEER_ID_INVALID)
                {
                    paired_device.reset();
                    paired_device.set_peer_id(peer_list[i]);

                    // Save it in flash memory.
                    Runtime.storage().put(flash_base_addr, ble_flash_data);
                    Runtime.storage().commit();

                    break;
                }
            }
        }
    }

    // For all devices in the flash storage check if exist in the FDS
    for (auto &paired_device : ble_flash_data.ble_connections)
    {
        bool found = false;
        for (uint32_t i = 0; i < peerCount; i++)
        {
            if (peer_list[i] != PM_PEER_ID_INVALID &&
                peer_list[i] == paired_device.get_peer_id())
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            paired_device.reset();

            // Save it in flash memory.
            Runtime.storage().put(flash_base_addr, ble_flash_data);
            Runtime.storage().commit();
        }
    }
}

EventHandlerResult BleManager::onSetup(void)
{
    flash_base_addr = kaleidoscope::plugin::EEPROMSettings::requestSlice(sizeof(ble_flash_data));

    Runtime.storage().get(flash_base_addr, ble_flash_data);
    // For now lest think that if this variable is invalid, restart everything.
    if (ble_flash_data.currentChannel == 0xFF)
    {
        ble_flash_data.reset();

        // Save it in flash memory.
        Runtime.storage().put(flash_base_addr, ble_flash_data);
        Runtime.storage().commit();
    }

#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_DEBUG("Ble_manager: Current channel %i", ble_flash_data.currentChannel);
#endif

    Runtime.storage().get(flash_base_addr, ble_flash_data);
    update_channel_and_name();
    // UX STUFFf
    uint8_t i = 0;
    for (auto &item : ble_flash_data.ble_connections) // get all the paires devices and store in a variable.
    {
        if (item.get_peer_id() == PM_PEER_ID_INVALID)
        {
            set_paired_channel_led(i, false); // set channel related bit to 0

#if BLE_MANAGER_DEBUG_LOG
            NRF_LOG_DEBUG("Ble_manager: Channel %i -> 0", i);
#endif
        }
        else
        {
            set_paired_channel_led(i, true); // set channel related bit to 1

#if BLE_MANAGER_DEBUG_LOG
            NRF_LOG_DEBUG("Ble_manager: Channel %i -> 1", i);
#endif
        }

        i++;
    }

#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_DEBUG("Ble_manager: %i channels in total.", channels);
#endif

    ledBluetoothPairingDefy.setConnectedChannel(NOT_CONNECTED);
    ledBluetoothPairingDefy.setEreaseDone(false);

#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_FLUSH();
#endif
    return EventHandlerResult::OK;
}

void BleManager::update_channel_and_name(void)
{
    set_current_channel(ble_flash_data.currentChannel);
    set_device_name(ble_flash_data.defy_ble_name);
    pm_peer_id_t active_connection_peer_id = ble_flash_data.ble_connections[ble_flash_data.currentChannel].get_peer_id();

    if (active_connection_peer_id == PM_PEER_ID_INVALID) // SI no tengo ningun dispositivo en el canal ejecuto el advertising con lista blanca para qu cualquier dispositivo lo encuentre
    {
#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_INFO("Ble_manager: Whitelist deactivated.");
#endif
        set_whitelist(false); // this allows the device to be visible to all devices
    }
    else
    {
#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_INFO("Ble_manager: Whitelist activated.");
#endif
        set_whitelist(true); // this allows the device to be visible ONLY for the paired devices
    }
}

EventHandlerResult BleManager::beforeEachCycle(void)
{
    if (!ble_innited())
    {
        return EventHandlerResult::OK;
    }

    timer_save_conn_run(2000);  // 2000ms timer.
    timer_save_name_run(2000);  // 2000ms timer.

    if (get_flag_security_proc_started())
    {
        mitm_activated = true;
        exit_pairing_mode(); // If it is in the "enter pin number" state, set default layer.
        clear_flag_security_proc_started();
    }
    else if (get_flag_security_proc_failed())
    {
        mitm_activated = false;
        send_led_mode(); // If enters pin number fails, returns to advertising led mode layer.
        show_bt_layer = true;
        clear_flag_security_proc_failed();
    }

    if (mitm_activated && ble_connected())
    {
        mitm_activated = false;
    }

    // Gives time to the EPPROM to update
    static bool activated_advertising = false;
    if (ble_is_advertising_mode())
    {
        // Just activate once
        if (!activated_advertising)
        {
            // set_pairing_led_effect();

#if BLE_MANAGER_DEBUG_LOG
            NRF_LOG_DEBUG("Ble_manager: Channel %i, advertising mode.", ble_flash_data.currentChannel);
            NRF_LOG_FLUSH();
#endif

            ledBluetoothPairingDefy.setAvertisingModeOn(ble_flash_data.currentChannel);
            show_bt_layer = true;
            send_led_mode();
            activated_advertising = true;
        }
    }
    else if (activated_advertising && ble_connected())
    {
        ledBluetoothPairingDefy.setConnectedChannel(ble_flash_data.currentChannel);
        ledBluetoothPairingDefy.setAvertisingModeOn(NOT_ON_ADVERTISING);
        set_paired_channel_led(ble_flash_data.currentChannel, true);
        send_led_mode();
        ble_get_device_name(device_name_evt_handler);  // Asynchronous call to the softdevice.
        activated_advertising = false;
        exit_pairing_mode();

        trigger_save_conn_timer = true;
    }
    else if (activated_advertising && ble_is_idle())
    {
        activated_advertising = false;
        LEDControl::disable();
        Communications_protocol::Packet p{};
        p.header.command = Communications_protocol::SLEEP;
        Communications.sendPacket(p);
    }

    // Reconection of side
    if (!activated_advertising && ble_is_idle() && LEDControl::isEnabled())
    {
        ble_goto_advertising_mode();
        ledBluetoothPairingDefy.setAvertisingModeOn(ble_flash_data.currentChannel);
        send_led_mode();
    }

    return EventHandlerResult::OK;
}

void BleManager::timer_save_conn_run(uint32_t timeout_ms)
{
    /*
        Instead of saving the data immediately, a timer is started that saves it Xms later, so as not to
        interfere with the use of the flash memory of the peer_manager module of the SDK.
    */

    if (trigger_save_conn_timer)
    {
        trigger_save_conn_timer = false;
        
#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_DEBUG("Ble_manager: Start timer save connection.");
#endif
        ti_save_new_conn = Runtime.millisAtCycleStart();
        timer_save_conn_start_count = true;
    }

    if (timer_save_conn_start_count)
    {
        if (Runtime.hasTimeExpired(ti_save_new_conn, timeout_ms))
        {
#if BLE_MANAGER_DEBUG_LOG
            NRF_LOG_DEBUG("Ble_manager: Timeout timer save connection.");
#endif
            save_connection();
            timer_save_conn_start_count = false;
        }

        // While this timer is running, the periodic update timer remains reset.
        EEPROM.reset_timer_update_periodically();
    }
}

void BleManager::save_connection(void)
{
    uint8_t *previous_dev_addr = ble_flash_data.ble_connections[ble_flash_data.currentChannel].get_device_addr();

#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_DEBUG("Ble_manager: previous_dev_addr = %02X %02X %02X %02X %02X %02X",
                  previous_dev_addr[0], previous_dev_addr[1],
                  previous_dev_addr[2], previous_dev_addr[3],
                  previous_dev_addr[4], previous_dev_addr[5]);


    NRF_LOG_DEBUG("Ble_manager: get_connected_device_address() = %02X %02X %02X %02X %02X %02X",
                  get_connected_device_address()[0], get_connected_device_address()[1],
                  get_connected_device_address()[2], get_connected_device_address()[3],
                  get_connected_device_address()[4], get_connected_device_address()[5]);
#endif

    uint16_t previous_peer_id = ble_flash_data.ble_connections[ble_flash_data.currentChannel].get_peer_id();

#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_DEBUG("Ble_manager: previous_peer_id = %i", previous_peer_id);
    NRF_LOG_DEBUG("Ble_manager: get_connected_peer_id() = %i", get_connected_peer_id());
#endif

    // It only saves it if it is different from the previous one.
    if ( (memcmp(previous_dev_addr, get_connected_device_address(), BLE_ADDRESS_LEN) != 0) ||
        (previous_peer_id != get_connected_peer_id()) )
    {
        // Change it in RAM.
        ble_flash_data.ble_connections[ble_flash_data.currentChannel].set_device_addr(get_connected_device_address(),
                                                                                      BLE_ADDRESS_LEN);
        ble_flash_data.ble_connections[ble_flash_data.currentChannel].set_peer_id(get_connected_peer_id());

        // Save it in flash memory.
#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_DEBUG("Ble_manager: Saving new connection...");
#endif

        Runtime.storage().put(flash_base_addr, ble_flash_data);
        Runtime.storage().commit();
    }
}

void BleManager::timer_save_name_run(uint32_t timeout_ms)
{
    /*
        Instead of saving the data immediately, a timer is started that saves it Xms later, so as not to
        interfere with the use of the flash memory of the peer_manager module of the SDK.
    */

    if (trigger_save_name_timer)
    {
        trigger_save_name_timer = false;
        
#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_DEBUG("Ble_manager: Start timer save name.");
#endif
        ti_save_new_name = Runtime.millisAtCycleStart();
        timer_save_name_start_count = true;
    }

    if (timer_save_name_start_count)
    {
        if (Runtime.hasTimeExpired(ti_save_new_name, timeout_ms))
        {
#if BLE_MANAGER_DEBUG_LOG
            NRF_LOG_DEBUG("Ble_manager: Timeout timer save name.");
#endif
            save_device_name();
            timer_save_name_start_count = false;
        }

        // While this timer is running, the periodic update timer remains reset.
        EEPROM.reset_timer_update_periodically();
    }
}

void BleManager::save_device_name(void)
{
    uint8_t *previous_dev_name = ble_flash_data.ble_connections[ble_flash_data.currentChannel].get_device_name_ptr();

#if BLE_MANAGER_DEBUG_LOG
    uint8_t empty[BLE_DEVICE_NAME_LEN] = {};  // Init all with 0.
    if (memcmp(previous_dev_name, empty, BLE_DEVICE_NAME_LEN) != 0)  // If the channel was in use.
    {
        NRF_LOG_DEBUG("Ble_manager: previous_dev_name = %s", previous_dev_name);
    }
    else
    {
        NRF_LOG_DEBUG("Ble_manager: previous_dev_name =");
    }

    NRF_LOG_DEBUG("Ble_manager: get_connected_device_name_ptr() = %s", get_connected_device_name_ptr());
#endif

    // It only saves it, if it is different from the previous one.
    if (memcmp(previous_dev_name, get_connected_device_name_ptr(), BLE_DEVICE_NAME_LEN) != 0)
    {
        // Change it in RAM.
        ble_flash_data.ble_connections[ble_flash_data.currentChannel].set_device_name(get_connected_device_name_ptr(),
                                                                                            BLE_DEVICE_NAME_LEN);

        // Save it in flash memory.
#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_DEBUG("Ble_manager: Saving device name...");
#endif

        Runtime.storage().put(flash_base_addr, ble_flash_data);
        Runtime.storage().commit();
    }
}

EventHandlerResult BleManager::onKeyswitchEvent(Key &mappedKey, KeyAddr key_addr, uint8_t keyState)
{
    /* Exit conditions. */
    if (!ble_innited())
    {
        if (mappedKey.getRaw() == ranges::BLUETOOTH_PAIRING && keyToggledOn(keyState))
        {
            auto const &keyScanner = Runtime.device().keyScanner();
            auto isDefyLeftWired = keyScanner.leftSideWiredConnection();
            auto isDefyRightWired = keyScanner.rightSideWiredConnection();
            auto isWiredMode = isDefyLeftWired || isDefyRightWired;

            if (isWiredMode)
            {
                ble_flash_data.forceBle = true;

                // Save it in flash memory.
                Runtime.storage().put(flash_base_addr, ble_flash_data);
                Runtime.storage().commit();

                reset_mcu();
            }
        }
        else
        {
            return EventHandlerResult::OK;
        }
    }

    if (keyToggledOff(keyState) && mitm_activated && is_num_key(mappedKey.getRaw()))
    {
        static uint8_t count = 0;

        encryption_pin_number[count] = raw_key_to_ascii(mappedKey.getRaw());

#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_DEBUG("Ble_manager: encryption_pin_number[%d] = %c", count, encryption_pin_number[count]);
        NRF_LOG_FLUSH();
#endif

        count++;

        if (count == 6) // PIN numbers has 6 digits.
        {
#if BLE_MANAGER_DEBUG_LOG
            NRF_LOG_DEBUG("Ble_manager: Sending pin number..");
            NRF_LOG_FLUSH();
#endif

            count = 0;
            ble_send_encryption_pin(encryption_pin_number);
            mitm_activated = false;
        }

        return EventHandlerResult::EVENT_CONSUMED;
    }

    if (!ble_is_advertising_mode() && show_bt_layer && mappedKey.getRaw() == ranges::BLUETOOTH_PAIRING && keyToggledOff(keyState))
    {
        exit_pairing_mode();

        return EventHandlerResult::EVENT_CONSUMED;
    }

    if (mappedKey.getRaw() == ranges::BLUETOOTH_PAIRING && keyToggledOff(keyState))
    {

        send_led_mode();
        show_bt_layer = true;

        return EventHandlerResult::EVENT_CONSUMED;
    }

    EventHandlerResult result = EventHandlerResult::OK;
    
    if (ble_is_idle())
    {
        ble_goto_advertising_mode();
        ledBluetoothPairingDefy.setAvertisingModeOn(ble_flash_data.currentChannel);
        send_led_mode();
        LEDControl::enable();
    }

    if (show_bt_layer)
    {
        if (((key_addr.col() == 0 && key_addr.row() == 0) || (key_addr.col() == 9 && key_addr.row() == 0)) && keyToggledOn(keyState))
        {
            reset_mcu();
        }

        /* Erease previous paired channels*/
        if (((key_addr.col() == 1 && key_addr.row() == 1) ||  // Q key
             (key_addr.col() == 2 && key_addr.row() == 1) ||  // W key
             (key_addr.col() == 3 && key_addr.row() == 1) ||  // E key
             (key_addr.col() == 4 && key_addr.row() == 1) ||  // R key
             (key_addr.col() == 5 && key_addr.row() == 1) ||  // T key
             (key_addr.col() == 10 && key_addr.row() == 1) || // Y key
             (key_addr.col() == 11 && key_addr.row() == 1) || // U key
             (key_addr.col() == 12 && key_addr.row() == 1) || // I key
             (key_addr.col() == 13 && key_addr.row() == 1) || // O key
             (key_addr.col() == 14 && key_addr.row() == 1))   // PT key
            && keyIsPressed(keyState))
        {
            set_channel_in_use(key_addr);
            uint8_t index_channel = channel_in_use;
            if (keyToggledOn(keyState))
            {
                connectionState[index_channel].timePressed = millis();
            }

            if (keyIsPressed(keyState) && millis() - connectionState[index_channel].timePressed >= 3000)
            {
                // TODO: create a led effect to let the user know that the erease was successful
                erase_paired_device(index_channel);

                result = EventHandlerResult::EVENT_CONSUMED;
            }

            if (keyToggledOff(keyState))
            {
                connectionState[index_channel].timePressed = 0;
                connectionState[index_channel].longPress = false;
            }
        }

        /*Change channel in use*/
        if (((key_addr.col() == 1 && key_addr.row() == 0) ||                                                  // 1 key
             (key_addr.col() == 2 && key_addr.row() == 0) ||                                                  // 2 key
             (key_addr.col() == 3 && key_addr.row() == 0) ||                                                  // 3 key
             (key_addr.col() == 4 && key_addr.row() == 0) ||                                                  // 4 key
             (key_addr.col() == 5 && key_addr.row() == 0) || (key_addr.col() == 10 && key_addr.row() == 0) || // 6 key
             (key_addr.col() == 11 && key_addr.row() == 0) ||                                                 // 7 key
             (key_addr.col() == 12 && key_addr.row() == 0) ||                                                 // 8 key
             (key_addr.col() == 13 && key_addr.row() == 0) ||                                                 // 9 key
             (key_addr.col() == 14 && key_addr.row() == 0))                                                   // 0 key
            && keyWasPressed(keyState))
        {
            set_channel_in_use(key_addr);

#if BLE_MANAGER_DEBUG_LOG
            //NRF_LOG_DEBUG("Ble_manager: channel_in_use: %i", channel_in_use);
#endif

            uint8_t index_channel = channel_in_use;
            // Only if the key has not been press for longer than 2 seconds then change the gapAddress
            if (keyToggledOff(keyState))
            {
                // NRF_LOG_DEBUG(" ble_is_advertising_mode(): %i", ble_is_advertising_mode());
                if (ble_flash_data.currentChannel != index_channel)
                {
#if BLE_MANAGER_DEBUG_LOG
                    NRF_LOG_DEBUG("Ble_manager: Changing channel %i to %i", ble_flash_data.currentChannel, index_channel);
#endif

                    ble_flash_data.currentChannel = index_channel;
                    ledBluetoothPairingDefy.setConnectedChannel(NOT_CONNECTED);
                    ledBluetoothPairingDefy.setAvertisingModeOn(ble_flash_data.currentChannel);
                    send_led_mode();
                    update_channel_and_name();

                    // First we disable scanning and advertising.
                    ble_adv_stop();
                    delay(200);
                    
                    // Save it in flash memory.
                    Runtime.storage().put(flash_base_addr, ble_flash_data);
                    Runtime.storage().commit();
                    /*
                        In this case, the data need to be saved instantly and not when
                        run_update_periodically() is executed.
                    */
                    EEPROM.update();

                    update_current_channel();
                    delay(200);

                    // Try to change the channel.
                    ble_disconnect();
                    delay(200);

                    // Try to reconnect again.
                    gap_params_init();
                    delay(200);

                    ble_adv_stop();
                    advertising_init();
                    delay(200);

                    /*
                        If it doesn't have any device paired on the channel, it goes into
                        advertising with a whitelist so that any device can find it.
                    */
                    pm_peer_id_t active_connection_peer_id = ble_flash_data.ble_connections[ble_flash_data.currentChannel].get_peer_id();
                    if (active_connection_peer_id == PM_PEER_ID_INVALID)
                    {
#if BLE_MANAGER_DEBUG_LOG
                        NRF_LOG_INFO("Ble_manager: Whitelist deactivated.");
#endif
                        ble_goto_advertising_mode();
                    }
                    else
                    {
#if BLE_MANAGER_DEBUG_LOG
                        NRF_LOG_INFO("Ble_manager: Whitelist activated.");
#endif
                        ble_goto_white_list_advertising_mode();
                    }

                    result = EventHandlerResult::EVENT_CONSUMED;
                }
                else if (ble_flash_data.currentChannel == index_channel && !ble_is_advertising_mode())
                {
                    exit_pairing_mode();
                }
            }

#if BLE_MANAGER_DEBUG_LOG
            NRF_LOG_FLUSH();
#endif
        }
    }

    return result;
}

bool BleManager::is_num_key(uint16_t raw_key)
{
    /*
        #define HID_KEYBOARD_1_AND_EXCLAMATION_POINT	0x1E	// Sel
        #define HID_KEYBOARD_2_AND_AT			0x1F	// Sel
        #define HID_KEYBOARD_3_AND_POUND		0x20	// Sel
        #define HID_KEYBOARD_4_AND_DOLLAR		0x21	// Sel
        #define HID_KEYBOARD_5_AND_PERCENT		0x22	// Sel
        #define HID_KEYBOARD_6_AND_CARAT		0x23	// Sel
        #define HID_KEYBOARD_7_AND_AMPERSAND		0x24	// Sel
        #define HID_KEYBOARD_8_AND_ASTERISK		0x25	// Sel
        #define HID_KEYBOARD_9_AND_LEFT_PAREN		0x26	// Sel
        #define HID_KEYBOARD_0_AND_RIGHT_PAREN		0x27	// Sel
    */
    if ((raw_key >= HID_KEYBOARD_1_AND_EXCLAMATION_POINT) && (raw_key <= HID_KEYBOARD_0_AND_RIGHT_PAREN))
    {
        return true;
    }

    return false;
}

char BleManager::raw_key_to_ascii(uint16_t raw_key)
{
    uint8_t num = raw_key - HID_KEYBOARD_1_AND_EXCLAMATION_POINT + 1;

    if (num != 10)
    {
        return (num + '0'); // To ASCII num.
    }
    else
    {
        return '0';
    }
}

void BleManager::erase_paired_device(uint8_t index_channel)
{
    if (ble_flash_data.ble_connections[index_channel].get_peer_id() != PM_PEER_ID_INVALID)
    {
#if BLE_MANAGER_DEBUG_LOG
        NRF_LOG_DEBUG("Ble_manager: Deleting paired device on channel %i...", index_channel);
        NRF_LOG_FLUSH();
#endif

        connectionState[index_channel].timePressed = 0;
        connectionState[index_channel].longPress = true;
        uint8_t peerCount = pm_peer_count();
        pm_peer_id_t peer_list[peerCount];
        uint32_t list_size = peerCount;
        pm_peer_id_list(peer_list, &list_size, PM_PEER_ID_INVALID, PM_PEER_ID_LIST_ALL_ID);

        for (uint8_t i = 0; i < peerCount; ++i)
        {
            if (peer_list[i] != PM_PEER_ID_INVALID && peer_list[i] == ble_flash_data.ble_connections[index_channel].get_peer_id())
            {
                delete_peer_by_id(ble_flash_data.ble_connections[index_channel].get_peer_id());
            }
        }

        ble_flash_data.ble_connections[index_channel].reset();
        set_paired_channel_led(index_channel, false); // set channel related bit to 0

        // Save it in flash memory.
        Runtime.storage().put(flash_base_addr, ble_flash_data);
        Runtime.storage().commit();

        if (index_channel == ble_flash_data.currentChannel)
        {
            ledBluetoothPairingDefy.setConnectedChannel(NOT_CONNECTED);
            ble_disconnect();
            // We could remove this reset but a weird led effect happends when the channel is the same
            reset_mcu();
        }

        ledBluetoothPairingDefy.setEreaseDone(true);
        send_led_mode(); // this is to update the led effect.
        ledBluetoothPairingDefy.setEreaseDone(false);
    }
}

void BleManager::exit_pairing_mode(void)
{
#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_DEBUG("Ble_manager: Exit pairing mode.");
    NRF_LOG_FLUSH();
#endif

    show_bt_layer = false;
    ColormapEffectDefy::updateBrigthness(ColormapEffectDefy::LedBrightnessControlEffect::BT_LED_EFFECT,
                                         false,
                                         true);
    ::LEDControl.set_force_mode(false);
    ::LEDControl.set_mode(0); // Disable LED fade effect.
}

void BleManager::set_paired_channel_led(uint8_t channel, bool turnOn)
{
    uint8_t bitPosition = channel; // Posición del bit que quieres establecer en 1 (empezando desde 0)
    if (turnOn)
    {
        channels |= (1 << bitPosition); // Establece el bit en la channel 3 en 1
    }
    else
    {
        channels &= ~(1 << bitPosition); // Establece el bit en la channel 3 en 0
    }

    ledBluetoothPairingDefy.setPairedChannels(channels);
}

void BleManager::send_led_mode(void)
{
    ::LEDControl.set_mode(10);
    ::LEDControl.set_force_mode(true);
    ColormapEffectDefy::updateBrigthness(ColormapEffectDefy::LedBrightnessControlEffect::BT_LED_EFFECT,
                                         true,
                                         false);
}

void BleManager::set_channel_in_use(KeyAddr key_addr)
{
    const int colMapping[5][2] =
    {
        {1, 10},
        {2, 11},
        {3, 12},
        {4, 13},
        {5, 14}
    };

    for (int i = 0; i < 5; i++)
    {
        if ( (key_addr.col() == colMapping[i][0] || key_addr.col() == colMapping[i][1]) &&
            (key_addr.row() == 0 || key_addr.row() == 1) )
        {
            channel_in_use = static_cast<Channels>(i);

            break;
        }
    }
}

bool BleManager::getForceBle(void)
{
    return ble_flash_data.forceBle;
}

void BleManager::setForceBle(bool enabled)
{
    ble_flash_data.forceBle = enabled;

    // Save it in flash memory.
    Runtime.storage().put(flash_base_addr, ble_flash_data);
    Runtime.storage().commit();
}

EventHandlerResult BleManager::onFocusEvent(const char *command)
{
    //    if (::Focus.handleHelp(command, "wireless.bluetooth.devicesMap\nwireless.bluetooth.deviceName")) return EventHandlerResult::OK;
    //
    //    if (strncmp(command, "wireless.bluetooth.", 19) != 0) return EventHandlerResult::OK;
    //    if (strcmp(command + 19, "devicesMap") == 0)
    //    {
    //        if (::Focus.isEOL())
    //        {
    //            for (const auto &connection : ble_flash_data.ble_connections)
    //            {
    //                connection.send();
    //            }
    //        }
    //        else
    //        {
    //            for (auto &connection : ble_flash_data.ble_connections)
    //            {
    //                connection.read();
    //            }
    //
    //            // Save it in flash memory.
    //            Runtime.storage().put(flash_base_addr, ble_flash_data);
    //            Runtime.storage().commit();
    //        }
    //    }
    //
    //    // This command need reset
    //    if (strcmp(command + 19, "deviceName") == 0)
    //    {
    //        if (::Focus.isEOL())
    //        {
    //#if BLE_MANAGER_DEBUG_LOG
    //            NRF_LOG_DEBUG("read request: wireless.bluetooth.deviceName");
    //#endif
    //
    //            for (const auto &device_name_letter : ble_flash_data.defy_ble_name)
    //            {
    //                ::Focus.send((uint8_t)device_name_letter);
    //            }
    //        }
    //        else
    //        {
    //#if BLE_MANAGER_DEBUG_LOG
    //            NRF_LOG_DEBUG("write request: wireless.bluetooth.deviceName");
    //#endif
    //
    //            for (auto &device_name_letter : ble_flash_data.defy_ble_name)
    //            {
    //                uint8_t aux;
    //                ::Focus.read(aux);
    //                device_name_letter = (char)aux;
    //            }
    //
    //            // Save it in flash memory.
    //            Runtime.storage().put(flash_base_addr, ble_flash_data);
    //            Runtime.storage().commit();
    //        }
    //    }

    // return EventHandlerResult::EVENT_CONSUMED;
    return EventHandlerResult::OK;
}

EventHandlerResult BleManager::beforeReportingState(void)
{
    if (show_bt_layer)
    {
        Runtime.device().hid().keyboard().releaseAllKeys();
    }

    return EventHandlerResult::OK;
}

} // namespace plugin
} //  namespace kaleidoscope


kaleidoscope::plugin::BleManager BleManager;


void device_name_evt_handler(void)
{
#if BLE_MANAGER_DEBUG_LOG
    NRF_LOG_INFO("Ble_manager: Event device name %s", get_connected_device_name_ptr());
#endif

    BleManager.trigger_save_name_timer = true;
}
