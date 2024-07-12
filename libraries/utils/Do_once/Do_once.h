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
