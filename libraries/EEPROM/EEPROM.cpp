/*
    EEPROM.cpp - RP2040 EEPROM emulation
    Copyright (c) 2021 Earle F. Philhower III. All rights reserved.

    Based on ESP8266 EEPROM library, which is
    Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "EEPROM.h"

#include "kaleidoscope/Runtime.h"
#include "Arduino.h"
#include "nrf_delay.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_fstorage.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_fstorage_sd.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#else
#include "nrf_drv_clock.h"
#include "nrf_fstorage_nvmc.h"
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif


#define FLASH_STORAGE_DEBUG_READ 1
#define FLASH_STORAGE_DEBUG_ERASE_PAGE 1
#define FLASH_STORAGE_DEBUG_WRITE 1

/*
    The NRF52833 has 128 pages of 4KB each one.
    4KB = 0x1000 in hex.
    Pages are numbered from 0 to 127.
    Start address of the last page (127) = 0x0007F000.
    Last address of the last page = 512 * 1024 Bytes = 524288 Bytes = 0x00080000

    Bootloader starts in 0x00077000 (page 119) and ends in 0x0007DF53 (page 125), 7 pages in total.
    We reserve 2 pages starting from 0x00075000 for the EEPROM class.
    The Bluetooth module uses the Nordic FDS library, and 3 pages are reserved for it (pages 114,
    115 and 116).
    Pages 39 to 113 are free to be used by the main application.

    Flash memory map:
        Page 127 (0x0007F000) -> MBR parameters storage. Last memory page.

        Page 126 (0x0007E000) -> Reserved to be use by the bootloader. Bootloader settings page.
        Page 125 (0x0007D000) -> Bootloader.
        Page 124 (0x0007C000) -> Bootloader.
        Page 123 (0x0007B000) -> Bootloader.
        Page 122 (0x0007A000) -> Bootloader.
        Page 121 (0x00079000) -> Bootloader.
        Page 120 (0x00078000) -> Bootloader.
        Page 119 (0x00077000) -> Bootloader.

        Page 118 (0x00076000) -> EEPROM class. It uses fstorage library from the SDK.
        Page 117 (0x00075000) -> EEPROM class.

        Page 116 (0x00074000) -> FDS library used by the peer manager module in the Bluetooth.
        Page 115 (0x00073000) -> FDS library.
        Page 114 (0x00072000) -> FDS library.

        Page 113 (0x00071000) -> Max Main App.
        .
        .
        Page 39  (0x00027000) -> Min Main App.

        Page 38  (0x00026000) -> Softdevice S140.
        .
        .
        Page 1   (0x00001000) -> Softdevice S140.

        Page 0   (0x00000000) -> MBR.

    Future changes:
        - Reserve only two pages for FDS.
        - Locate the two pages for FDS directly below the bootloader and then the memory pages
          used by the EEPROM class. This way it is easier to add memory pages as we need them
          to be used by Kaleidoscope.
*/
#define FLASH_STORAGE_FIRST_PAGE_START_ADDR     0x00075000
#define FLASH_STORAGE_NUM_PAGES                 2
#define FLASH_STORAGE_PAGE_SIZE                 4096 /* Size of the flash pages in Bytes. */

#define LAST_PAGE_END_ADDR FLASH_STORAGE_FIRST_PAGE_START_ADDR + (FLASH_STORAGE_PAGE_SIZE * FLASH_STORAGE_NUM_PAGES) - 1


volatile static bool flag_write_completed = false;
volatile static bool flag_erase_completed = false;

static void fstorage_evt_handler(nrf_fstorage_evt_t *evt);

// Creates an fstorage instance.
NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage_instance) = {
    .evt_handler = fstorage_evt_handler,

    /*
        You must set these manually, even at runtime, before nrf_fstorage_init() is called.
        The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
        last page of flash available to write data.
    */
    .start_addr = FLASH_STORAGE_FIRST_PAGE_START_ADDR,
    .end_addr = LAST_PAGE_END_ADDR,
};

static void fstorage_evt_handler(nrf_fstorage_evt_t *p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_ERROR("EEPROM: Error while executing an fstorage operation.");
        NRF_LOG_FLUSH();

        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
#if FLASH_STORAGE_DEBUG_WRITE
            NRF_LOG_DEBUG("EEPROM: Writing completed.");
            NRF_LOG_FLUSH();
#endif

            flag_write_completed = true;
        }
        break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
#if FLASH_STORAGE_DEBUG_READ or FLASH_STORAGE_DEBUG_WRITE
            NRF_LOG_DEBUG("EEPROM: Erase completed.");
            NRF_LOG_FLUSH();
#endif

            flag_erase_completed = true;
        }
        break;

        default:
        {
        }
        break;
    }
}

