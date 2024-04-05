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
#pragma once
#include "kaleidoscope/plugin.h"
#include "kaleidoscope/Runtime.h"
#include <Kaleidoscope-EEPROM-Settings.h>
#include <Kaleidoscope-Ranges.h>
#include <Arduino.h>

namespace kaleidoscope {
namespace plugin {
class Battery : public Plugin {
   public:
    EventHandlerResult onSetup();
    EventHandlerResult onFocusEvent(const char *command);
    EventHandlerResult onKeyswitchEvent(Key &mapped_Key, KeyAddr key_addr, uint8_t key_state);

   private:
    static uint8_t battery_level;
    static uint8_t saving_mode;
    static uint16_t settings_saving_;
    static uint8_t status_left;
    static uint8_t status_right;
    static uint8_t battery_level_left;
    static uint8_t battery_level_right;
};

}  // namespace plugin
}  // namespace kaleidoscope

extern kaleidoscope::plugin::Battery Battery;