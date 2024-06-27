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
    Example of use 1:
        #include "Run_task_once.h"

        void my_task(void)
        {
            NRF_LOG_DEBUG("Executing task only once...");
            NRF_LOG_FLUSH();
        }

        Run_task_once important_task(my_task);

        void main()
        {
            while (1)
            {
                important_task.run_task_once();
            }
        }

    Example of use 2:
        If the task you want to execute is a method of an object, you must
        first wrap the method in a free function.

        #include "Run_task_once.h"

        class My_class
        {
          public:
            void my_method_task(void)
            {
                NRF_LOG_DEBUG("Executing method only once...");
                NRF_LOG_FLUSH();
            }
        };

        My_class my_object;

        void my_task(void)
        {
            my_object.my_method_task();
        }

        Run_task_once important_task(my_task);

        void main()
        {
            while (1)
            {
                important_task.run_task_once();  // To rerun this task once, you can call somewhere the its reset() method.
            }
        }
*/

#ifndef _RUN_TASK_ONCE_H_
#define _RUN_TASK_ONCE_H_


// Definimos el tipo FuncType como un puntero a función que retorna void y no toma argumentos.
using func_type_t = void (*)(void);


class Run_task_once
{
  public:
    Run_task_once(func_type_t task) : _task(task) {}

    bool run_task_once(void)
    {
        if (flag)
        {
            flag = false;
            _task();

            return true;
        }

        return false;
    }

    void reset(void)
    {
        flag = true;
    }

  private:
    bool flag = true;
    func_type_t _task;
};


#endif // _RUN_TASK_ONCE_H_
