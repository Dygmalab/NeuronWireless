/* -*- mode: c++ -*-
 * kaleidoscope::plugin::RaiseFirmwareVersion -- Tell the firmware version via Focus
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
 */

#include "DefyFirmwareVersion.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope.h"
#include "nrf_log.h"

#ifndef Defy_FIRMWARE_VERSION
#define Defy_FIRMWARE_VERSION "v0.0.0"
#endif

namespace kaleidoscope
{
namespace plugin
{

EventHandlerResult FirmwareVersion::onFocusEvent(const char *command)
{
    const char *cmd = "version";
    if (::Focus.handleHelp(command, cmd)) return EventHandlerResult::OK;

    if (strcmp(command, cmd) != 0) return EventHandlerResult::OK;

    NRF_LOG_DEBUG("read request: version");

    char cstr[70];
    strcpy(cstr, Defy_FIRMWARE_VERSION);
    ::Focus.sendRaw<char *>(cstr);

    return EventHandlerResult::EVENT_CONSUMED;
}

} // namespace plugin
} // namespace kaleidoscope

kaleidoscope::plugin::FirmwareVersion FirmwareVersion;
