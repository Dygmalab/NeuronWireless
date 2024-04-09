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
