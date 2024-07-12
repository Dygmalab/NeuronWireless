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