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
    This class implements EasyDMA for accessing RAM without CPU involvement.

    Some chips like the NRF52833 require the chip select signal to goes high
    at the end of the transmission for the SPI to work correctly.
    On this particular chip the SPI will not work if the chip select signal
    is left low permanently.

    The default options:
    nrf_spis_mode_t spi_mode = NRF_SPIS_MODE_0,
    nrf_gpio_pin_drive_t pin_miso_strength = NRF_GPIO_PIN_S0S1,
    nrf_gpio_pin_pull_t pin_csn_pullup = NRF_GPIO_PIN_NOPULL);

    Official documentation:
    https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/hardware_driver_spi_slave.html
    https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/group__nrfx__spis.html

    Default configuration of the SPI slave instance:
    {
        .miso_pin = NRFX_SPIS_PIN_NOT_USED,
        .mosi_pin = NRFX_SPIS_PIN_NOT_USED,
        .sck_pin = NRFX_SPIS_PIN_NOT_USED,
        .csn_pin = NRFX_SPIS_PIN_NOT_USED,
        .mode = NRF_SPIS_MODE_0,
        .bit_order = NRF_SPIS_BIT_ORDER_MSB_FIRST,
        .csn_pullup = NRFX_SPIS_DEFAULT_CSN_PULLUP,   // NRFX_SPIS_DEFAULT_CSN_PULLUP is -> NRF_GPIO_PIN_NOPULL
        .miso_drive = NRFX_SPIS_DEFAULT_MISO_DRIVE,   // NRFX_SPIS_DEFAULT_MISO_DRIVE is -> NRF_GPIO_PIN_S0S1
        .def = NRFX_SPIS_DEFAULT_DEF,
        .orc = NRFX_SPIS_DEFAULT_ORC,
        .irq_priority = NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY,
    }

    NRF_SPIS_MODE_0: SCK active high, sample on leading edge of clock.  -> CPOL = 0 / CPHA = 0
    NRF_SPIS_MODE_1: SCK active high, sample on trailing edge of clock. -> CPOL = 0 / CPHA = 1
    NRF_SPIS_MODE_2: SCK active low, sample on leading edge of clock.   -> CPOL = 1 / CPHA = 0
    NRF_SPIS_MODE_3: SCK active low, sample on trailing edge of clock.  -> CPOL = 1 / CPHA = 1

    Author: Juan Hauara @JuanHauara
*/

#include "Spi_slave.h"
#include "Ble_composite_dev.h"
#include "CRC_wrapper.h"

// SPIS user event handler.
#if COMPILE_SPI0_SUPPORT
static const nrf_drv_spis_t spi_slave_inst0 = NRF_DRV_SPIS_INSTANCE(0);

static volatile uint8_t spi0_rx_buff[RX_BUFF_LEN];
static Communications_protocol::Packet spi0_packet;
static Fifo_buffer spi0tx_fifo(sizeof(spi1_packet));
static Fifo_buffer spi0rx_fifo(sizeof(spi1_packet));


void spi0_slave_event_handler(nrf_drv_spis_event_t event) {
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE) {
        memcpy(spi0_packet.buf, (const void *)spi0_rx_buff, sizeof(Communications_protocol::Packet));
        spi0_rx_fifo.put(&spi0_packet);  // Put the new spi_packet in the Rx FIFO.

        memset((void *)spi0_rx_buff, 0, RX_BUFF_LEN);                         // Clear Rx buffer.
        memset(spi0_packet.buf, 0, sizeof(Communications_protocol::Packet));  // Clear Packet buffer.

        spi0_packet.header.device  = Communications_protocol::NEURON_DEFY_WIRELESS;
        spi0_packet.header.command = Communications_protocol::IS_ALIVE;

        if (!spi0_tx_fifo.is_empty())  // If there are packets to send in the Tx FIFO.
        {
            spi0_tx_fifo.get(&spi0_packet);  // Get a packet and send when the master polls.
        }

        if (!spi0_tx_fifo.is_empty()) {
            spi0_packet.header.has_more_packets = true;
        } else {
            spi0_packet.header.has_more_packets = false;
        }


        APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spi_slave_inst0, (const uint8_t *)&spi0_packet.buf, TX_BUFF_LEN, (uint8_t *)spi0_rx_buff, RX_BUFF_LEN));
    }
}
#endif

#if COMPILE_SPI1_SUPPORT
static const nrf_drv_spis_t spi_slave_inst1 = NRF_DRV_SPIS_INSTANCE(1);

