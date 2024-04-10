/*
About softdevice:
    -BLE module needs s140_nrf52_7.2.0_softdevice.hex to work.
     The generated .hex does not contain the softdevice, you must flash it together with this firmware.
*/

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include "nordic_common.h"
#include "nrf.h"
#include "nrf_assert.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_power.h"
#include "nrf_sdm.h"

#include "app_error.h"
#include "app_timer.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"

// UART debug log
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#ifdef __cplusplus
}
#endif
#include "Arduino.h"
#include "EEPROM.h"
#include "Watchdog_timer.h"
#include "common.h"
// BLE
#include "Ble_composite_dev.h"

// Kaleidoscope
#include "Kaleidoscope-Colormap.h"
#include "Kaleidoscope-DynamicMacros.h"
#include "Kaleidoscope-DynamicSuperKeys.h"
#include "Kaleidoscope-EEPROM-Keymap.h"
#include "Kaleidoscope-FocusSerial.h"
#include "Kaleidoscope-IdleLEDsDefy.h"
#include "Kaleidoscope-LayerFocus.h"
#include "Kaleidoscope-MagicCombo.h"
#include "Kaleidoscope-MouseKeys.h"
#include "Kaleidoscope-USB-Quirks.h"
#include "Kaleidoscope.h"
// #include "RaiseIdleLEDs.h"
#include "DefyFirmwareVersion.h"
#include "kaleidoscope/device/dygma/defy_wireless/Focus.h"
// #include "kaleidoscope/device/dygma/defy_wireless/SideFlash.h"

// Support for host power management (suspend & wakeup)
#include "Kaleidoscope-HostPowerManagement.h"

#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-Qukeys.h"

// #include "LED-CapsLockLight.h"

// LED effects
#include "Colormap-Defy.h"
#include "LED-Palette-Theme-Defy.h"
#include "LEDEffect-BatteryStatus-Defy.h"
#include "LEDEffect-Bluetooth-Pairing-Defy.h"
#include "LEDEffect-Breathe-Defy.h"
#include "LEDEffect-Rainbow-Defy.h"
#include "LEDEffect-SolidColor-Defy.h"
#include "LEDEffect-Stalker-Defy.h"
// #include "DefaultColormap.h"

#include "Battery.h"
#include "Ble_manager.h"
#include "Communications.h"
#include "Radio_manager.h"
#include "Upgrade.h"
#include "nrf_fstorage.h"
#include "rf_host_device_api.h"
#include <Adafruit_TinyUSB.h>

#if !COMPILE_FOR_NEURON_2_HARDWARE_V1_0 && !COMPILE_FOR_NEURON_2_HARDWARE_V1_1
#warning "<<<<<<<<< The project is not being built for production >>>>>>>>>"
#endif


Watchdog_timer watchdog_timer;


// clang-format off
/*lint -save -e14 */
void app_error_fault_handler(uint32_t id, uint32_t pc, uint32_t info)  // On assert, the system can only recover with a reset.
{
    watchdog_timer.reset();
    NRF_BREAKPOINT_COND;

#ifndef DEBUG
    NRF_LOG_ERROR("Fatal error");
#else
    switch (id)
    {
#ifdef SOFTDEVICE_PRESENT
        case NRF_FAULT_ID_SD_ASSERT:
        {
            NRF_LOG_ERROR("SOFTDEVICE: ASSERTION FAILED");
        }
        break;

        case NRF_FAULT_ID_APP_MEMACC:
        {
            NRF_LOG_ERROR("SOFTDEVICE: INVALID MEMORY ACCESS");
        }
        break;
#endif
        case NRF_FAULT_ID_SDK_ASSERT:
        {
            assert_info_t *p_info = (assert_info_t *)info;
            NRF_LOG_ERROR("ASSERTION FAILED at %s:%u", p_info->p_file_name, p_info->line_num);
        }
        break;

        case NRF_FAULT_ID_SDK_ERROR:
        {
            error_info_t *p_info = (error_info_t *)info;
            NRF_LOG_ERROR("ERROR %u [%s] at %s:%u\r\nPC at: 0x%08x", p_info->err_code, nrf_strerror_get(p_info->err_code), p_info->p_file_name, p_info->line_num,
                          pc);
            NRF_LOG_ERROR("End of error report");
        }
        break;

        default:
        {
            NRF_LOG_ERROR("UNKNOWN FAULT at 0x%08X", pc);
        }
        break;
    }
#endif

    if (EEPROM.getNeedUpdate())
    {
        EEPROM.update();
    }
    NRF_LOG_FINAL_FLUSH();

    __disable_irq();

#ifndef DEBUG
    NRF_LOG_WARNING("<< Soft reset >>");
    NRF_LOG_FINAL_FLUSH();
    NVIC_SystemReset();  // Soft reset MCU.
#else
    app_error_save_and_stop(id, pc, info);
#endif // DEBUG
}
/*lint -restore */

