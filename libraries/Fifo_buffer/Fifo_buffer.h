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

#ifndef __FIFO_BUFFER_H__
#define __FIFO_BUFFER_H__


#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_drv_spis.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif


#define FIFO_BUFFER_DEBUG       0

#define INTERNAL_BUFFER_SIZE    3000  // item_size x number of items of the FIFO in Bytes.


class Fifo_buffer
{
    public:
        Fifo_buffer(size_t _item_size) : item_size(_item_size)
        {
            memset(internal_buffer, 0, INTERNAL_BUFFER_SIZE);
        };

        bool put(const void *cell);
        size_t get(void *cell);
        size_t peek(void *cell);
        size_t removeOne();

        bool is_empty(void);
        bool is_full(void);
        size_t get_num_items(void);

    private:
        size_t item_size;

        uint8_t internal_buffer[INTERNAL_BUFFER_SIZE];  // item_size x number of items of the FIFO Bytes.
        int32_t index = 0;
        int32_t num_items = 0;

        void forward_fifo(void);
};


#endif  // __FIFO_BUFFER_H__