static volatile uint8_t spi1_rx_buff[RX_BUFF_LEN];
static Communications_protocol::Packet spi1_packet;
static Fifo_buffer spi1_rx_fifo(sizeof(spi1_packet));
static Fifo_buffer spi1_tx_fifo(sizeof(spi1_packet));

void spi1_slave_event_handler(nrf_drv_spis_event_t event) {
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE) {
        memcpy(spi1_packet.buf, (const void *)spi1_rx_buff, sizeof(Communications_protocol::Packet));
        uint8_t rx_crc         = spi1_packet.header.crc;
        spi1_packet.header.crc = 0;
        if (crc8(spi1_packet.buf, sizeof(Communications_protocol::Header) + spi1_packet.header.size) == rx_crc) {
            spi1_rx_fifo.put(&spi1_packet);  // Put the new spi_packet in the Rx FIFO.
        }

        memset((void *)spi1_rx_buff, 0, RX_BUFF_LEN);                         // Clear Rx buffer.
        memset(spi1_packet.buf, 0, sizeof(Communications_protocol::Packet));  // Clear Packet buffer.

        spi1_packet.header.device  = Communications_protocol::NEURON_DEFY;
        if (ble_innited()) {
            spi1_packet.header.device = Communications_protocol::BLE_NEURON_2_DEFY;
        }
        spi1_packet.header.command = Communications_protocol::IS_ALIVE;

        if (!spi1_tx_fifo.is_empty())  // If there are packets to send in the Tx FIFO.
        {
            spi1_tx_fifo.get(&spi1_packet);  // Get a packet and send when the master polls.
        }

        if (!spi1_tx_fifo.is_empty()) {
            spi1_packet.header.has_more_packets = true;
        } else {
            spi1_packet.header.has_more_packets = false;
        }
        spi1_packet.header.crc = 0;
        spi1_packet.header.crc = crc8(spi1_packet.buf, sizeof(Communications_protocol::Header) + spi1_packet.header.size);

        APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spi_slave_inst1, (const uint8_t *)&spi1_packet.buf, TX_BUFF_LEN, (uint8_t *)spi1_rx_buff, RX_BUFF_LEN));
    }
}
#endif

#if COMPILE_SPI2_SUPPORT
static const nrf_drv_spis_t spi_slave_inst2 = NRF_DRV_SPIS_INSTANCE(2);
static volatile uint8_t spi2_rx_buff[RX_BUFF_LEN];
static Communications_protocol::Packet spi2_packet;
static Fifo_buffer spi2_tx_fifo(sizeof(spi1_packet));
static Fifo_buffer spi2_rx_fifo(sizeof(spi1_packet));

void spi2_slave_event_handler(nrf_drv_spis_event_t event) {
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE) {
        memcpy(spi2_packet.buf, (const void *)spi2_rx_buff, sizeof(Communications_protocol::Packet));
        uint8_t rx_crc         = spi2_packet.header.crc;
        spi2_packet.header.crc = 0;
        if (crc8(spi2_packet.buf, sizeof(Communications_protocol::Header) + spi2_packet.header.size) == rx_crc) {
            spi2_rx_fifo.put(&spi2_packet);  // Put the new spi_packet in the Rx FIFO.
        }

        memset((void *)spi2_rx_buff, 0, RX_BUFF_LEN);                         // Clear Rx buffer.
        memset(spi2_packet.buf, 0, sizeof(Communications_protocol::Packet));  // Clear Packet buffer.

        spi2_packet.header.device = Communications_protocol::NEURON_DEFY;
        if (ble_innited()) {
            spi2_packet.header.device = Communications_protocol::BLE_NEURON_2_DEFY;
        }
        spi2_packet.header.command = Communications_protocol::IS_ALIVE;

        if (!spi2_tx_fifo.is_empty())  // If there are packets to send in the Tx FIFO.
        {
            spi2_tx_fifo.get(&spi2_packet);  // Get a packet and send when the master polls.
        }

        if (!spi2_tx_fifo.is_empty()) {
            spi2_packet.header.has_more_packets = true;
        } else {
            spi2_packet.header.has_more_packets = false;
        }
        spi2_packet.header.crc = 0;
        spi2_packet.header.crc = crc8(spi2_packet.buf, sizeof(Communications_protocol::Header) + spi2_packet.header.size);

        APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spi_slave_inst2, (const uint8_t *)&spi2_packet.buf, TX_BUFF_LEN, (uint8_t *)spi2_rx_buff, RX_BUFF_LEN));
    }
}
#endif