namespace std
{
    void __throw_out_of_range_fmt(const char *, ...)
    {
        while (1);
    };
}

namespace std
{
    void __throw_bad_function_call()
    {
        while (1);
    };
}

namespace std
{
    void __throw_length_error(char const *)
    {
        while (1);
    };
}

namespace std
{
    void __throw_bad_alloc()
    {
        while (1);
    };
}

// Kaleidoscope
enum
{
    QWERTY,
    NUMPAD,
    _LAYER_MAX
}; // layers

/*
    This comments temporarily turns off astyle's indent enforcement so we can make
    the keymaps actually resemble the physical key layout better
*/
KEYMAPS(
[QWERTY] = KEYMAP_STACKED
(
    /* Left Side */
    Key_Escape, Key_1, Key_2, Key_3, Key_4, Key_5, Key_6,
    Key_Tab, Key_Q, Key_W, Key_E, Key_R, Key_T, Consumer_VolumeIncrement,
    Key_CapsLock, Key_A, Key_S, Key_D, Key_F, Key_G, Consumer_VolumeDecrement,
    Key_Backslash, Key_Z, Key_X, Key_C, Key_V, Key_B,
    Key_LeftControl, Key_LeftGui, Key_Backspace, Key_Delete,
    Key_LeftShift, Key_LeftAlt, Key_Enter, Key_Space,

    /* Right Side */
    Key_7, Key_8, Key_9, Key_0, Key_Minus, Key_Equals, Key_Backspace,
    Key_Y, Key_U, Key_I, Key_O, Key_P, Key_LeftBracket, Key_RightBracket,
    Key_H, Key_J, Key_K, Key_L, Key_Semicolon, Key_Quote, Key_RightShift,
    Key_N, Key_M, Key_Comma, Key_Period, Key_Slash, Key_RightShift,
    Key_LEDEffectNext, Key_Home, Key_UpArrow, Key_End,
    Key_RightArrow, Key_DownArrow, Key_LeftArrow, Key_Enter
),

[NUMPAD] = KEYMAP_STACKED
(
    /* Left Side */
    Key_Escape, Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6,
    Key_Tab, Key_NoKey, Key_UpArrow, Key_NoKey, Key_NoKey, Key_NoKey, Key_NoKey,
    Key_CapsLock, Key_LeftArrow, Key_DownArrow, Key_RightArrow, Key_NoKey, Key_NoKey, Key_NoKey,
    Key_LeftShift, Key_Backslash, Key_NoKey, Key_NoKey, Key_NoKey, Key_NoKey,
    Key_LeftControl, Key_LeftGui, Key_LeftAlt, Key_Space,
    Key_Space, Key_Backspace, Key_Enter, Key_Delete,

    /* Right Side */
    Key_F7, Key_F8, Key_F9, Key_F10, Key_F11, Key_F12, Key_Backspace,
    Key_KeypadSubtract, Key_7, Key_8, Key_9, Key_KeypadDivide, Key_NoKey, Key_Enter,
    Key_KeypadAdd, Key_4, Key_5, Key_6, Key_KeypadMultiply, Key_NoKey, Key_Backslash,
    Key_KeypadDot, Key_1, Key_2, Key_3, Key_UpArrow, Key_RightShift,
    Key_0, Key_Space, Key_LeftArrow, Key_DownArrow,
    Key_RightArrow, Key_RightControl, Key_Delete, MoveToLayer(QWERTY)
)
);
/* Re-enable astyle's indent enforcement */
// clang-format on

/** toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event)
{
    switch (event)
    {
        case kaleidoscope::plugin::HostPowerManagement::Suspend:
            LEDControl.disable();
            break;
        case kaleidoscope::plugin::HostPowerManagement::Resume:
            LEDControl.enable();
            break;
        case kaleidoscope::plugin::HostPowerManagement::Sleep:
            break;
    }
}

/** hostPowerManagementEventHandler dispatches power management events (suspend,
 * resume, and sleep) to other functions that perform action based on these
 * events.
 */
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event)
{
    toggleLedsOnSuspendResume(event);
}

enum
{
    COMBO_TOGGLE_NKRO_MODE
};

static uint32_t protocol_toggle_start = 0;

static void toggleKeyboardProtocol(uint8_t combo_index)
{
    USBQuirks.toggleKeyboardProtocol();
    protocol_toggle_start = Kaleidoscope.millisAtCycleStart();
}

static void protocolBreathe()
{
    if (Kaleidoscope.hasTimeExpired(protocol_toggle_start, uint16_t(10000)))
    {
        protocol_toggle_start = 0;
    }

    if (protocol_toggle_start == 0) return;

    uint8_t hue = 120;
    if (Kaleidoscope.hid().keyboard().getProtocol() == HID_BOOT_PROTOCOL)
    {
        hue = 0;
    }

    cRGB color = breath_compute(hue);
    ::LEDControl.setCrgbAt(KeyAddr(4, 0), color);
    ::LEDControl.setCrgbAt(KeyAddr(3, 0), color);
    ::LEDControl.setCrgbAt(KeyAddr(4, 2), color);
    ::LEDControl.setCrgbAt(KeyAddr(0, 6), color);
    ::LEDControl.syncLeds();
}

// clang-format off
USE_MAGIC_COMBOS(
{.action = toggleKeyboardProtocol,
// Left Ctrl + Left Shift + Left Alt + 6
.keys = {R4C0, R3C0, R4C2, R0C6}}
);
// clang-format on

// kaleidoscope::plugin::EEPROMPadding JointPadding(8);
static kaleidoscope::plugin::LEDSolidColorDefy solidRedDefy(255, 0, 0, 0);
static kaleidoscope::plugin::LEDSolidColorDefy solidGreenDefy(0, 255, 0, 0);
static kaleidoscope::plugin::LEDSolidColorDefy solidBlueDefy(0, 0, 255, 0);
static kaleidoscope::plugin::LEDSolidColorDefy solidWhiteDefy(0, 0, 0, 255);
static kaleidoscope::plugin::LEDSolidColorDefy solidBlackDefy(0, 0, 0, 0);
static kaleidoscope::plugin::LEDBatteryStatusDefy batteryStatus{};
static kaleidoscope::plugin::LEDStalkerDefy stalkerDefy{};

// clang-format off
KALEIDOSCOPE_INIT_PLUGINS(
FirmwareVersion, EEPROMSettings,
EEPROMKeymap, FocusSettingsCommand, FocusEEPROMCommand, Upgrade, DynamicSuperKeys,
LEDControl, FocusLEDCommand,
LEDPaletteThemeDefy, ColormapEffectDefy,
LEDRainbowWaveEffectDefy, LEDRainbowEffectDefy, stalkerDefy, solidRedDefy,
solidGreenDefy, solidBlueDefy, solidWhiteDefy, solidBlackDefy, batteryStatus, ledBluetoothPairingDefy,
IdleLEDsDefy, PersistentIdleDefyLEDs, DefyFocus, Qukeys, DynamicMacros,
/*SideFlash,*/ Focus, MouseKeys, OneShot, LayerFocus,
HostPowerManagement, Battery,
/*BLE*/
RadioManager, BleManager
);
// clang-format on
// End Kaleidoscope

static void gpio_output_voltage_setup(void);
static void init_gpio(void);
void reset_mcu(void);
void yield(void);

void setup(void)
{
    // RF Host library
    rfhdev_api_init();

    // GPIO
    gpio_output_voltage_setup();
    init_gpio();

    watchdog_timer.init();
    watchdog_timer.reset();
    HID().begin();

#if ENABLE_UART_DEBUG_LOG
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
#endif
    NRF_LOG_INFO("< N2 nRF52833 >");
    NRF_LOG_INFO("Initializing...");
    NRF_LOG_FLUSH();

    Communications.init();

    // Kaleidoscope
    Kaleidoscope.setup();
    EEPROMKeymap.setup(10);            // Reserve space in the keyboard's EEPROM(flash memory) for the keymaps.
    ColormapEffectDefy.max_layers(10); // Reserve space for the number of Colormap layers we will use.
    // DefaultColormap.setup();
    DynamicSuperKeys.setup(0, 1024);
    DynamicMacros.reserve_storage(2048);
}

