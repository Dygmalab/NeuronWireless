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

#ifndef _TIME_WINDOW_H_
#define _TIME_WINDOW_H_


#include "stdint.h"


typedef enum
{
    UNIT_MILLISECONDS,
    UNIT_MICROSECONDS
} time_window_unit_t;

class Timer
{
  public:
    Timer(time_window_unit_t time_unit) : _time_unit(time_unit) {}

    void start(void);
    bool has_ended(uint64_t duration);
    void stop(void);

    uint64_t get_elapsed_time(void);
    uint64_t get_remaining_time(uint64_t duration);

  private:
    time_window_unit_t _time_unit;
    uint64_t ti = 0;
    bool flag_stop_timer = false;

    uint64_t get_time(void);
};


#endif  // _TIME_WINDOW_H_
