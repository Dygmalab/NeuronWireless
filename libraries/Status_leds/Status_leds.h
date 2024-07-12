/*
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