Spi_slave::Spi_slave(uint8_t _spi_port, uint32_t _miso_pin, uint32_t _mosi_pin, uint32_t _sck_pin, uint32_t _cs_pin, nrf_spis_mode_t _spi_mode, nrf_gpio_pin_drive_t _pin_miso_strength, nrf_gpio_pin_pull_t _pin_csn_pullup)
  : spi_port(_spi_port), miso_pin(_miso_pin), mosi_pin(_mosi_pin), sck_pin(_sck_pin), cs_pin(_cs_pin), spi_mode(_spi_mode), pin_miso_strength(_pin_miso_strength), pin_csn_pullup(_pin_csn_pullup) {
#if COMPILE_SPI0_SUPPORT
    if (spi_port == 0) {
        spi_slave_inst = &spi_slave_inst0;
        rx_fifo        = &spi0_rx_fifo;
        tx_fifo        = &spi0_tx_fifo;
    }
#endif

#if COMPILE_SPI1_SUPPORT
    if (spi_port == 1) {
        spi_slave_inst = &spi_slave_inst1;
        rx_fifo        = &spi1_rx_fifo;
        tx_fifo        = &spi1_tx_fifo;
    }
#endif

#if COMPILE_SPI2_SUPPORT
    if (spi_port == 2) {
        spi_slave_inst = &spi_slave_inst2;
        rx_fifo        = &spi2_rx_fifo;
        tx_fifo        = &spi2_tx_fifo;
    }
#endif

#if SPI_SLAVE_DEBUG
    if (spi_port > 2) {
        NRF_LOG_DEBUG("ERROR in Spi_slave class, you must set the COMPILE_SPIx_SUPPORT flag.");
        NRF_LOG_FLUSH();
    }
#endif
};

