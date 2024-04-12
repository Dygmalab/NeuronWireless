/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "RawHID.h"
#include "Adafruit_TinyUSB.h"
#include "Ble_composite_dev.h"
#include "ble_hid_service.h"
#include "hidDefy.h"

tu_fifo_t rx_ff;
tu_fifo_t tx_ff;

uint8_t rx_ff_buf[OUTPUT_REPORT_LEN_RAW*50];//UP to 50 messages
uint8_t tx_ff_buf[INPUT_REPORT_LEN_RAW*50];//Up to 50 messages


RawHID_ RawHID;


int RawHID_::peek(void)
{
    uint8_t chr;
    return tu_fifo_peek(&rx_ff, &chr) ? (int)chr : -1;
}

int RawHID_::read(void)
{
    uint8_t ch;
    return tu_fifo_read_n(&rx_ff, &ch, 1) ? (int32_t)ch : -1;
}

size_t RawHID_::read(uint8_t *buffer, size_t size)
{
    return tu_fifo_read_n(&rx_ff, buffer, (uint16_t) size);
}

void RawHID_::flush(void)
{
    if(!ble_connected()) return;
    uint16_t size = tu_fifo_count(&tx_ff);
    if (size ==0) return;
    uint8_t buff[INPUT_REPORT_LEN_RAW];
    memset(buff,0,INPUT_REPORT_LEN_RAW);
    uint16_t i = tu_fifo_peek_n(&tx_ff, buff, INPUT_REPORT_LEN_RAW);
    if (i != 0)
    {
        if (ble_send_report(HID_REPORTID_RAWHID, buff, INPUT_REPORT_LEN_RAW)){
            tu_fifo_advance_read_pointer(&tx_ff, i);
        }
    }
    //Send report each time
}

size_t RawHID_::write(uint8_t ch)
{
    return write(&ch, 1);
}

size_t RawHID_::write(const uint8_t *buffer, size_t size)
{
    if(!ble_connected()) return 0;
    return tu_fifo_write_n(&tx_ff, buffer, size);
}

int RawHID_::availableForWrite(void)
{
    return ble_connected();
}

RawHID_::~RawHID_()
{
}

int RawHID_::available(void)
{
    return tu_fifo_count(&rx_ff);
}

void RawHID_::begin()
{
    tu_fifo_config(&rx_ff, rx_ff_buf, TU_ARRAY_SIZE(rx_ff_buf), 1, false);
    tu_fifo_config(&tx_ff, tx_ff_buf, TU_ARRAY_SIZE(tx_ff_buf), 1, true);
}


size_t strnlen_s(const char *s, size_t maxsize)
{
    if (s == NULL) {
        return 0;
    }
    
    size_t count = 0;

    while (*s++ && maxsize--) {
        count++;
    }

    return count;
}

extern "C"
{

    bool callBackRawHID(uint8_t *buff)
    {
        size_t size = strnlen_s((char *)buff, OUTPUT_REPORT_LEN_RAW);
        tu_fifo_write_n(&rx_ff, buff, (uint16_t) size);
        return false;
    }
}
