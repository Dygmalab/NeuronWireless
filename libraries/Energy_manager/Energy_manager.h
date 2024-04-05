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
#include "kaleidoscope/plugin.h"
#include "kaleidoscope/Runtime.h"
#include <Kaleidoscope-EEPROM-Settings.h>
#include <Kaleidoscope-Ranges.h>
#include <Arduino.h>

namespace kaleidoscope {
namespace plugin {
class EnergyManager : public Plugin {
   public:
    EventHandlerResult onSetup();
    EventHandlerResult onFocusEvent(const char *command);
    EventHandlerResult onKeyswitchEvent(Key &mapped_Key, KeyAddr key_addr, uint8_t key_state);

   private:
    static uint8_t energy_saving_mode;
    static uint8_t energy_mode_disable;
    static uint16_t storage_base_;
    static uint16_t storage_size_;
    static uint8_t energy_saving_disable;
};

}  // namespace plugin
}  // namespace kaleidoscope

extern kaleidoscope::plugin::EnergyManager EnergyManager;