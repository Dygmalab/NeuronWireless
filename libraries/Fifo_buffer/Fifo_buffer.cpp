#include "Fifo_buffer.h"

#include "string.h"


bool Fifo_buffer::put(const void *item)
{
    if ( (index + item_size) <= INTERNAL_BUFFER_SIZE )  // If there is place in the buffer.
    {
        memcpy(internal_buffer + index, item, item_size);
        index += item_size;
        num_items++;

        #if FIFO_BUFFER_DEBUG
        NRF_LOG_DEBUG("FIFO put ok, num_items = %d", num_items);
        NRF_LOG_DEBUG("FIFO put ok, index = %d", index);
        for (uint32_t i = 0; i < INTERNAL_BUFFER_SIZE; i++)
        {
            if ( i == (index - item_size) )
            {
                NRF_LOG_DEBUG("--> internal_buffer[%d] = %d", i, internal_buffer[i]);
            }
            else
            {
                NRF_LOG_DEBUG("internal_buffer[%d] = %d", i, internal_buffer[i]);
            }
        }
        NRF_LOG_DEBUG("--------");
        #endif

        return true;
    }

    #if FIFO_BUFFER_DEBUG
    NRF_LOG_DEBUG("FIFO full");
    #endif

    return false;  // Fifo full.
}

size_t Fifo_buffer::get(void *item)
{
    memset(item, 0, item_size);

    if (index == 0)
    {
        #if FIFO_BUFFER_DEBUG
        NRF_LOG_DEBUG("FIFO empty");
        #endif
        
        return 0;
    }

    memcpy(item, internal_buffer, item_size);  // Reads item.
    forward_fifo();                            // Moves the fifo cells one place forward.
    index -= item_size;
    num_items--;

    #if FIFO_BUFFER_DEBUG
    NRF_LOG_DEBUG("FIFO get ok, num_items = %d", num_items);
    NRF_LOG_DEBUG("FIFO get ok, index = %d", index);
    for (uint32_t i = 0; i < INTERNAL_BUFFER_SIZE; i++)
    {
        NRF_LOG_DEBUG("internal_buffer[%d] = %d", i, internal_buffer[i]);
    }
    NRF_LOG_FLUSH();
    #endif

    return item_size;
}

size_t Fifo_buffer::removeOne()
{
    forward_fifo();                            // Moves the fifo cells one place forward.
    index -= item_size;
    num_items--;
    return item_size;
}

size_t Fifo_buffer::peek(void *item)
{
    memset(item, 0, item_size);

    if (index == 0)
    {
#if FIFO_BUFFER_DEBUG
        NRF_LOG_DEBUG("FIFO empty");
#endif

        return 0;
    }

    memcpy(item, internal_buffer, item_size);  // Reads item.

#if FIFO_BUFFER_DEBUG
    NRF_LOG_DEBUG("FIFO get ok, num_items = %d", num_items);
    NRF_LOG_DEBUG("FIFO get ok, index = %d", index);
    for (uint32_t i = 0; i < INTERNAL_BUFFER_SIZE; i++)
    {
        NRF_LOG_DEBUG("internal_buffer[%d] = %d", i, internal_buffer[i]);
    }
    NRF_LOG_FLUSH();
#endif

    return item_size;
}

void Fifo_buffer::forward_fifo(void)
{
    // Moves the fifo cells one place forward.
    int32_t count = 0;
    for (int32_t i = 0; i < (num_items - 1); i++)   // Moves num_items.
    {
        for (size_t j = 0; j < item_size; j++)     // Moves the whole item.
        {
            internal_buffer[count] = internal_buffer[item_size + count];
            count++;
        }
    }
    
    memset(&internal_buffer[count], 0, item_size);  // Clear the last item.
}

bool Fifo_buffer::is_empty(void)
{
    if (index <= 0) return true;

    return false;
}

bool Fifo_buffer::is_full(void)
{
    // If there is no room in the FIFO for one more item, then it is full.
    if ( (index + item_size) <= INTERNAL_BUFFER_SIZE ) return false;
    
    return true;
}

size_t Fifo_buffer::get_num_items(void)
{
    return num_items;
}