void EEPROMClass::begin(size_t size)
{
    nrf_fstorage_api_t *fs_api;

#ifdef SOFTDEVICE_PRESENT
#if FLASH_STORAGE_DEBUG_READ or FLASH_STORAGE_DEBUG_WRITE
    NRF_LOG_DEBUG("EEPROM: SoftDevice is present. Using nrf_fstorage_sd driver implementation.");
    NRF_LOG_FLUSH();
#endif

    fs_api = &nrf_fstorage_sd;
#else
#if FLASH_STORAGE_DEBUG_READ or FLASH_STORAGE_DEBUG_WRITE
    NRF_LOG_DEBUG("EEPROM: SoftDevice not present. Using nrf_fstorage_nvmc driver implementation.");
    NRF_LOG_FLUSH();
#endif

    fs_api = &nrf_fstorage_nvmc;
#endif

    ret_code_t rc = nrf_fstorage_init(&fstorage_instance, fs_api, NULL);
    APP_ERROR_CHECK(rc);

    if ((size <= 0) || (size > FLASH_STORAGE_NUM_PAGES * FLASH_STORAGE_PAGE_SIZE))
    {
        size = FLASH_STORAGE_NUM_PAGES * FLASH_STORAGE_PAGE_SIZE;
    }

    _size = (size + 255) & (~255); // Flash writes limited to 256 byte boundaries

    // In case begin() is called a 2nd+ time, don't reallocate if size is the same
    if (_data && size != _size)
    {
        delete[] _data;
        _data = new uint8_t[size];

#if FLASH_STORAGE_DEBUG_READ or FLASH_STORAGE_DEBUG_WRITE
        NRF_LOG_DEBUG("EEPROM: Reserved %d Bytes.", size);
        NRF_LOG_FLUSH();
#endif
    }
    else if (!_data)
    {
        _data = new uint8_t[size];

#if FLASH_STORAGE_DEBUG_READ or FLASH_STORAGE_DEBUG_WRITE
        NRF_LOG_DEBUG("EEPROM: Reserved %d Bytes.", size);
        NRF_LOG_FLUSH();
#endif
    }

    // At startup, EEPROMclass loads all the flash memory pages it uses.
    ret_code_t ret_code = nrf_fstorage_read(&fstorage_instance, FLASH_STORAGE_FIRST_PAGE_START_ADDR, _data, size);
#if FLASH_STORAGE_DEBUG_READ
    NRF_LOG_DEBUG("EEPROM: Loaded flash memory, ret_code = %i", ret_code);
    NRF_LOG_FLUSH();
#endif

    _dirty = false; // make sure dirty is cleared in case begin() is called 2nd+ time
}

bool EEPROMClass::end(void)
{
    bool retval;

    if (!_size)
    {
        return false;
    }

    retval = commit();
    if (_data)
    {
        delete[] _data;
    }
    _data = 0;
    _size = 0;
    _dirty = false;

    return retval;
}

uint8_t EEPROMClass::read(int const address)
{
    if (address < 0 || (size_t)address >= _size)
    {
        return 0;
    }

    if (!_data)
    {
        return 0;
    }

    return _data[address];
}

void EEPROMClass::write(int const address, uint8_t const value)
{
    if (address < 0 || (size_t)address >= _size)
    {
        return;
    }

    if (!_data)
    {
        return;
    }

    if (_data[address] != value)
    {
        _data[address] = value;
        _dirty = true;  // Optimise _dirty. Only flagged if data written is different.
    }
}

bool EEPROMClass::commit(void)
{
    if (!_size)
    {
        return false;
    }

    if (!_dirty)
    {
        return true;
    }

    if (!_data)
    {
        return false;
    }

    /*
        Indicates that the buffer in RAM memory has new data that must be written to flash memory
        the next time the Update() method is executed.
    */
    needUpdate = true;

    /*
        It resets the periodic update timer to give time for the application to perform all
        the necessary writes, in case there are many, in RAM and then save them all
        together with a single write to flash memory, automatically when this timer expires.
    */
    reset_timer_update_periodically();

    return true;
}

uint8_t *EEPROMClass::getDataPtr(void)
{
    _dirty = true;

    return &_data[0];
}

uint8_t const *EEPROMClass::getConstDataPtr(void) const
{
    return &_data[0];
}

