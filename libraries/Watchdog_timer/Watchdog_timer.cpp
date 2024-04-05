/*
 * Time Counter -- Class to count time in milliseconds and microseconds.
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
 * Author:
 * Date:
 *
 */

#include "Watchdog_timer.h"

void watchdog_event_handler(void)
{
    /*
        NOTE: The max amount of time we can spend in WDT interrupt is
        two cycles of 32768[Hz] clock - after that, reset occurs.
    */
}

void Watchdog_timer::init(void)
{
#if NRFX_WDT_ENABLED

    // Configure WDT.
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;

    ret_code_t err_code = nrf_drv_wdt_init(&config, watchdog_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_drv_wdt_channel_alloc(&wdt_channel_id);
    APP_ERROR_CHECK(err_code);

    nrf_drv_wdt_enable();
    inited = true;
#endif
}

void Watchdog_timer::reset(void)
{
#if NRFX_WDT_ENABLED
    if (inited)
    {
        nrf_drv_wdt_channel_feed(wdt_channel_id);
    }
#endif
}
