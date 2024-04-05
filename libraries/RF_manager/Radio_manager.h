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

#pragma once

#include "kaleidoscope/plugin.h"
#include "kaleidoscope/Runtime.h"
#include <Kaleidoscope-EEPROM-Settings.h>
#include <Kaleidoscope-Ranges.h>
#include <Arduino.h>


namespace kaleidoscope {
namespace plugin {
class RadioManager : public Plugin {
   public:
    EventHandlerResult onSetup();
    static void init();
    static void poll();
    EventHandlerResult onFocusEvent(const char *command);

    static bool isEnabled();
    static bool isInited();
  private:

    enum Power :uint8_t {
         LOW_P,
         MEDIUM_P,
         HIGH_P,
     };
    static bool inited;
    static uint16_t channel_hop;
    static Power power_rf;
    static uint16_t settings_base_;
    static void setPowerRF();
};

}  // namespace plugin
}  // namespace kaleidoscope

extern kaleidoscope::plugin::RadioManager RadioManager;