void Spi_slave::init(void) {
    /*
        Enable the constant latency sub power mode to minimize the time it takes
        for the SPIS peripheral to become active after the CSN line is asserted
        (when the CPU is in sleep mode).
    */
    //NRF_POWER->TASKS_CONSTLAT = 1;  // Error de softdevice al activar.

    nrf_drv_spis_config_t spi_slave_config = NRF_DRV_SPIS_DEFAULT_CONFIG;
    /*
        Default configuration of the SPI slave instance:
        {
            .miso_pin = NRFX_SPIS_PIN_NOT_USED,
            .mosi_pin = NRFX_SPIS_PIN_NOT_USED,
            .sck_pin = NRFX_SPIS_PIN_NOT_USED,
            .csn_pin = NRFX_SPIS_PIN_NOT_USED,
            .mode = NRF_SPIS_MODE_0,
            .bit_order = NRF_SPIS_BIT_ORDER_MSB_FIRST,
            .csn_pullup = NRFX_SPIS_DEFAULT_CSN_PULLUP,   // NRFX_SPIS_DEFAULT_CSN_PULLUP is -> NRF_GPIO_PIN_NOPULL
            .miso_drive = NRFX_SPIS_DEFAULT_MISO_DRIVE,   // NRFX_SPIS_DEFAULT_MISO_DRIVE is -> NRF_GPIO_PIN_S0S1
            .def = NRFX_SPIS_DEFAULT_DEF,
            .orc = NRFX_SPIS_DEFAULT_ORC,
            .irq_priority = NRFX_SPIS_DEFAULT_CONFIG_IRQ_PRIORITY,
        }


        NRF_SPIS_MODE_0: SCK active high, sample on leading edge of clock.  -> CPOL = 0 / CPHA = 0
        NRF_SPIS_MODE_1: SCK active high, sample on trailing edge of clock. -> CPOL = 0 / CPHA = 1
        NRF_SPIS_MODE_2: SCK active low, sample on leading edge of clock.   -> CPOL = 1 / CPHA = 0
        NRF_SPIS_MODE_3: SCK active low, sample on trailing edge of clock.  -> CPOL = 1 / CPHA = 1
    */
    spi_slave_config.miso_pin   = miso_pin;
    spi_slave_config.mosi_pin   = mosi_pin;
    spi_slave_config.sck_pin    = sck_pin;
    spi_slave_config.csn_pin    = cs_pin;
    spi_slave_config.mode       = spi_mode;
    spi_slave_config.miso_drive = pin_miso_strength;
    spi_slave_config.csn_pullup = pin_csn_pullup;

    Communications_protocol::Packet packet{};

#if COMPILE_SPI0_SUPPORT
    if (spi_port == 0) {
        APP_ERROR_CHECK(nrf_drv_spis_init(spi_slave_inst, &spi_slave_config, spi0_slave_event_handler));

        // Start listening to the SPI master.
        memset((void *)spi0_rx_buff, 0, RX_BUFF_LEN);  // Clear Rx buffer.

        /*
            Function for preparing the SPI slave instance for a single SPI transaction.
            To receive data, the SPI buffers must be set by calling nrf_drv_spis_buffers_set.

            New buffers must be set by calling nrf_drv_spis_buffers_set after every finished
            transaction. Otherwise, the transaction is ignored, and the default character is
            clocked out.

            Note:
            This function can be called from the callback function context.

            Client applications must call this function after every NRFX_SPIS_XFER_DONE event
            if the SPI slave driver must be prepared for a possible new SPI transaction.

            Peripherals using EasyDMA (including SPIS) require the transfer buffers
            to be placed in the Data RAM region. If this condition is not met, this
            function will fail with the error code NRFX_ERROR_INVALID_ADDR.
        */
        APP_ERROR_CHECK(nrf_drv_spis_buffers_set(spi_slave_inst, (const uint8_t *)&packet.buf, sizeof(&packet), (uint8_t *)spi0_rx_buff, RX_BUFF_LEN));
    }
#endif

#if COMPILE_SPI1_SUPPORT
    if (spi_port == 1) {
        APP_ERROR_CHECK(nrf_drv_spis_init(spi_slave_inst, &spi_slave_config, spi1_slave_event_handler));

        // Start listening to the SPI master.
        memset((void *)spi1_rx_buff, 0, RX_BUFF_LEN);  // Clear Rx buffer.

        /*
            Function for preparing the SPI slave instance for a single SPI transaction.
            To receive data, the SPI buffers must be set by calling nrf_drv_spis_buffers_set.

            New buffers must be set by calling nrf_drv_spis_buffers_set after every finished
            transaction. Otherwise, the transaction is ignored, and the default character is
            clocked out.

            Note:
            This function can be called from the callback function context.

            Client applications must call this function after every NRFX_SPIS_XFER_DONE event
            if the SPI slave driver must be prepared for a possible new SPI transaction.

            Peripherals using EasyDMA (including SPIS) require the transfer buffers
            to be placed in the Data RAM region. If this condition is not met, this
            function will fail with the error code NRFX_ERROR_INVALID_ADDR.
        */
        APP_ERROR_CHECK(nrf_drv_spis_buffers_set(spi_slave_inst, (const uint8_t *)&packet.buf, TX_BUFF_LEN, (uint8_t *)spi1_rx_buff, RX_BUFF_LEN));
    }
#endif

#if COMPILE_SPI2_SUPPORT
    if (spi_port == 2) {
        APP_ERROR_CHECK(nrf_drv_spis_init(spi_slave_inst, &spi_slave_config, spi2_slave_event_handler));

        // Start listening to the SPI master.
        memset((void *)spi2_rx_buff, 0, RX_BUFF_LEN);  // Clear Rx buffer.

        /*
            Function for preparing the SPI slave instance for a single SPI transaction.
            To receive data, the SPI buffers must be set by calling nrf_drv_spis_buffers_set.

            New buffers must be set by calling nrf_drv_spis_buffers_set after every finished
            transaction. Otherwise, the transaction is ignored, and the default character is
            clocked out.

            Note:
            This function can be called from the callback function context.

            Client applications must call this function after every NRFX_SPIS_XFER_DONE event
            if the SPI slave driver must be prepared for a possible new SPI transaction.

            Peripherals using EasyDMA (including SPIS) require the transfer buffers
            to be placed in the Data RAM region. If this condition is not met, this
            function will fail with the error code NRFX_ERROR_INVALID_ADDR.
        */
        APP_ERROR_CHECK(nrf_drv_spis_buffers_set(spi_slave_inst, (const uint8_t *)&packet.buf, TX_BUFF_LEN, (uint8_t *)spi2_rx_buff, RX_BUFF_LEN));
    }
#endif
}

void Spi_slave::deinit(void) {
    /*
        Function for uninitializing the SPI slave driver instance.
        When the SPI slave driver instance is no longer needed or its
        configuration must be changed, call nrf_drv_spis_uninit.

    When the peripheral is disabled, the pins will behave as
                                         regular GPIOs and use the configuration in their respective
                                             OUT bit field and PIN_CNF[n] register.
                                                 */

    nrf_drv_spis_uninit(spi_slave_inst);
}
