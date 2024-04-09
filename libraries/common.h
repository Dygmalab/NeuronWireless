#ifndef __COMMON_H__
#define __COMMON_H__

#include "stdint.h"

// clang-format off

#define COMPILE_FOR_NEURON_2_HARDWARE_V1_0      0
#define COMPILE_FOR_NEURON_2_HARDWARE_V1_1      1
#define COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0   0
#define COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1   0
#define COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_2   0
#define ENABLE_UART_DEBUG                       0


// Disable DEBUG logs for production.
#if (COMPILE_FOR_NEURON_2_HARDWARE_V1_0 || COMPILE_FOR_NEURON_2_HARDWARE_V1_1) && ENABLE_UART_DEBUG
#error "You need to deactivate de ENABLE_UART_DEBUG flag for the production hardware."
#endif

#define ENABLE_UART_DEBUG_LOG   ( (not COMPILE_FOR_NEURON_2_HARDWARE_V1_0) & (not COMPILE_FOR_NEURON_2_HARDWARE_V1_1) &                                     \
                                  (COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0 | COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1 | COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_2) & \
                                  ENABLE_UART_DEBUG )

#if ( (COMPILE_FOR_NEURON_2_HARDWARE_V1_0 + COMPILE_FOR_NEURON_2_HARDWARE_V1_1 + \
       COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0 + COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1 + COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_2) \
       > 1)
    #pragma message "COMPILE_FOR_NEURON_2_HARDWARE_V1_0 is defined: " #COMPILE_FOR_NEURON_2_HARDWARE_V1_0
    #pragma message "COMPILE_FOR_NEURON_2_HARDWARE_V1_1 is defined: " #COMPILE_FOR_NEURON_2_HARDWARE_V1_1
    #pragma message "COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0 is defined: " #COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0
    #pragma message "COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1 is defined: " #COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1
    #pragma message "COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_2 is defined: " #COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_2

    #error "You should not select more than one flag at a time."
#endif

#define TWI_MASTER_SCL_PIN   5
#define TWI_MASTER_SDA_PIN   4
#define TWI_MASTER_FLASH_CLK NRF_TWIM_FREQ_100K  /* flashing doesn't work reliably at higher clock speeds */

#if COMPILE_FOR_NEURON_2_HARDWARE_V1_0
    #define SIDE_NRESET_1 NRF_GPIO_PIN_MAP(0, 17)  /* nRESET signal OUT to keyboard side 1; HIGH = running, LOW = reset */
    #define SIDE_NRESET_2 NRF_GPIO_PIN_MAP(0, 1)   /* nRESET signal OUT to keyboard side 2; HIGH = running, LOW = reset */

    #define LED_GREEN_PIN NRF_GPIO_PIN_MAP(0, 28)
    #define LED_RED_PIN   NRF_GPIO_PIN_MAP(0, 03)

    #define BTN_RESET NRF_GPIO_PIN_MAP(0, 18)  /* LOW activated. */
    #define BTN_BOOT  NRF_GPIO_PIN_MAP(0, 15)  /* LOW activated. */
#elif COMPILE_FOR_NEURON_2_HARDWARE_V1_1
    #define SIDE_NRESET_1 NRF_GPIO_PIN_MAP(0, 28)  /* nRESET signal OUT to keyboard side 1; HIGH = running, LOW = reset */
    #define SIDE_NRESET_2 NRF_GPIO_PIN_MAP(0, 29)  /* nRESET signal OUT to keyboard side 2; HIGH = running, LOW = reset */

    #define LED_GREEN_PIN NRF_GPIO_PIN_MAP(0, 10)
    #define LED_RED_PIN   NRF_GPIO_PIN_MAP(0, 9)

    #define BTN_RESET NRF_GPIO_PIN_MAP(0, 18)  /* LOW activated. */
    #define BTN_BOOT  NRF_GPIO_PIN_MAP(0, 15)  /* LOW activated. */
