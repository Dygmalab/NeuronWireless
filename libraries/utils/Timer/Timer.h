/*
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
