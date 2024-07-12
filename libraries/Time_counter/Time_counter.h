/*
 * Time Counter -- Class to count time in milliseconds and microseconds.
 *
 * You can set the resolution in microseconds, which is not recommended
 * to be less than 50us or 100us due to the error caused by the delay
 * due to the execution of the code itself.
 *
 * You can set the timer used and the capture/compare unit used through
 * the corresponding #defines. You can also set the counter frequency if you wish.
 *
 * The MIT License (MIT)
 * 
 * Copyright (C) 2022  Dygma Lab S.L.
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
 *
 */

#ifndef __TIME_COUNTER_NRF__
#define __TIME_COUNTER_NRF__

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_drv_timer.h"

#ifdef __cplusplus
}
#endif

// Set parameters:
#define TIMER_NUMBER 4                         // Timer used for generate the compare events for increment the millis and micros counters.
#define TIMER_CC_CHANNEL NRF_TIMER_CC_CHANNEL0 // CC channel used for generate the millis interrupt: NRF_TIMER_CC_CHANNEL0, NRF_TIMER_CC_CHANNEL1, ..

#define TIMER_FREQUENCY NRF_TIMER_FREQ_16MHz
#define TIMER_FREQUENCY_IN_MHZ 16

#if (TIMER_COUNT_TIME_CC_CHANNEL == NRF_TIMER_CC_CHANNEL0)
#define TIMER_COMPARE_EVENT_NUMBER NRF_TIMER_EVENT_COMPARE0
#define TIMER_CLEAR NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK
#elif (TIMER_COUNT_TIME_CC_CHANNEL == NRF_TIMER_CC_CHANNEL1)
#define TIMER_COMPARE_EVENT_NUMBER NRF_TIMER_EVENT_COMPARE1
#define TIMER_CLEAR NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK
#elif (TIMER_COUNT_TIME_CC_CHANNEL == NRF_TIMER_CC_CHANNEL2)
#define TIMER_COMPARE_EVENT_NUMBER NRF_TIMER_EVENT_COMPARE2
#define TIMER_CLEAR NRF_TIMER_SHORT_COMPARE2_CLEAR_MASK
#elif (TIMER_COUNT_TIME_CC_CHANNEL == NRF_TIMER_CC_CHANNEL3)
#define TIMER_COMPARE_EVENT_NUMBER NRF_TIMER_EVENT_COMPARE3
#define TIMER_CLEAR NRF_TIMER_SHORT_COMPARE3_CLEAR_MASK
#elif (TIMER_COUNT_TIME_CC_CHANNEL == NRF_TIMER_CC_CHANNEL4)
#define TIMER_COMPARE_EVENT_NUMBER NRF_TIMER_EVENT_COMPARE4
#define TIMER_CLEAR NRF_TIMER_SHORT_COMPARE4_CLEAR_MASK
#elif (TIMER_COUNT_TIME_CC_CHANNEL == NRF_TIMER_CC_CHANNEL5)
#define TIMER_COMPARE_EVENT_NUMBER NRF_TIMER_EVENT_COMPARE5
#define TIMER_CLEAR NRF_TIMER_SHORT_COMPARE5_CLEAR_MASK
#endif

void timer_counter_init(uint32_t micros_resolution);
uint64_t timer_counter_get_millis(void);
uint64_t timer_counter_get_micros(void);

#endif // __TIME_COUNTER_NRF__