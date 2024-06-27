/*
 * Class to manage the Neuron 2 MCU watchdog.
 *
 * First, you need to enable the watchdog with the NRFX_WDT_ENABLED flag in the sdk_config.h file
 * and set the timeout in [ms] with NRFX_WDT_CONFIG_RELOAD_VALUE.
 *
 * Copyright (C) 2022  Dygma Lab S.L.
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
 * Author: Juan Hauara @JuanHauara
 *
 */

#ifndef __WATCHDOG_TIMER_H__
#define __WATCHDOG_TIMER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_drv_wdt.h"

// UART debug log
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif


class Watchdog_timer
{
    public:
        void init(void);
        void reset(void);

    private:
        nrf_drv_wdt_channel_id wdt_channel_id;
        bool inited;
};


#endif  // __WATCHDOG_TIMER_H__