/* -*- mode: c++ -*-
 * kaleidoscope::plugin::Energy_manager -- Manage Energy settings and functions on wireless devices
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

#include "Energy_manager.h"
#include "Kaleidoscope-FocusSerial.h"
#include "kaleidoscope/key_events.h"

namespace kaleidoscope {
namespace plugin {

uint8_t EnergyManager::energy_saving_mode;
uint8_t EnergyManager::energy_mode_disable;
uint16_t EnergyManager::storage_base_;
uint16_t EnergyManager::storage_size_;

EventHandlerResult EnergyManager::onKeyswitchEvent(Key &mappedKey,
                                                   KeyAddr key_addr,
                                                   uint8_t keyState) {
    if (mappedKey.getRaw() != ranges::ENERGY_MODE_NEXT || mappedKey.getRaw() != ranges::ENERGY_MODE_PREV) {
        return EventHandlerResult::OK;
    }

    if (mappedKey.getRaw() == ranges::ENERGY_MODE_NEXT) {

        if (keyToggledOn(keyState)) {
            // TODO: Key pressed!! Cycle to next energy mode
        }
    }

    if (mappedKey.getRaw() == ranges::ENERGY_MODE_PREV) {

        if (keyToggledOn(keyState)) {
            // TODO: Key pressed!! Cycle to previous energy mode
        }
    }

    return EventHandlerResult::EVENT_CONSUMED;
}

EventHandlerResult EnergyManager::onFocusEvent(const char *command) {
    if (::Focus.handleHelp(command, "wireless.energy.modes\nwireless.energy.currentMode\nwireless.energy.disable"))
        return EventHandlerResult::OK;

    if (strncmp(command, "wireless.energy.", 16) != 0)
        return EventHandlerResult::OK;

    if (strcmp(command + 16, "modes") == 0) {
        if (::Focus.isEOL()) {
            NRF_LOG_DEBUG("read request: wireless.energy.modes");
            for (uint16_t i = 0; i < storage_size_; i++) {
                uint8_t b=0;
                // b = Runtime.storage().read(storage_base_ + i); //TODO: retrieve stored energy modes from memory
                ::Focus.send(b);  //TODO: send list of energy modes stored
            }
        } else {
            uint16_t pos = 0;
            uint8_t b;

            NRF_LOG_DEBUG("write request: wireless.energy.modes");

            while (!::Focus.isEOL()) {
                ::Focus.read(b);
                Runtime.storage().update(storage_base_ + pos++, b);
            }
            Runtime.storage().commit();
        }
    }

    if (strcmp(command + 16, "disable") == 0) {
        if (::Focus.isEOL()) {
            //NRF_LOG_DEBUG("read request: wireless.energy.disable without data");
        } else {
            NRF_LOG_DEBUG("read request: wireless.energy.disable");
            ::Focus.read(energy_mode_disable);  //TODO: send energy stack status, messages are still TBD
        }
    }

    if (strcmp(command + 16, "currentMode") == 0) {
        if (::Focus.isEOL()) {
            NRF_LOG_DEBUG("read request: wireless.energy.currentMode");
            ::Focus.send(energy_saving_mode);  //TODO: send energy saving current mode status

        } else {
            NRF_LOG_DEBUG("write request: wireless.energy.currentMode");
            ::Focus.read(energy_saving_mode);  // change the stability mode state (On / Off)
        }
    }

    return EventHandlerResult::EVENT_CONSUMED;
}

}  // namespace plugin
}  //  namespace kaleidoscope

kaleidoscope::plugin::EnergyManager EnergyManager;
