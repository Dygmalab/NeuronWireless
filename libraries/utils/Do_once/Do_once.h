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

        #include "Do_once.h"

        Do_once send_msg_code;

        void main()
        {
            while (1)
            {
                if (send_msg_code.do_once())  // The code inside this if() will be executed only once.
                {
                    // To rerun this code once, you can call somewhere its reset() method.

                    NRF_LOG_DEBUG("This message will be printed only once");
                    NRF_LOG_FLUSH();
                }
            }
        }
*/

#ifndef _DO_ONCE_H_
#define _DO_ONCE_H_


class Do_once
{
  public:
    bool do_once(void)
    {
        if (flag)
        {
            flag = false;

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
};


#endif // _DO_ONCE_H_
