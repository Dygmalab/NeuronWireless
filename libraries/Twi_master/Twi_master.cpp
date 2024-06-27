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
    This library uses the TWIM driver wich is a Two Wire Interface Master with
    EasyDMA (TWIM) peripheral driver.

    Official documentation page:
    https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/group__nrfx__twim.html

    Author: Juan Hauara @JuanHauara
*/

#include "Twi_master.h"

#ifdef __cplusplus
extern "C" 
{
#endif

#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif


/////////////////
static bool flag_transfer_completed = false;
static bool flag_rx_completed = false;
static bool flag_tx_completed = false;

void twi_master_handler(nrfx_twim_evt_t const *p_event, void *p_context)
{
    switch (p_event->type)  // Check what type of TWI event ocurred.
    {
        case NRFX_TWIM_EVT_DONE:  // TWI Transfer completed event.
            if (p_event->xfer_desc.type == NRFX_TWIM_XFER_RX) // If it was an Rx transfer.
            {
                flag_rx_completed = true;
            }

            if (p_event->xfer_desc.type == NRFX_TWIM_XFER_TX) // If it was an Tx transfer.
            {
                flag_tx_completed = true;
            }

            flag_transfer_completed = true;
            break;

        default:
            break;
    }
}
/////////////////


bool Twi_master::init(nrf_twim_frequency_t speed, bool hold_pullup_uninit)
{
    twi_master_config.scl = scl_pin;
    twi_master_config.sda = sda_pin;
    twi_master_config.frequency = speed;  // NRF_TWIM_FREQ_100K, NRF_TWIM_FREQ_250K, NRF_TWIM_FREQ_400K
    twi_master_config.interrupt_priority = APP_IRQ_PRIORITY_MID;  // It cannot be high if we use a softdevice.
    twi_master_config.hold_bus_uninit = hold_pullup_uninit;  // Hold pull up state on GPIO pins after uninit.

    nrfx_err_t ret = nrfx_twim_init(&twi_master_driver, &twi_master_config, NULL, NULL);
    APP_ERROR_CHECK(ret);

    if (ret != NRF_SUCCESS)
    {
        #if (TWI_MASTER_DEBUG > 0)
            NRF_LOG_ERROR("Error 6411: cod %d", ret);
            NRF_LOG_FLUSH();
        #endif

        return false;
    }

    // Pin configuration: No pullup, high output current.
    nrf_gpio_cfg(scl_pin,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0D1,
        NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(sda_pin,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0D1,
        NRF_GPIO_PIN_NOSENSE);
    
    nrfx_twim_enable(&twi_master_driver);
    
    return true;
}

bool Twi_master::transfer_completed(void)
{
    return flag_transfer_completed;
}

bool Twi_master::write_bytes(uint8_t slave_addr, uint8_t *tx_buffer, uint32_t nBytes)
{
    /*
        Blocking function.

        See 6.27.4 Master write sequence, pag. 433 of the datahseet.

        Parameters
        ----------
        slave_addr: 7-bit address of the TWI slave device.
        rx_buffer: Pointer to a Bytes buffer with the data to send.
        nBytes: Number of Bytes to send.
    */
    
    flag_transfer_completed = false;
    flag_tx_completed = false;
    
    // See documentation:
    // https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/group__nrf__drv__twi.html
    nrfx_err_t ret = nrfx_twim_tx(&twi_master_driver, slave_addr, tx_buffer, nBytes, false);

    if (ret != NRF_SUCCESS)
    {
        #if (TWI_MASTER_DEBUG > 0)
        NRF_LOG_ERROR("Error 8741: cod %d", ret);
        NRF_LOG_FLUSH();
        #endif

        return false;
    }

    #if (TWI_MASTER_DEBUG > 1)
    NRF_LOG_INFO("%d Bytes sent to slave 0x%02x", nBytes, slave_addr);
    NRF_LOG_FLUSH();
    #endif

    return true;
}

bool Twi_master::request_bytes(uint8_t slave_addr, uint8_t *rx_buffer, uint32_t nBytes)
{
    /*
        Blocking function.

        See 6.27.5 Master read sequence, pag. 434 of the datahseet.

        Parameters
        ----------
        slave_addr: 7-bit slave address of the device to request bytes from.
        rx_buffer: Pointer to a Bytes buffer where to save the received data.
        nBytes: Number of Bytes to request.

        Notes
        -----
        In the event that the slave has more than one register to read data from, for 
        example a humidity and temperature sensor will have a register to read the 
        humidity and another to read the temperature. In that case we must first tell 
        it what is the record we want to read and the amount of Bytes through a call 
        to the nrfx_twim_tx() function and then we will be able to receive the Bytes
        with nrfx_twim_rx().
    */

    nrfx_err_t ret;

    // Request and Receive the data from the TWI slave.
    flag_transfer_completed = false;
    flag_rx_completed = false;

    // See documentation:
    // https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/group__nrf__drv__twi.html
    ret = nrfx_twim_rx(&twi_master_driver, slave_addr, rx_buffer, nBytes);

    if (ret != NRF_SUCCESS)
    {
        #if (TWI_MASTER_DEBUG > 0)
        NRF_LOG_ERROR("Error 1751: cod %d", ret);
        NRF_LOG_FLUSH();
        #endif

        return false;
    }
  
    #if TWI_MASTER_PRINT_RECEIVED_BUFFER
    for (uint32_t i = 0; i < nBytes; i++)
    {
        NRF_LOG_INFO("rx_buffer[%d] = %d", i, rx_buffer[i]);
    }
    NRF_LOG_FLUSH();
    #endif
    
    return true;
}

void Twi_master::reset_module(void)
{
    nrfx_twim_disable(&twi_master_driver);
    nrfx_twim_uninit(&twi_master_driver);
    nrf_delay_us(100);

    nrfx_twim_init(&twi_master_driver, &twi_master_config, twi_master_handler, NULL);

    // Pin configuration: No pullup, high output current.
    nrf_gpio_cfg(scl_pin,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0D1,
        NRF_GPIO_PIN_NOSENSE);

    nrf_gpio_cfg(sda_pin,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        NRF_GPIO_PIN_NOPULL,
        NRF_GPIO_PIN_H0D1,
        NRF_GPIO_PIN_NOSENSE);

    nrfx_twim_enable(&twi_master_driver);
    nrf_delay_us(100);
}