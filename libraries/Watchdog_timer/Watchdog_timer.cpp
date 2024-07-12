/*
 *
 * Class to manage the Neuron 2 MCU watchdog.
 *
 * First, you need to enable the watchdog with the NRFX_WDT_ENABLED flag in the sdk_config.h file
 * and set the timeout in [ms] with NRFX_WDT_CONFIG_RELOAD_VALUE.
 * 
 * The MIT License (MIT)
 * 
 * Copyright (C) 2020  Dygma Lab S.L.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Author: Juan Hauara @JuanHauara
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
