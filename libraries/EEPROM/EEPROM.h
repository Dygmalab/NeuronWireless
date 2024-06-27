/*
    EEPROM.cpp - RP2040 EEPROM emulation
    Copyright (c) 2021 Earle F. Philhower III. All rights reserved.

    Based on ESP8266 EEPROM library, which is
    Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.

    Based on Nordic Semiconductor SDK nrf5_sdk_17.1.0_ddde560, which is
    Copyright (C) 2020  Dygma Lab S.L.
    All rights reserved.

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

    Author: Juan Hauara @JuanHauara
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>


class EEPROMClass
{
    public:
        void begin(size_t size);
        bool end(void);

        /*
            Use of the class for storing data in flash memory:
            - The put() and write() methods stores data in the internal RAM buffer of the EEPROMClass.
            - The commit() method sets an internal flag indicating that the RAM buffer has changes
              that need to be saved to the flash memory.
            - The update() method writes the entire RAM buffer to the flash memory.
        */
        void write(int const address, uint8_t const val);
        bool commit(void);
        bool getNeedUpdate(void);
        void update(void);
        void timer_update_periodically_run(uint32_t timeout_ms);
        void reset_timer_update_periodically(void);
        uint8_t read(int const address);

        void erase(void);
        uint8_t *getDataPtr(void);
        uint8_t const *getConstDataPtr(void) const;

        template<typename T>
        T &get(int const address, T &t)
        {
            if (address < 0 || address + sizeof(T) > _size)
            {
                return t;
            }

            memcpy((uint8_t *)&t, _data + address, sizeof(T));

            return t;
        }

        template<typename T>
        const T &put(int const address, const T &t)
        {
            if (address < 0 || address + sizeof(T) > _size)
            {
                return t;
            }

            if (memcmp(_data + address, (const uint8_t *)&t, sizeof(T)) != 0)
            {
                _dirty = true;
                memcpy(_data + address, (const uint8_t *)&t, sizeof(T));
            }

            return t;
        }

        template<typename T>
        const T &update(int const address, const T &t)
        {
            return put(address, t);
        }

        size_t length()
        {
            return _size;
        }

        uint8_t &operator[](int const address)
        {
            return getDataPtr()[address];
        }

        uint8_t const &operator[](int const address) const
        {
            return getConstDataPtr()[address];
        }

    protected:
        uint8_t *_data = nullptr;
        size_t _size = 0;
        bool needUpdate = false;
        bool _dirty = false;

        bool trigger_update_periodically_timer = true;
        uint32_t ti_periodically_update = 0;
};

extern EEPROMClass EEPROM;
