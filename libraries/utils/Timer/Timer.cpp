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

/*
    Example of use:

        #include "Timer.h"

        // You can choose the unit in milliseconds or microseconds, to have better resolution.
        Timer timer_send_msg(UNIT_MILLISECONDS);

        void main()
        {
            // Every time the timer is started, it resets and starts counting from zero.
            // If this method is not called after creating the object, the timer will fire
            // inmediatly the first time the has_ended() method is called.
            timer_send_msg.start();

            while (1)
            {
                if (timer_send_msg.has_ended(1500))
                {
                    NRF_LOG_DEBUG("Hello");
                    NRF_LOG_FLUSH();

                    //timer_send_msg.stop();  // One shoot timer.
                    timer_send_msg.start();  // Cyclic timer.
                }
            }
        }
*/

#include "Timer.h"


// NRF52833
// --------------------------------
#ifdef __cplusplus
extern "C"
{
#endif

#include "Time_counter.h"

#ifdef __cplusplus
}
#endif

#define TIME_WINDOW_GET_MILLIS_FUNC()     timer_counter_get_millis()
#define TIME_WINDOW_GET_MICROS_FUNC()     timer_counter_get_micros()
// --------------------------------


// RP2040
// --------------------------------
/*#include "pico/stdlib.h"

#define TIME_WINDOW_GET_MILLIS_FUNC()   to_ms_since_boot(get_absolute_time())
#define TIME_WINDOW_GET_MICROS_FUNC()   to_us_since_boot(get_absolute_time())*/
// --------------------------------


void Timer::start(void)
{
    /*
        Starts the timer counting from zero.
        If this method is not called after creating the object, the timer will fire
        inmediatly the first time the has_ended() method is called.
    */

    flag_stop_timer = false;
    ti = get_time();
}

void Timer::stop(void)
{
    /*
        Immediately stops the timer.
        This method can also be used to create a one shot timer.
        The timer can be activated again by calling start().
    */

    flag_stop_timer = true;
}

bool Timer::has_ended(uint64_t duration)
{
    if ( (get_time() - ti >= duration) &&
        !flag_stop_timer )
    {
        return true;
    }

    return false;
}

uint64_t Timer::get_elapsed_time(void)
{
    /*
        Returns the time since the start() method was last called.
    */

    return (get_time() - ti);
}

uint64_t Timer::get_remaining_time(uint64_t duration)
{
    /*
        Returns the time remaining for the timer to activate.
    */

    return (duration - get_time());
}

uint64_t Timer::get_time(void)
{
    if (_time_unit == UNIT_MILLISECONDS)
    {
        return TIME_WINDOW_GET_MILLIS_FUNC();
    }
    else if (_time_unit == UNIT_MICROSECONDS)
    {
        return TIME_WINDOW_GET_MICROS_FUNC();
    }

    return 0;
}