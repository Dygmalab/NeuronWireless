/* -*- mode: c++ -*-
 * kaleidoscope::plugin::Battery_manager -- Manage battery levels and status in wireless devices
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

#include "Battery.h"
#include "Colormap-Defy.h"
#include "Communications.h"
#include "Kaleidoscope-FocusSerial.h"
#include "kaleidoscope/key_events.h"
#include "kaleidoscope/plugin/LEDControlDefy.h"

#define NOT_CHARGING 0
namespace kaleidoscope
{
namespace plugin
{

#define DEBUG_LOG_BATTERY_MANAGER   0

uint8_t Battery::battery_level;
uint8_t Battery::saving_mode;
uint16_t Battery::settings_saving_;
uint8_t Battery::status_left = 4;
uint8_t Battery::status_right = 4;
uint8_t Battery::battery_level_left = 100;
uint8_t Battery::battery_level_right = 100;

EventHandlerResult Battery::onKeyswitchEvent(Key &mappedKey, KeyAddr key_addr, uint8_t keyState)
{
    if (mappedKey.getRaw() != ranges::BATTERY_LEVEL)
    {
        return EventHandlerResult::OK;
    }

    if (keyToggledOn(keyState))
    {
        ::LEDControl.set_mode(9);
        ::LEDControl.set_force_mode(true);
        ColormapEffectDefy::updateBrigthness(ColormapEffectDefy::LedBrightnessControlEffect::BATTERY_STATUS, true);
    }

    if (keyToggledOff(keyState))
    {
        ColormapEffectDefy::updateBrigthness(ColormapEffectDefy::LedBrightnessControlEffect::BATTERY_STATUS, false);
        ::LEDControl.set_force_mode(false);
        ::LEDControl.set_mode(0);
    }

    return EventHandlerResult::EVENT_CONSUMED;
}

EventHandlerResult Battery::onFocusEvent(const char *command)
{
    if (::Focus.handleHelp(
            command,
            "wireless.battery.left.level\nwireless.battery.right.level\nwireless.battery.left.status\nwireless.battery.right.status\nwireless.battery.savingMode"))
        return EventHandlerResult::OK;

    if (strncmp(command, "wireless.battery.", 17) != 0) return EventHandlerResult::OK;

    if (strcmp(command + 17, "right.level") == 0)
    {
        if (::Focus.isEOL())
        {
#if DEBUG_LOG_BATTERY_MANAGER
            NRF_LOG_DEBUG("read request: wireless.battery.right.level");
#endif
            ::Focus.send(battery_level_right);
        }
    }

    if (strcmp(command + 17, "left.level") == 0)
    {
        if (::Focus.isEOL())
        {
#if DEBUG_LOG_BATTERY_MANAGER
            NRF_LOG_DEBUG("read request: wireless.battery.left.level");
#endif
            ::Focus.send(battery_level_left);
        }
    }

    if (strcmp(command + 17, "right.status") == 0)
    {
        if (::Focus.isEOL())
        {
#if DEBUG_LOG_BATTERY_MANAGER
            NRF_LOG_DEBUG("read request: wireless.battery.right.status");
#endif
            ::Focus.send(status_right);
        }
    }

    if (strcmp(command + 17, "left.status") == 0)
    {
        if (::Focus.isEOL())
        {
            Communications_protocol::Packet p{};
            p.header.command = Communications_protocol::BATTERY_STATUS;
            Communications.sendPacket(p);
#if DEBUG_LOG_BATTERY_MANAGER
            NRF_LOG_DEBUG("read request: wireless.battery.left.status");
#endif
            ::Focus.send(status_left);
        }
    }

    if (strcmp(command + 17, "savingMode") == 0)
    {
        if (::Focus.isEOL())
        {
#if DEBUG_LOG_BATTERY_MANAGER
            NRF_LOG_DEBUG("read request: wireless.battery.savingMode");
#endif
            ::Focus.send(saving_mode);
        }
        else
        {
            ::Focus.read(saving_mode);
#if DEBUG_LOG_BATTERY_MANAGER
            NRF_LOG_DEBUG("write request: wireless.battery.savingMode");
#endif
            Communications_protocol::Packet p{};
            p.header.command = Communications_protocol::BATTERY_SAVING;
            p.header.size = 1;
            p.data[0] = saving_mode;
            Communications.sendPacket(p);
            Runtime.storage().put(settings_saving_, saving_mode);
            Runtime.storage().commit();
        }
    }

    return EventHandlerResult::EVENT_CONSUMED;
}

bool inline filterHand(Communications_protocol::Devices incomingDevice, bool right_or_left)
{
    if (right_or_left == 1)
    {
        return incomingDevice == Communications_protocol::KEYSCANNER_DEFY_RIGHT || incomingDevice == Communications_protocol::BLE_DEFY_RIGHT ||
               incomingDevice == Communications_protocol::RF_DEFY_RIGHT;
    }
    else
    {
        return incomingDevice == Communications_protocol::KEYSCANNER_DEFY_LEFT || incomingDevice == Communications_protocol::BLE_DEFY_LEFT ||
               incomingDevice == Communications_protocol::RF_DEFY_LEFT;
    }
}

EventHandlerResult Battery::onSetup()
{

    // Save saving_mode variable in EEPROM
    settings_saving_ = ::EEPROMSettings.requestSlice(sizeof(saving_mode));
    uint8_t saving;
    Runtime.storage().get(settings_saving_, saving);
    if (saving == 0xFF)
    { // If is the first time we read from memory
        saving_mode = 0;
        Runtime.storage().put(settings_saving_, saving_mode); // Save default value 0.
        Runtime.storage().commit();
    }
    Runtime.storage().get(settings_saving_, saving_mode); // safe get


    Communications.callbacks.bind(BATTERY_STATUS, (
                                                      [this](Packet const &packet)
                                                      {
                                                          if (filterHand(packet.header.device, false))
                                                          {
                                                              status_left = packet.data[0];
                                                          }
                                                          if (filterHand(packet.header.device, true))
                                                          {
                                                              status_right = packet.data[0];
                                                          }

#if DEBUG_LOG_BATTERY_MANAGER
                                                          NRF_LOG_DEBUG("Battery Status device %i %i", packet.header.device, packet.data[0]);
#endif
                                                      }));

    Communications.callbacks.bind(BATTERY_LEVEL, (
                                                     [this](Packet const &packet)
                                                     {
                                                         if (filterHand(packet.header.device, false))
                                                         {
                                                             battery_level_left = packet.data[0];
                                                         }
                                                         if (filterHand(packet.header.device, true))
                                                         {
                                                             battery_level_right = packet.data[0];
                                                         }
                                                         uint16_t battery_level_mv;
                                                         memcpy(&battery_level_mv, &packet.data[1], sizeof(battery_level_mv));
                                                         ble_battery_level_update(min(battery_level_left, battery_level_right));
#if DEBUG_LOG_BATTERY_MANAGER
                                                         NRF_LOG_DEBUG("Battery level: %i device %i percentage %i mv",
                                                                       packet.header.device,
                                                                       packet.data[0],
                                                                       battery_level_mv);
#endif
                                                     }));

    Communications.callbacks.bind(DISCONNECTED, (
                                                    [this](Packet const &packet)
                                                    {
                                                        if (filterHand(packet.header.device, false))
                                                        {
                                                            battery_level_left = 100;
                                                            status_left = 4;
                                                        }
                                                        if (filterHand(packet.header.device, true))
                                                        {
                                                            battery_level_right = 100;
                                                            status_right = 4;
                                                        }
                                                        ble_battery_level_update(min(battery_level_left, battery_level_right));
                                                    }));

    Communications.callbacks.bind(CONNECTED, (
                                                 [this](Packet packet)
                                                 {
                                                     packet.header.command = BATTERY_SAVING;
                                                     packet.header.size = 1;
                                                     packet.data[0] = saving_mode;
                                                     Communications.sendPacket(packet);
                                                 }));
    return EventHandlerResult::OK;
}

} // namespace plugin
} //  namespace kaleidoscope

kaleidoscope::plugin::Battery Battery;
