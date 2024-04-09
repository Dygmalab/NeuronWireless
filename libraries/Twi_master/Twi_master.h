/*
    This library uses the TWIM driver wich is a Two Wire Interface Master with 
    EasyDMA (TWIM) peripheral driver.

    Official documentation page:
    https://infocenter.nordicsemi.com/topic/sdk_nrf5_v17.1.0/group__nrfx__twim.html
*/


#ifndef __TWI_MASTER__
#define __TWI_MASTER__


#ifdef __cplusplus
extern "C" 
{
#endif

#include <nrfx_twim.h>

#ifdef __cplusplus
}
#endif


#define TWI_MASTER_DEBUG                 0
#define TWI_MASTER_PRINT_RECEIVED_BUFFER 0


#define TWI_PORT    0


class Twi_master
{
    public:
        Twi_master(uint32_t _scl_pin, uint32_t _sda_pin) : scl_pin(_scl_pin), sda_pin(_sda_pin) {};

        bool init(nrf_twim_frequency_t speed, bool hold_pullup_uninit);
        
        bool write_bytes(uint8_t slave_addr, uint8_t *tx_buffer, uint32_t nBytes);
        bool request_bytes(uint8_t slave_addr, uint8_t *rx_buffer, uint32_t nBytes);
        bool transfer_completed(void);

        void reset_module(void);

    private:
        uint32_t scl_pin;
        uint32_t sda_pin;

        const nrfx_twim_t twi_master_driver = NRFX_TWIM_INSTANCE(TWI_PORT);
        nrfx_twim_config_t twi_master_config;
};

#endif  // __TWI_MASTER__
