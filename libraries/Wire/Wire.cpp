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

#include "Wire.h"

#include "Arduino.h"
#include "Twi_master.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif


bool Wire::twi_already_init = false;

static Twi_master twi_master(TWI_SCL_PIN, TWI_SDA_PIN);

Wire Wire1;

/*
    Methods to be compatible with the Arduino Wire library as a master.

    Process for an I2C master to send data to the slave:
        1° Set the address of the slave to which to send the data with the beginTransmission(slaveAddr) method.
           slaveAddr is the 7bit address of the slave.
        2° Write the data in the internal buffer of the Wire library with the write() method.
           The internal buffer is 32Bytes.
        3° Send the data with the endTransmission() method. Returns 0 if successful.

    Process for an I2C master to receive data from the slave:
        1° The master has to request bytes from a peripheral using the requestFrom() method.
        2° The bytes may then be received Byte by Byte using the available() and read() methods or you can also
           use the readBytes() method that is inherited from the Stream class to read multiple Bytes at once.
*/
void Wire::begin(uint16_t clock_khz)
{
    if (!Wire::twi_already_init)
    {
        nrf_twim_frequency_t freq;

        switch (clock_khz)
        {
            case 100:
                freq = NRF_TWIM_FREQ_100K;
                break;
            case 250:
                freq = NRF_TWIM_FREQ_250K;
                break;
            case 400:
                freq = NRF_TWIM_FREQ_400K;
                break;
            default:
                NRF_LOG_ERROR("TWI clock can only be 100KHz, 250KHz or 400KHz.")
                NRF_LOG_FLUSH();
                return;
        }

        Wire::twi_already_init = twi_master.init(freq, true);

        if (Wire::twi_already_init)
        {
            twi_master.reset_module();
        }
    }
}

// Write I2C
void Wire::beginTransmission(uint8_t slave_addr)
{
    set_addr(slave_addr);
}

void Wire::write(uint8_t n)
{
    uint8_t buff[1] = {n};
    write_success = write(buff, 1);
}

uint8_t Wire::endTransmission(bool stopBit)
{
    if (write_success) return 0; // 0 = success

    return 5; // 5 = timeout error
}

// Read I2C
void Wire::requestFrom(uint8_t address, uint32_t lenMessage, bool stopBit)
{
    set_addr(address);
}

uint8_t Wire::readBytes(uint8_t *message, uint32_t lenMessage)
{
    if (!read(message, lenMessage)) return 0;

    return lenMessage;
}

void Wire::setTimeout(uint16_t timeout)
{
    _timeout = timeout;
}


void Wire::set_addr(uint8_t slave_addr)
{
    _slave_addr = slave_addr;
}

bool Wire::read(uint8_t *data, uint16_t data_len)
{
    /*
        Blocking function.

        Parameters
        ----------
        *data: Pointer to a buffer of Bytes where to save the received data.
        data_len: Number of Bytes to request.

        Returns true if succes or false if not.
    */

    memset(data, 0, data_len); // Clear buffer.

#if TWI_DEBUG
    NRF_LOG_INFO("Requesting %d Bytes..", data_len);
    NRF_LOG_FLUSH();
#endif

    if (!twi_master.request_bytes(_slave_addr, data, data_len))
    {
        twi_master.reset_module();

#if TWI_DEBUG
        NRF_LOG_DEBUG("<<<<< ERROR requesting Bytes, TWI reseted >>>>>");
        NRF_LOG_FLUSH();
#endif

        return false;
    }

    // Wait for the transmission to get completed.
    uint64_t ti = millis();
    while (!twi_master.transfer_completed())
    {
        if ((millis() - ti) > _timeout)
        {
            twi_master.reset_module();

#if TWI_DEBUG
            NRF_LOG_ERROR("<<<<< ERROR requesting Bytes (Time out), TWI reseted >>>>>");
            NRF_LOG_FLUSH();
#endif

            return false;
        }
    }

#if TWI_PRINT_RECEIVED_BUFF
    NRF_LOG_INFO("Slave addr: 0x%02x", _slave_addr);
    for (uint16_t i = 0; i < data_len; i++)
    {
        NRF_LOG_INFO("data[%d] = %d", i, data[i]);
    }
    NRF_LOG_FLUSH();
#endif

    return true;
}

bool Wire::write(uint8_t *data, uint16_t data_len)
{
    /*
        Blocking function.

        Parameters
        ----------
        *data: Pointer to an array with the Bytes to send.
        data_len: Number of Bytes to send.

        Returns true if succes or false if not.
    */

#if TWI_DEBUG
    NRF_LOG_INFO("Writing %d Bytes..", data_len);
    NRF_LOG_FLUSH();
#endif

    if (!twi_master.write_bytes(_slave_addr, data, data_len))
    {
        twi_master.reset_module();

#if TWI_DEBUG
        NRF_LOG_INFO("<<<<< ERROR writing Bytes, TWI reseted >>>>>");
        NRF_LOG_FLUSH();
#endif

        return false;
    }

    // Wait for the transmission to get completed.
    uint64_t ti = millis();
    while (!twi_master.transfer_completed())
    {
        if ((millis() - ti) > _timeout)
        {
            twi_master.reset_module();

#if TWI_DEBUG
            NRF_LOG_INFO("<<<<< ERROR writing Bytes (Time out), TWI reseted >>>>>");
            NRF_LOG_FLUSH();
#endif

            return false;
        }
    }

    return true;
}
