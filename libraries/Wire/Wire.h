/*
 * Arduino interface wrapper of the Wire library to use TWI_MASTER functions for the NRF52 chips.
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

#ifndef __WIRE_NRF52_H__
#define __WIRE_NRF52_H__


#include <stdlib.h>
#include <stdint.h>


#define TWI_DEBUG               1
#define TWI_PRINT_RECEIVED_BUFF 0


#define TWI_SCL_PIN             5
#define TWI_SDA_PIN             4


class Wire
{
  public:
    static void begin(uint16_t clock_khz);

    // Methods to be compatible with the Arduino Wire library.
    void beginTransmission(uint8_t slave_addr);
    void write(uint8_t n);
    uint8_t endTransmission(bool stopBit);
    void requestFrom(uint8_t address, uint32_t lenMessage, bool stopBit);
    uint8_t readBytes(uint8_t *message, uint32_t lenMessage);
    void setTimeout(uint16_t timeout);

    void set_addr(uint8_t slave_addr);
    bool read(uint8_t *buffer, uint16_t len);
    bool write(uint8_t *buffer, uint16_t len);

  private:
    static bool twi_already_init;
    uint8_t _slave_addr;
    uint16_t _timeout=100;
    uint8_t num_bytes;
    bool write_success;
};

extern Wire Wire1;


#endif  // __WIRE_NRF52_H__
