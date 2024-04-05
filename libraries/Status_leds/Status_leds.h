/*
 * Defy Neuron Status LEDs -- Report to the user the current status of the device via LED.
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

#ifndef __STATUS_LEDS__
#define __STATUS_LEDS__

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_pwm.h"
#include "nrfx_pwm.h"

#ifdef __cplusplus
}
#endif

#define STATUS_LEDS_FADE_RESOLUTION 100
#define STATUS_LEDS_PWM_MAX_COUNT 2500

class Status_leds
{
public:
    Status_leds(uint16_t green_led_pin, uint16_t red_led_pin);

    void init();

    void green_fade(uint8_t min_brightness, uint8_t max_brightness);
    void static_green(uint8_t brightness);
    void stop_all();
    void red_fade(uint8_t min_brightness, uint8_t max_brightness);
    void yellow_fade(uint8_t min_brightness, uint8_t max_brightness);
    void all_colors_fade(uint8_t min_brightness, uint8_t max_brightness);

private:
    uint8_t _green_led_pin;
    uint8_t _red_led_pin;

    nrfx_pwm_t pwm_instance;
    nrfx_pwm_config_t pwm_config;

    nrf_pwm_values_individual_t pwm_values[STATUS_LEDS_FADE_RESOLUTION];
    nrf_pwm_values_individual_t pwm_values_static[1];
    nrf_pwm_sequence_t pwm_sequence;

    bool flag_green_fade_running = false;
    bool flag_green_static_running = false;
    bool flag_red_fade_running = false;
    bool flag_yellow_fade_running = false;
    bool flag_red_green_fade_alternate_running = false;
};

#endif // __STATUS_LEDS__