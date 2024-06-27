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

#include "Time_counter.h"

static uint64_t global_micros_resolution;
static uint64_t global_microseconds = 0;

static uint64_t micros_resolution;
static nrf_drv_timer_t driver_timer = NRF_DRV_TIMER_INSTANCE(TIMER_NUMBER);

// Handler for driver_timer events.
static void timer_event_handler(nrf_timer_event_t event_type, void *p_context)
{
    // There is an interrupt every micros_resolution microseconds.
    if (event_type == TIMER_COMPARE_EVENT_NUMBER)
    {
        global_microseconds += global_micros_resolution;
    }
}

static uint64_t timer_counter_calc_compare_value(void)
{
    /*
        timer_period x compare_value = timer_resolution

    => compare_value = micros_resolution [us] * timer_frequency [MHz]
                        */
    return (micros_resolution * TIMER_FREQUENCY_IN_MHZ);
}

void timer_counter_init(uint32_t micros_resolution_)
{
    static bool already_inited = false;

    if (already_inited)
    {
        return;
    }
    already_inited = true;

    micros_resolution = micros_resolution_;
    global_micros_resolution = micros_resolution;

    nrf_drv_timer_config_t timer_config;

    /*
        The "frequency" parameter here is actually the prescaler value, and the
        timer runs at the following frequency: f = 16MHz / 2^prescaler.
    */
    timer_config.frequency = TIMER_FREQUENCY;
    timer_config.mode = NRF_TIMER_MODE_TIMER;        // NRF_TIMER_MODE_COUNTER
    timer_config.bit_width = NRF_TIMER_BIT_WIDTH_32; // NRF_TIMER_BIT_WIDTH_8, NRF_TIMER_BIT_WIDTH_16, NRF_TIMER_BIT_WIDTH_24, NRF_TIMER_BIT_WIDTH_32
    timer_config.interrupt_priority = APP_IRQ_PRIORITY_LOW_MID;
    timer_config.p_context = NULL;

    ret_code_t ret = nrf_drv_timer_init(&driver_timer, &timer_config, timer_event_handler);
    APP_ERROR_CHECK(ret);

    nrf_drv_timer_extended_compare(&driver_timer, TIMER_CC_CHANNEL, timer_counter_calc_compare_value(), TIMER_CLEAR, true);

    nrf_drv_timer_enable(&driver_timer);
}

uint64_t timer_counter_get_micros(void)
{
    return global_microseconds;
}

uint64_t timer_counter_get_millis(void)
{
    return (global_microseconds / 1000);
}