#elif COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0
    #define SIDE_NRESET_1 NRF_GPIO_PIN_MAP(0, 17)  /* nRESET signal OUT to keyboard side 1; HIGH = running, LOW = reset */
    #define SIDE_NRESET_2 NRF_GPIO_PIN_MAP(1, 1)   /* nRESET signal OUT to keyboard side 2; HIGH = running, LOW = reset */

    /* Debug board LEDs are HIGH activated. */
    #define LED_GREEN_PIN NRF_GPIO_PIN_MAP(0, 28)
    #define LED_RED_PIN   NRF_GPIO_PIN_MAP(0, 03)

    #define BTN_RESET      NRF_GPIO_PIN_MAP(0, 18)  /* LOW activated. */
    #define BTN_BOOT       NRF_GPIO_PIN_MAP(0, 15)  /* LOW activated. */
#elif COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1
    #define SIDE_NRESET_1 NRF_GPIO_PIN_MAP(0, 28)  /* nRESET signal OUT to keyboard side 1; HIGH = running, LOW = reset */
    #define SIDE_NRESET_2 NRF_GPIO_PIN_MAP(0, 29)  /* nRESET signal OUT to keyboard side 2; HIGH = running, LOW = reset */

    /* Debug board LEDs are HIGH activated. */
    #define LED_GREEN_PIN NRF_GPIO_PIN_MAP(0, 10)
    #define LED_RED_PIN   NRF_GPIO_PIN_MAP(0, 9)

    #define BTN_RESET      NRF_GPIO_PIN_MAP(0, 18)  /* LOW activated. */
    #define BTN_BOOT       NRF_GPIO_PIN_MAP(0, 15)  /* LOW activated. */
#elif COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_2
    #define SIDE_NRESET_1 NRF_GPIO_PIN_MAP(0, 28)  /* nRESET signal OUT to keyboard side 1; HIGH = running, LOW = reset */
    #define SIDE_NRESET_2 NRF_GPIO_PIN_MAP(0, 29)  /* nRESET signal OUT to keyboard side 2; HIGH = running, LOW = reset */

    #define LED_GREEN_PIN NRF_GPIO_PIN_MAP(0, 10)
    #define LED_RED_PIN   NRF_GPIO_PIN_MAP(0, 9)

    #define BTN_RESET NRF_GPIO_PIN_MAP(0, 18)  /* LOW activated. */
    #define BTN_BOOT  NRF_GPIO_PIN_MAP(0, 15)  /* LOW activated. */
#endif

/* Only for DEBUG */
#if (COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0 | COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1)
    /* The LEDs on the breadboard are HIGH activated. */
    #define LED1           NRF_GPIO_PIN_MAP(1, 02)
    #define LED2           NRF_GPIO_PIN_MAP(1, 04)
    #define LED3           NRF_GPIO_PIN_MAP(0, 21)
    #define LED4           NRF_GPIO_PIN_MAP(1, 06)
    #define LED5           NRF_GPIO_PIN_MAP(0, 22)
    #define LED6           NRF_GPIO_PIN_MAP(1, 07)
    #define LED7           NRF_GPIO_PIN_MAP(0, 24)
    #define LED8           NRF_GPIO_PIN_MAP(0, 16)

    /* The buttons on the breadboard are LOW activated. */
    #define BTN1           NRF_GPIO_PIN_MAP(0, 12)
    #define BTN2           NRF_GPIO_PIN_MAP(0, 13)
    #define BTN3           NRF_GPIO_PIN_MAP(0, 14)

    #define LED_USB_STATUS LED1  /* Blink quickly when device is not configured or slowly when configured and working properly. */
    #define LED_USB_POWER  LED2  /* Turns on when connection is detected on USB port. It is turned off when connection is removed. */
    #define LED_RUNNING    LED3  /* Turns on when program is not sleeping. */
    #define LED_ACTIVE     LED4  /* Turns on when program is not in system OFF. */
    #define LED_CAPSLOCK   LED6  /* CapsLock LED state. */
    #define LED_NUMLOCK    LED7  /* Numlock LED state. */

    /* Buttons and LEDs are LOW activated in the debelopment board. */
    #define BTN_SYSTEM_OFF BTN1  /* Set the request for system OFF. */
#endif

#endif  /* __COMMON_H__ */
