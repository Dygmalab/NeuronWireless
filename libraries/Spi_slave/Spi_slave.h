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
*/

#ifndef __SPI_SLAVE_H__
#define __SPI_SLAVE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "nrf_drv_spis.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif

#include <queue>
#include <Communications_protocol.h>
#include "Fifo_buffer.h"

#define SPI_SLAVE_DEBUG                 0
#define SPI_DEBUG_PRINT_RX_PACKET       0
#define NUM_BYTES_OF_RX_PACKET_TO_PRINT 8

#define COMPILE_SPI0_SUPPORT            0
#define COMPILE_SPI1_SUPPORT            1
#define COMPILE_SPI2_SUPPORT            1


#define RX_BUFF_LEN                     sizeof(Communications_protocol::Packet)
#define TX_BUFF_LEN                     sizeof(Communications_protocol::Packet)

class Spi_slave {
   public:
    Spi_slave(uint8_t _spi_port,
              uint32_t _miso_pin,
              uint32_t _mosi_pin,
              uint32_t _sck_pin,
              uint32_t _cs_pin,
              nrf_spis_mode_t _spi_mode = NRF_SPIS_MODE_0,
              nrf_gpio_pin_drive_t _pin_miso_strength = NRF_GPIO_PIN_S0S1,
              nrf_gpio_pin_pull_t _pin_csn_pullup = NRF_GPIO_PIN_NOPULL);

    void init(void);
    void deinit(void);

    Fifo_buffer *rx_fifo;
    Fifo_buffer *tx_fifo;

   private:
    uint8_t spi_port;

    uint32_t miso_pin;
    uint32_t mosi_pin;
    uint32_t sck_pin;
    uint32_t cs_pin;

    nrf_spis_mode_t spi_mode;                // NRF_SPIS_MODE_0, NRF_SPIS_MODE_1, ..
    nrf_gpio_pin_drive_t pin_miso_strength;  // NRF_GPIO_PIN_H0S1, NRF_GPIO_PIN_S0H1, ..
    nrf_gpio_pin_pull_t pin_csn_pullup;      //�NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_PULLUP.

    const nrf_drv_spis_t *spi_slave_inst;
};


#endif  // __SPI_SLAVE_H__
