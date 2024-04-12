
#ifndef __RF_HOST_DEVICE_API_H_
#define __RF_HOST_DEVICE_API_H_

#ifdef __cplusplus
extern "C" {
#endif

    #include "rf_gateway.h"

    #define RFGW_PIPE_ID_KEYSCANNER_LEFT RFGW_PIPE_ID_1
    #define RFGW_PIPE_ID_KEYSCANNER_RIGHT RFGW_PIPE_ID_2

    extern void rfhdev_api_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __RF_HOST_DEVICE_API_H_ */
