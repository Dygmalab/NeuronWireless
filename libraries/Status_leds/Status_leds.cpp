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

#include "Status_leds.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_gpio.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif

Status_leds::Status_leds(uint16_t green_led_pin, uint16_t red_led_pin)
{
    _green_led_pin = green_led_pin;
    _red_led_pin = red_led_pin;

    pwm_instance = NRFX_PWM_INSTANCE(1);

    pwm_config =
        {
            .output_pins =
                {
                    _green_led_pin,        // channel 0
                    _red_led_pin,          // channel 1
                    NRFX_PWM_PIN_NOT_USED, // channel 2
                    NRFX_PWM_PIN_NOT_USED  // channel 3
                },
            .irq_priority = APP_IRQ_PRIORITY_LOWEST,
            .base_clock = NRF_PWM_CLK_125kHz,
            .count_mode = NRF_PWM_MODE_UP,
            .top_value = STATUS_LEDS_PWM_MAX_COUNT,
            .load_mode = NRF_PWM_LOAD_INDIVIDUAL,
            .step_mode = NRF_PWM_STEP_AUTO};
};

void Status_leds::init(void)
{
    nrfx_pwm_init(&pwm_instance, &pwm_config, NULL);
}

void Status_leds::green_fade(uint8_t min_brightness, uint8_t max_brightness)
{
    if (flag_green_fade_running)
        return;

    flag_green_static_running = false;
    flag_green_fade_running = true;
    flag_red_fade_running = false;
    flag_yellow_fade_running = false;
    flag_red_green_fade_alternate_running = false;

    nrfx_pwm_stop(&pwm_instance, true);

    int32_t max = ((100 - max_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t min = ((100 - min_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t delta = max - min;
    if (delta < 0)
        delta = -delta;
    int32_t step = delta / (STATUS_LEDS_FADE_RESOLUTION / 2);

    //    NRF_LOG_INFO("max = %d", max);
    //    NRF_LOG_INFO("min = %d", min);
    //    NRF_LOG_INFO("delta = %d", delta);
    //    NRF_LOG_INFO("step = %d", step);

    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION / 2; i++)
    {
        pwm_values[i].channel_0 = min - step * i;
        pwm_values[i].channel_1 = STATUS_LEDS_PWM_MAX_COUNT; // Constant duty cycle.

        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_0 = pwm_values[i].channel_0;   // It reflects the values for the decreasing part.
        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_1 = STATUS_LEDS_PWM_MAX_COUNT; // Constant duty cycle.
    }

    //    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION; i++)
    //    {
    //        NRF_LOG_INFO("pwm_values[%d].channel_0 = %d", i, pwm_values[i].channel_0);
    //        NRF_LOG_FLUSH();
    //    }

    pwm_sequence =
        {
            .values = {.p_individual = pwm_values},
            .length = NRF_PWM_VALUES_LENGTH(pwm_values),
            .repeats = 0,
            .end_delay = 0};

    nrfx_pwm_simple_playback(&pwm_instance, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void Status_leds::static_green(uint8_t brightness)
{
    if (flag_green_static_running)
        return;

    flag_green_static_running = true;
    flag_green_fade_running = false;
    flag_red_fade_running = false;
    flag_yellow_fade_running = false;
    flag_red_green_fade_alternate_running = false;

    nrfx_pwm_stop(&pwm_instance, true);

    uint16_t max = STATUS_LEDS_PWM_MAX_COUNT - STATUS_LEDS_PWM_MAX_COUNT * ((float)brightness / (float)255);
    pwm_values_static[0].channel_0 = max;
    pwm_values_static[0].channel_1 = STATUS_LEDS_PWM_MAX_COUNT; // Constant duty cycle.

    pwm_sequence =
        {
            .values = {.p_individual = pwm_values_static},
            .length = NRF_PWM_VALUES_LENGTH(pwm_values_static),
            .repeats = 0,
            .end_delay = 0};

    nrfx_pwm_simple_playback(&pwm_instance, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void Status_leds::red_fade(uint8_t min_brightness, uint8_t max_brightness)
{
    if (flag_red_fade_running)
        return;

    flag_green_static_running = false;
    flag_green_fade_running = false;
    flag_red_fade_running = true;
    flag_yellow_fade_running = false;
    flag_red_green_fade_alternate_running = false;

    nrfx_pwm_stop(&pwm_instance, true);

    int32_t max = ((100 - max_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t min = ((100 - min_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t delta = max - min;
    if (delta < 0)
        delta = -delta;
    int32_t step = delta / (STATUS_LEDS_FADE_RESOLUTION / 2);

    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION / 2; i++)
    {
        pwm_values[i].channel_0 = STATUS_LEDS_PWM_MAX_COUNT;
        pwm_values[i].channel_1 = min - step * i; // Constant duty cycle.

        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_0 = STATUS_LEDS_PWM_MAX_COUNT; // It reflects the values for the decreasing part.
        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_1 = pwm_values[i].channel_1;   // Constant duty cycle.
    }

    pwm_sequence =
        {
            .values = {.p_individual = pwm_values},
            .length = NRF_PWM_VALUES_LENGTH(pwm_values),
            .repeats = 0,
            .end_delay = 0};

    nrfx_pwm_simple_playback(&pwm_instance, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void Status_leds::yellow_fade(uint8_t min_brightness, uint8_t max_brightness)
{
    if (flag_yellow_fade_running)
        return;

    flag_green_static_running = false;
    flag_green_fade_running = false;
    flag_red_fade_running = false;
    flag_yellow_fade_running = true;
    flag_red_green_fade_alternate_running = false;

    nrfx_pwm_stop(&pwm_instance, true);

    int32_t max = ((100 - max_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t min = ((100 - min_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t delta = max - min;
    if (delta < 0)
        delta = -delta;
    int32_t step = delta / (STATUS_LEDS_FADE_RESOLUTION / 2);

    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION / 2; i++)
    {
        pwm_values[i].channel_0 = min - step * i;
        pwm_values[i].channel_1 = min - step * i; // Constant duty cycle.

        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_0 = pwm_values[i].channel_0; // It reflects the values for the decreasing part.
        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_1 = pwm_values[i].channel_1; // Constant duty cycle.
    }

    pwm_sequence =
        {
            .values = {.p_individual = pwm_values},
            .length = NRF_PWM_VALUES_LENGTH(pwm_values),
            .repeats = 0,
            .end_delay = 0};

    nrfx_pwm_simple_playback(&pwm_instance, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void Status_leds::all_colors_fade(uint8_t min_brightness, uint8_t max_brightness)
{
    if (flag_red_green_fade_alternate_running)
        return;

    flag_green_static_running = false;
    flag_green_fade_running = false;
    flag_red_fade_running = false;
    flag_yellow_fade_running = false;
    flag_red_green_fade_alternate_running = true;

    nrfx_pwm_stop(&pwm_instance, true);

    int32_t max = ((100 - max_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t min = ((100 - min_brightness) * STATUS_LEDS_PWM_MAX_COUNT) / 100;
    int32_t delta = max - min;
    if (delta < 0)
        delta = -delta;
    int32_t step = delta / (STATUS_LEDS_FADE_RESOLUTION / 2);

    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION / 2; i++)
    {
        pwm_values[i].channel_0 = min - step * i;
        pwm_values[STATUS_LEDS_FADE_RESOLUTION / 2 - i - 1].channel_1 = pwm_values[i].channel_0;

        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_0 = pwm_values[i].channel_0; // It reflects the values for the decreasing part.
    }

    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION / 2; i++)
    {
        pwm_values[STATUS_LEDS_FADE_RESOLUTION - i - 1].channel_1 = pwm_values[i].channel_1; // It reflects the values for the decreasing part.
    }

    //    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION; i++)
    //    {
    //        NRF_LOG_INFO("pwm_values[%d].channel_0 = %d", i, pwm_values[i].channel_0);
    //        NRF_LOG_FLUSH();
    //    }
    //    NRF_LOG_INFO("==============================");
    //    for (uint16_t i = 0; i < STATUS_LEDS_FADE_RESOLUTION; i++)
    //    {
    //        NRF_LOG_INFO("pwm_values[%d].channel_1 = %d", i, pwm_values[i].channel_1);
    //        NRF_LOG_FLUSH();
    //    }

    pwm_sequence =
        {
            .values = {.p_individual = pwm_values},
            .length = NRF_PWM_VALUES_LENGTH(pwm_values),
            .repeats = 0,
            .end_delay = 0};

    nrfx_pwm_simple_playback(&pwm_instance, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}
void Status_leds::stop_all()
{
    flag_green_static_running = false;
    flag_green_fade_running = false;
    flag_red_fade_running = false;
    flag_yellow_fade_running = false;
    flag_red_green_fade_alternate_running = false;
    nrfx_pwm_stop(&pwm_instance, true);
}
