/* -*- mode: c++ -*-
 * kaleidoscope::plugin::RadioManager -- Manage RF Signal and status in wireless devices
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

#include "Radio_manager.h"
#include "Adafruit_USBD_Device.h"
#include "CRC_wrapper.h"
#include "Communications.h"
#include "Kaleidoscope-FocusSerial.h"
#include "nrf_gpio.h"
#include "rf_host_device_api.h"
namespace kaleidoscope
{
namespace plugin
{

uint16_t RadioManager::settings_base_ = 0;
bool RadioManager::inited = false;
RadioManager::Power RadioManager::power_rf = LOW_P;
uint16_t RadioManager::channel_hop;
EventHandlerResult RadioManager::onSetup()
{
    settings_base_ = kaleidoscope::plugin::EEPROMSettings::requestSlice(sizeof(power_rf));
    Runtime.storage().get(settings_base_, power_rf);
    if (power_rf == 0xFF)
    {
        power_rf = LOW_P;
        Runtime.storage().put(settings_base_, power_rf);
        Runtime.storage().commit();
    }
    return EventHandlerResult::OK;
}

void RadioManager::init()
{
    NRF_LOG_INFO("Working with RF");
    inited = true;
    rfgw_host_init();
    rfgw_addr_set(rfgw_addr_suggest());
    setPowerRF();
    rfgw_enable();
    // Open the Keyscanner pipes.
    rfgw_pipe_open(RFGW_PIPE_ID_KEYSCANNER_LEFT);
    rfgw_pipe_open(RFGW_PIPE_ID_KEYSCANNER_RIGHT);
}

void RadioManager::setPowerRF()
{
    if (!inited) return;
    switch (power_rf)
    {
        case LOW_P:
            rfgw_tx_power_set(RFGW_TX_POWER_0_DBM);
            break;
        case MEDIUM_P:
            rfgw_tx_power_set(RFGW_TX_POWER_4_DBM);
            break;
        case HIGH_P:
            rfgw_tx_power_set(RFGW_TX_POWER_8_DBM);
            break;
    }
}

EventHandlerResult RadioManager::onFocusEvent(const char *command)
{
    if (::Focus.handleHelp(command, "wireless.rf.power\nwireless.rf.channelHop\nwireless.rf.syncPairing")) return EventHandlerResult::OK;

    if (strncmp(command, "wireless.rf.", 12) != 0) return EventHandlerResult::OK;

    // Will be apply
    if (strcmp(command + 12, "power") == 0)
    {
        if (::Focus.isEOL())
        {
            NRF_LOG_DEBUG("read request: wireless.rf.power");
            ::Focus.send<uint8_t>((uint8_t)power_rf);
        }
        else
        {
            uint8_t power;
            ::Focus.read(power);
            if (power <= HIGH_P)
            {
                power_rf = (RadioManager::Power)power;
                setPowerRF();
                Runtime.storage().put(settings_base_, power_rf);
                Runtime.storage().commit();
            }
        }
    }

    if (strcmp(command + 12, "channelHop") == 0)
    {
        if (::Focus.isEOL())
        {
            NRF_LOG_DEBUG("read request: wireless.rf.channelHop");
            ::Focus.send(channel_hop); // TODO: get set channelHop mode configuration status
        }
        else
        {
            NRF_LOG_DEBUG("write request: wireless.rf.channelHop");
            ::Focus.read(channel_hop);
        }
    }

    if (strcmp(command + 12, "syncPairing") == 0)
    {
        if (::Focus.isEOL())
        {
            NRF_LOG_DEBUG("sync pairing procedure: wireless.rf.syncPairing");
            Communications_protocol::Packet packet{};
            packet.header.command = Communications_protocol::RF_ADDRESS;
            uint32_t rf_address = rfgw_addr_suggest();
            packet.header.size = sizeof(rf_address);
            memcpy(packet.data, &rf_address, packet.header.size);
            Communications.sendPacket(packet);
        }
    }

    return EventHandlerResult::EVENT_CONSUMED;
}

bool RadioManager::isInited()
{
    return inited;
}

void RadioManager::poll()
{
    return rfgw_poll();
}

} // namespace plugin
} //  namespace kaleidoscope

kaleidoscope::plugin::RadioManager RadioManager;
