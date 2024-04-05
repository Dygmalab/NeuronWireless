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
 * Date: 08/22/2022
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