void loop()
{
    watchdog_timer.reset();

    // Execute Kaleidoscope.
    Kaleidoscope.loop();
    Communications.run();
    protocolBreathe();
    EEPROM.timer_update_periodically_run(1000);  // Check if it is necessary to write the eeprom every 1000 ms.

    NRF_LOG_PROCESS(); // Process deferred logs (send it to the host computer via UART).

    /* Control the sleep mode here */
    /* We need a way of deciding on when we can go to sleep mode and when we should continue. */
    /* When we have such process, we need to add it to the rf_glue.c mcu_sleep_postpone( ) */
    // mcu_sleep_control( );

    // Even if we miss an event enabling USB, USB event would wake us up.
    //__WFE();
    // Clear SEV flag if CPU was woken up by event.
    //__SEV();
    //__WFE();
}


static void gpio_output_voltage_setup(void)
{
    // Configure UICR_REGOUT0 register only if it is set to default value.
    if ((NRF_UICR->REGOUT0 & UICR_REGOUT0_VOUT_Msk) != (UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos))
    {
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        NRF_UICR->REGOUT0 = (NRF_UICR->REGOUT0 & ~((uint32_t)UICR_REGOUT0_VOUT_Msk)) |
                            (UICR_REGOUT0_VOUT_3V3 << UICR_REGOUT0_VOUT_Pos);

        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy);

        // System reset is needed to update UICR registers.
        NVIC_SystemReset();  // Soft reset MCU.
        NRF_UICR->REGOUT0;
    }
}

static void init_gpio(void)
{
    // Configure pins as Inputs
    nrf_gpio_cfg_input(BTN_RESET, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BTN_BOOT, NRF_GPIO_PIN_PULLUP);

    nrf_gpio_cfg_output(SIDE_NRESET_1);
    nrf_gpio_cfg_output(SIDE_NRESET_2);

    nrf_gpio_pin_write(SIDE_NRESET_1, 0);
    nrf_gpio_pin_write(SIDE_NRESET_2, 0);

/* Only for DEBUG */
#if (COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_0 | COMPILE_FOR_DEBUG_BOARD_HARDWARE_V1_1)
    // Configure pins as Inputs
    nrf_gpio_cfg_input(BTN1, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BTN2, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BTN3, NRF_GPIO_PIN_PULLUP);

    // Configure pins as Outputs
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_cfg_output(LED3);
    nrf_gpio_cfg_output(LED8);
#endif
}

// Lest implement the reset_mcu so that if we have something to write to the flash is goin to wait for the procedure to finish.
void reset_mcu(void)
{
    while (nrf_fstorage_is_busy(NULL))  // Wait until fstorage is available.
    {
        yield();  // Meanwhile execute tasks.
    }

    if (EEPROM.getNeedUpdate())
    {
        watchdog_timer.reset();
        EEPROM.update();
    }

    sd_softdevice_disable();  // Disable SD.

    // Disable all interrupts
    NVIC->ICER[0] = 0xFFFFFFFF;
    NVIC->ICPR[0] = 0xFFFFFFFF;
#if (defined(__NRF_NVIC_ISER_COUNT) && (__NRF_NVIC_ISER_COUNT == 2))
    NVIC->ICER[1] = 0xFFFFFFFF;
    NVIC->ICPR[1] = 0xFFFFFFFF;
#endif

    NRF_LOG_WARNING("<< Soft reset >>");
    NRF_LOG_FINAL_FLUSH();
    NVIC_SystemReset();  // Soft reset MCU.

    // maybe yield ?
    while (1)
    {
        yield();
    }
}

void yield(void)
{
    watchdog_timer.reset();

#ifdef USE_TINYUSB
    TinyUSB_Device_FlushCDC();
#endif

    if(ble_connected())
    {
        RawHID.flush();
    }

    //FreeRTOS yield to other tasks
#ifdef USE_TINYUSB
    HID().SendLastReport();
    TinyUSB_Device_Task();
#endif

    if(ble_innited())
    {
        ble_run();
    }
}