void EEPROMClass::update(void)
{
    if (!needUpdate)
    {
        return;
    }

    erase();

    while (nrf_fstorage_is_busy(NULL))  // Wait until fstorage is available.
    {
        yield();  // Meanwhile execute tasks.
    }

#if FLASH_STORAGE_DEBUG_ERASE_PAGE
    NRF_LOG_DEBUG("EEPROM: Writing flash...");
    NRF_LOG_FLUSH();
#endif
    flag_write_completed = false;
    ret_code_t ret_code = nrf_fstorage_write(&fstorage_instance,
                                             FLASH_STORAGE_FIRST_PAGE_START_ADDR,
                                             _data,
                                             _size,
                                             NULL);
    if (ret_code == NRF_SUCCESS)
    {
        /*
            The operation was accepted.
            Upon completion, the NRF_FSTORAGE_ERASE_RESULT event is sent to the callback function
            registered by the instance.

            If error, try increasing NRF_FSTORAGE_SD_MAX_RETRIES and NRF_FSTORAGE_SD_QUEUE_SIZE.
        */
        while (!flag_write_completed)
        {
            yield();  // Meanwhile execute tasks.
        }
        needUpdate = false;

        /*
            It resets the periodic update timer since we just wrote the flash memory.
            This makes sense since the update() method can be called manually by the user, in addition
            to being called all the time automatically by timer_update_periodically_run().
        */
        reset_timer_update_periodically();
    }
    else
    {
        NRF_LOG_ERROR("EEPROM: Write error, ret_code = %d", ret_code);
        NRF_LOG_FLUSH();

        /*
            If error, try increasing NRF_FSTORAGE_SD_MAX_RETRIES and NRF_FSTORAGE_SD_QUEUE_SIZE.

            Error codes:
            ret = 14 -> NRF_ERROR_NULL: If p_fs or p_src is NULL.
            ret = 8  -> NRF_ERROR_INVALID_STATE: If the module is not initialized.
            ret = 9  -> NRF_ERROR_INVALID_LENGTH: If len is zero or not a multiple of the program unit, or if it is otherwise invalid.
            ret = 16 -> NRF_ERROR_INVALID_ADDR: If the address dest is outside the flash memory boundaries specified in p_fs, or if it is unaligned.
            ret = 4  -> NRF_ERROR_NO_MEM: If no memory is available to accept the operation. When using the SoftDevice implementation, this error indicates that the
           internal queue of operations is full.
        */
    }
}

void EEPROMClass::erase(void)
{
    while (nrf_fstorage_is_busy(NULL))  // Wait until fstorage is available.
    {
        yield();  // Meanwhile execute tasks.
    }

#if FLASH_STORAGE_DEBUG_ERASE_PAGE
    NRF_LOG_DEBUG("EEPROM: Erasing flash...");
    NRF_LOG_FLUSH();
#endif
    flag_erase_completed = false;
    ret_code_t ret_code = nrf_fstorage_erase(&fstorage_instance,
                                             FLASH_STORAGE_FIRST_PAGE_START_ADDR,
                                             FLASH_STORAGE_NUM_PAGES,
                                             NULL);
    if (ret_code == NRF_SUCCESS)
    {
        /*
            The operation was accepted.
            Upon completion, the NRF_FSTORAGE_ERASE_RESULT event is sent to the callback function
            registered by the instance.

            If error, try increasing NRF_FSTORAGE_SD_MAX_RETRIES and NRF_FSTORAGE_SD_QUEUE_SIZE.
        */
        while (!flag_erase_completed)
        {
            yield();  // Meanwhile execute tasks.
        }

        return;
    }

    NRF_LOG_ERROR("EEPROM: Erase error, ret_code = %lu", ret_code);
    NRF_LOG_FLUSH();

    /*
        If error, try increasing NRF_FSTORAGE_SD_MAX_RETRIES and NRF_FSTORAGE_SD_QUEUE_SIZE.

        Error codes:
        ret = 14 -> NRF_ERROR_NULL: If p_fs or p_src is NULL.
        ret = 8  -> NRF_ERROR_INVALID_STATE: If the module is not initialized.
        ret = 9  -> NRF_ERROR_INVALID_LENGTH: If len is zero or not a multiple of the program unit, or if it is otherwise invalid.
        ret = 16 -> NRF_ERROR_INVALID_ADDR: If the address dest is outside the flash memory boundaries specified in p_fs, or if it is unaligned.
        ret = 4  -> NRF_ERROR_NO_MEM: If no memory is available to accept the operation. When using the SoftDevice implementation, this error indicates that the
       internal queue of operations is full.
    */
}

bool EEPROMClass::getNeedUpdate(void)
{
    return needUpdate;
}

void EEPROMClass::timer_update_periodically_run(uint32_t timeout_ms)
{
    if (trigger_update_periodically_timer)
    {
        trigger_update_periodically_timer = false;
        ti_periodically_update = kaleidoscope::Runtime.millisAtCycleStart();
    }

    if (kaleidoscope::Runtime.hasTimeExpired(ti_periodically_update, timeout_ms))
    {
        if (EEPROM.getNeedUpdate())
        {
#if FLASH_STORAGE_DEBUG_WRITE
            NRF_LOG_DEBUG("EEPROM: Flash updated automatically.");
            NRF_LOG_FLUSH();
#endif
            EEPROM.update();
        }
//        else
//        {
//            NRF_LOG_DEBUG("flash not updated");
//            NRF_LOG_FLUSH();
//        }

        trigger_update_periodically_timer = true;
    }
}

void EEPROMClass::reset_timer_update_periodically(void)
{
    ti_periodically_update = kaleidoscope::Runtime.millisAtCycleStart();
}


EEPROMClass EEPROM;
