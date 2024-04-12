
#include "Arduino.h"
#include "nrf_drv_clock.h"
#include "rf_host_device_api.h"

void rfhdev_api_init(void)
{
    result_t result = RESULT_ERR;
    rfhdev_config_t rfhdev_config;

    rfhdev_config.clock_hfclk_start_cb = nrfx_clock_hfclk_start;
    rfhdev_config.clock_hfclk_stop_cb = nrfx_clock_hfclk_stop;
    rfhdev_config.ppi_channel_alloc_cb = nrfx_ppi_channel_alloc;
    rfhdev_config.sleep_postpone_cb = NULL;
    rfhdev_config.millis_request_cb = millis;

    result = rfhdev_init( &rfhdev_config );

    UNUSED( result );
}
