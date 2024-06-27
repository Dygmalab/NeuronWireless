#-------------------------------------------------------------------------------
# Application name (project name)
#-------------------------------------------------------------------------------

APP_NAME = Wireless_neuron

#-------------------------------------------------------------------------------
# Common, platform and project root directories
#-------------------------------------------------------------------------------

# Common root directory
COMMON_ROOT_DIR = ../

# Application root directory
APP_ROOT_DIR = $(COMMON_ROOT_DIR)/

# Application build directory
APP_BUILD_DIR = $(APP_ROOT_DIR)/build/

# Libraries root directory
LIB_ROOT_DIR = $(APP_ROOT_DIR)/libraries/

# Platform build directory
PLATFORM_BUILD_DIR = $(APP_BUILD_DIR)/platform/

# SDK root directories
NRF5_SDK = nRF5_SDK_17.1.0_ddde560

SDK_ROOT_DIR = $(LIB_ROOT_DIR)/SDK/
SDK_NRF5_SDK_ROOT_DIR = $(SDK_ROOT_DIR)/$(NRF5_SDK)/

# Kaleidoscope root directory
KALEIDOSCOPE_ROOT_DIR = $(LIB_ROOT_DIR)/Kaleidoscope/
KEYBOARDIO_HID_ROOT_DIR = $(LIB_ROOT_DIR)/KeyboardioHID/

# Utils root directory
UTILS_ROOT_DIR = $(COMMON_ROOT_DIR)/utils/

#-------------------------------------------------------------------------------
# Include common definitions
#-------------------------------------------------------------------------------

# Common definitions
include $(PLATFORM_BUILD_DIR)/MakeDefsCommon.mk

#-------------------------------------------------------------------------------
# BSP Target board variant
#-------------------------------------------------------------------------------

# Hardware board name
#BSP_TARGET_BOARD = BSP_DEFY_N2_DEBUG
BSP_TARGET_BOARD = BSP_DEFY_N2

#-------------------------------------------------------------------------------
# Include target MCU specific definitions
#-------------------------------------------------------------------------------

# Target specific MCU definitions
include $(APP_BUILD_DIR)/MakeDefsBsp.mk


#-------------------------------------------------------------------------------
# Include target MCU specific definitions
#-------------------------------------------------------------------------------

# Target specific MCU definitions
ARDUINO_BUILD_DIR = $(LIB_ROOT_DIR)/Arduino/build
include $(ARDUINO_BUILD_DIR)/MakeArduinoNRF52833.mk

#-------------------------------------------------------------------------------
# Path and toolchain executable prefix to run particular build process tools
#-------------------------------------------------------------------------------

CROSS_FW = $(CS_CROSS_DIR_PATH)$(CS_CROSS_PREFIX_ARM_NONE_EABI)

#-------------------------------------------------------------------------------
# Flags for compiler, assembler and linker
#-------------------------------------------------------------------------------

# Flags common to compiler, assembler and linker
COMMONFLAGS += -mcpu=cortex-m4
COMMONFLAGS += -mfpu=fpv4-sp-d16
COMMONFLAGS += -mthumb -mabi=aapcs -fmessage-length=0 -fsigned-char -fno-builtin -fno-strict-aliasing
COMMONFLAGS += -fsingle-precision-constant
COMMONFLAGS += -ffunction-sections
COMMONFLAGS += -fdata-sections
COMMONFLAGS += -Wall
COMMONFLAGS += -mbe32

# Compiler flags
CFLAGS += $(COMMONFLAGS)
CFLAGS += -Wno-switch -std=c11

CXXFLAGS += $(COMMONFLAGS)
CXXFLAGS += -Wno-switch -std=c++11 -fno-exceptions -fno-rtti
# Handling the Kaleidoscope warnings
CXXFLAGS += -Wno-write-strings

# Assembler flags
ASFLAGS += $(COMMONFLAGS)

# Linker flags
LFLAGS += $(COMMONFLAGS) -L$(LIBDIR) -Wl,--gc-sections
LFLAGS += -specs=nano.specs -specs=nosys.specs -lc -lnosys -lm
LFLAGS += -Wl,-T./init/device/$(TARGET_MCU_DIR_NAME)/flash.ld


#-------------------------------------------------------------------------------
# Base defines
#-------------------------------------------------------------------------------

# Memory
DEFINES += -D__STACK_SIZE=2048
DEFINES += -D__HEAP_SIZE=2048

# GCC toolchain is used for building process
DEFINES += -Dgcc

# THUMB mode
DEFINES += -D_THUMB

#-------------------------------------------------------------------------------
# SDK defines
#-------------------------------------------------------------------------------

# Softdevice
DEFINES += -DSOFTDEVICE_PRESENT
DEFINES += -DS140

# HAL
#DEFINES += -DCONFIG_GPIO_AS_PINRESET
DEFINES += -DCONFIG_NFCT_PINS_AS_GPIOS

# System timer
DEFINES += -DAPP_TIMER_V2
DEFINES += -DAPP_TIMER_V2_RTC1_ENABLED

# Others
DEFINES += -DNRFX_PPI_ENABLED=1
DEFINES += -DNRF52_ARCH


#-------------------------------------------------------------------------------
# RF Host Device
#-------------------------------------------------------------------------------

RF_HOST_DEVICE_HEX = $(LIB_ROOT_DIR)/rf_host_device/rf_host_device/bin/rf_host_device.hex

#-------------------------------------------------------------------------------
# Library search directiories
#-------------------------------------------------------------------------------

LIBDIR += -L./init/device/$(TARGET_MCU_DIR_NAME)

#-------------------------------------------------------------------------------
# Include search directories
#-------------------------------------------------------------------------------

# Application project
INCDIR += -I$(APP_ROOT_DIR)/
INCDIR += -I$(LIB_ROOT_DIR)/
INCDIR += -I$(LIB_ROOT_DIR)/Ble_composite_dev/
INCDIR += -I$(LIB_ROOT_DIR)/Communications/src/
INCDIR += -I$(LIB_ROOT_DIR)/Fifo_buffer/
INCDIR += -I$(LIB_ROOT_DIR)/EEPROM/
INCDIR += -I$(LIB_ROOT_DIR)/NeuronLedLibrary/
INCDIR += -I$(LIB_ROOT_DIR)/NeuronLedLibrary/LedModeSerializable/
INCDIR += -I$(LIB_ROOT_DIR)/rf_host_device/
INCDIR += -I$(LIB_ROOT_DIR)/rf_host_device/rf_host_device/includes/
INCDIR += -I$(LIB_ROOT_DIR)/rf_host_device/rf_host_device/includes/middleware/
INCDIR += -I$(LIB_ROOT_DIR)/rf_host_device/rf_host_device/includes/rf_gateway/
INCDIR += -I$(LIB_ROOT_DIR)/rf_host_device/rf_host_device/includes/rf_protocol/
INCDIR += -I$(LIB_ROOT_DIR)/Spi_slave/
INCDIR += -I$(LIB_ROOT_DIR)/Status_leds/
INCDIR += -I$(LIB_ROOT_DIR)/Time_counter/
INCDIR += -I$(LIB_ROOT_DIR)/Watchdog_timer/
INCDIR += -I$(LIB_ROOT_DIR)/Wire/
INCDIR += -I$(LIB_ROOT_DIR)/Twi_master/
INCDIR += -I$(LIB_ROOT_DIR)/UpgradeKeyscanner/include/
INCDIR += -I$(LIB_ROOT_DIR)/UpgradeKeyscanner/src/
INCDIR += -I$(LIB_ROOT_DIR)/CRC/
INCDIR += -I$(LIB_ROOT_DIR)/Battery_manager/
INCDIR += -I$(LIB_ROOT_DIR)/Ble_manager/
INCDIR += -I$(LIB_ROOT_DIR)/RF_manager/
INCDIR += -I$(LIB_ROOT_DIR)/utils/Do_once/
INCDIR += -I$(LIB_ROOT_DIR)/utils/Run_task_once/
INCDIR += -I$(LIB_ROOT_DIR)/utils/Timer/

# Kaleidoscope
INCDIR += -I$(KALEIDOSCOPE_ROOT_DIR)/src/
INCDIR += -I$(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/device/dygma/
INCDIR += -I$(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/device/dygma/defy_wireless/

# SDK

INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_advertising/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_dtm/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_link_ctx_manager/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_services/ble_bas/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_services/ble_dis/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_services/ble_hids/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/common/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/nrf_ble_gatt/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/nrf_ble_qwr/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/ble/peer_manager/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/atomic/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/atomic_fifo/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/atomic_flags/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/balloc/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/bootloader/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/bootloader/dfu/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/delay/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/experimental_section_vars/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/fds/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/fstorage/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/log/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/log/src/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/memobj/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/mutex/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/pwr_mgmt/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/ringbuf/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/scheduler/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/sortlist/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/strerror/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/timer/

INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/util/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/softdevice/common/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/softdevice/s140/headers/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/softdevice/s140/headers/nrf52
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/components/toolchain/cmsis/include/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/external/fprintf/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/integration/nrfx/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/integration/nrfx/legacy/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/include/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/hal/
INCDIR += -I$(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/mdk/

#-------------------------------------------------------------------------------
# C sources
#-------------------------------------------------------------------------------
# Application project
SRCSC += $(wildcard $(APP_ROOT_DIR)/*.c)
SRCSCXX += $(wildcard $(APP_ROOT_DIR)/*.cpp)

SRCSC += $(LIB_ROOT_DIR)/Ble_composite_dev/Ble_composite_dev.c
SRCSC += $(LIB_ROOT_DIR)/Ble_composite_dev/ble_hid_service.c
SRCSC += $(LIB_ROOT_DIR)/rf_host_device/rf_host_device_api.c
SRCSC += $(LIB_ROOT_DIR)/Time_counter/Time_counter.c

SRCSCXX += $(LIB_ROOT_DIR)/DefyFirmwareVersion.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Fifo_buffer/Fifo_buffer.cpp
SRCSCXX += $(LIB_ROOT_DIR)/EEPROM/EEPROM.cpp
SRCSCXX += $(LIB_ROOT_DIR)/NeuronLedLibrary/Colormap-Defy.cpp
SRCSCXX += $(LIB_ROOT_DIR)/NeuronLedLibrary/LEDEffect-BatteryStatus.cpp
SRCSCXX += $(LIB_ROOT_DIR)/NeuronLedLibrary/LEDEffect-Bluetooth-Pairing-Defy.cpp
SRCSCXX += $(LIB_ROOT_DIR)/NeuronLedLibrary/LEDEffect-Rainbow-Defy.cpp
SRCSCXX += $(LIB_ROOT_DIR)/NeuronLedLibrary/LEDEffect-SolidColor-Defy.cpp
SRCSCXX += $(LIB_ROOT_DIR)/NeuronLedLibrary/LEDEffect-Stalker-Defy.cpp
SRCSCXX += $(LIB_ROOT_DIR)/NeuronLedLibrary/LED-Palette-Theme-Defy.cpp

SRCSCXX += $(LIB_ROOT_DIR)/Spi_slave/Spi_slave.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Spi_slave/SpiPort.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Status_leds/Status_leds.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Watchdog_timer/Watchdog_timer.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Twi_master/Twi_master.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Wire/Wire.cpp
SRCSCXX += $(LIB_ROOT_DIR)/UpgradeKeyscanner/src/KeyScannerFlasher.cpp
SRCSCXX += $(LIB_ROOT_DIR)/UpgradeKeyscanner/src/Upgrade.cpp
SRCSCXX += $(LIB_ROOT_DIR)/CRC/CRC_wrapper.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Battery_manager/Battery.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Ble_manager/Ble_manager.cpp
SRCSCXX += $(LIB_ROOT_DIR)/RF_manager/Radio_manager.cpp
SRCSCXX += $(LIB_ROOT_DIR)/utils/Timer/Timer.cpp

# SDK
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_advertising/ble_advertising.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_services/ble_bas/ble_bas.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_services/ble_dis/ble_dis.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/ble_services/ble_hids/ble_hids.c
SRCSC += $(wildcard $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/common/*.c)
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c
SRCSC += $(wildcard $(SDK_NRF5_SDK_ROOT_DIR)/components/ble/peer_manager/*.c)

SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/atomic/nrf_atomic.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/atomic_fifo/nrf_atfifo.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/atomic_flags/nrf_atflags.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/balloc/nrf_balloc.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/experimental_section_vars/nrf_section_iter.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/fds/fds.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/fstorage/nrf_fstorage.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/fstorage/nrf_fstorage_nvmc.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/fstorage/nrf_fstorage_sd.c
SRCSC += $(wildcard $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/log/src/*.c)
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/memobj/nrf_memobj.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/strerror/nrf_strerror.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/scheduler/app_scheduler.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/timer/app_timer2.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/timer/drv_rtc.c

SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/sortlist/nrf_sortlist.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/util/app_error.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/util/app_error_handler_gcc.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/util/app_error_weak.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/util/app_util_platform.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/libraries/util/nrf_assert.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/softdevice/common/nrf_sdh.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/components/softdevice/common/nrf_sdh_ble.c

SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/integration/nrfx/legacy/nrf_drv_clock.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/integration/nrfx/legacy/nrf_drv_power.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/integration/nrfx/legacy/nrf_drv_spis.c

SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/nrfx_clock.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/nrfx_power.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/nrfx_ppi.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/nrfx_spis.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/nrfx_timer.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/nrfx_twim.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/prs/nrfx_prs.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/hal/nrf_nvmc.c
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/drivers/src/nrfx_pwm.c

# Kaleidoscope
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/device/dygma/defy_wireless/bootloader_util.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/device/dygma/defy_wireless/Hand.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/device/dygma/defy_wireless/Focus.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/device/dygma/Defy_wireless.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/key_events.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/layers.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/DynamicMacros.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/DynamicSuperKeys.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/EEPROM-Keymap.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/EEPROM-Settings.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/EEPROM-Settings/crc.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/FocusSerial.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/HostPowerManagement.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/IdleLEDsDefy.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/LayerFocus.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/LEDControl/LEDUtils.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/LEDControlDefy.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/MouseKeys.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/MouseKeys/MouseWrapper.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/OneShot.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/plugin/Qukeys.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope/Runtime.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope_internal/device.cpp
SRCSCXX += $(KALEIDOSCOPE_ROOT_DIR)/src/kaleidoscope_internal/LEDModeManager.cpp

SRCSCXX += $(LIB_ROOT_DIR)/Communications/src/device/defyN2/CommunicationsN2.cpp
SRCSCXX += $(LIB_ROOT_DIR)/Communications/src/Communications_protocol_rf.cpp

ifdef FIRMWARE_VERSION
	DEFINES += -DFIRMWARE_VERSION=\"${FIRMWARE_VERSION}\"
endif
DEFINES += -DBOARD_VENDORID=0x35EF
DEFINES += -DBOARD_PRODUCTID=0x0012
DEFINES += -DBOARD_MANUFACTURER=\"DYGMA\"
DEFINES += -DBOARD_PRODUCT=\"DEFY\"
DEFINES += -DUSB_PRODUCT=\"DEFY\"
DEFINES += -DKALEIDOSCOPE_HARDWARE_H=\"kaleidoscope/device/dygma/Defy_wireless.h\"
DEFINES += -DKALEIDOSCOPE_LED_CONTROL_H=\<kaleidoscope/plugin/LEDControlDefy.h\>
DEFINES += -DEEPROM_EMULATION_SIZE=8192


# Keyboardio HID
INCDIR += -I$(KEYBOARDIO_HID_ROOT_DIR)/src/
SRCSCXX += $(KEYBOARDIO_HID_ROOT_DIR)/src/MultiReport/ConsumerControl.cpp
SRCSCXX += $(KEYBOARDIO_HID_ROOT_DIR)/src/MultiReport/Keyboard.cpp
SRCSCXX += $(KEYBOARDIO_HID_ROOT_DIR)/src/MultiReport/Mouse.cpp
SRCSCXX += $(KEYBOARDIO_HID_ROOT_DIR)/src/MultiReport/RawHID.cpp
SRCSCXX += $(KEYBOARDIO_HID_ROOT_DIR)/src/MultiReport/SystemControl.cpp
SRCSCXX += $(KEYBOARDIO_HID_ROOT_DIR)/src/hidDefy.cpp
SRCSCXX += $(KEYBOARDIO_HID_ROOT_DIR)/src/HIDReportObserver.cpp

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

.PHONY: all release debug print_build_start print_build_end $(BINARY_OUTPUT) clean progr progd

all: release

release: COMMONFLAGS += -Os -s
#release: COMMONFLAGS += -O0 -g
release: print_build_start $(BINARY_OUTPUT) print_build_end

debug: COMMONFLAGS += -O1 -g
#debug: COMMONFLAGS += -O0 -g
debug: DEFINES += -DDEBUG -DDEBUG_NRF
debug: print_build_start $(BINARY_OUTPUT) print_build_end

print_build_start:
	@echo "Build process for $(APP_NAME) project in $(MODE) mode started ..."
	@echo "Compiling ..."

print_build_end:
	@echo "Build process for $(APP_NAME) project in $(MODE) mode finished."

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),progr)
ifneq ($(MAKECMDGOALS),progd)

include $(PLATFORM_BUILD_DIR)/MakeRulesCommon.mk

endif
endif
endif


$(BINARY_OUTPUT): $(OBJSC) $(OBJSCXX) $(OBJSA) $(OBJSa)
	@echo "Compiling done."
	@echo "Linking ..."
	$(CXX) -Xlinker -Map -Xlinker $@.map $(LFLAGS) -o $@.elf $(OBJSC) $(OBJSCXX) $(OBJSA) $(OBJSa) $(LIBS)
#	$(OBJDUMP) -D -S $@.elf > $@.lst
	@echo "Linking done."
	$(SIZE) -A $@.elf
	@echo "Creating application binaries ..."
	$(OBJCOPY) -O binary $@.elf $@.bin
	$(OBJCOPY) -O ihex $@.elf $@.hex
	@echo "Creating final application binary ..."
	python $(UTILS_ROOT_DIR)/fw_package_builder.py $(RF_HOST_DEVICE_HEX) $@.hex $@_final.hex
	@echo "Creating application binaries done."

clean:
	@echo "Cleaning $(APP_NAME) ..."
	$(RM) $(MODE_DEBUG_DIR_NAME)
	$(RM) $(MODE_DEBUG_DIR_NAME)/$(APP_NAME).*
	$(RM) $(MODE_RELEASE_DIR_NAME)
	$(RM) $(MODE_RELEASE_DIR_NAME)/$(APP_NAME).*
	@echo "Cleaning done."
	

list: $(MODE_DEBUG_DIR_NAME)/$(APP_NAME).elf
	$(OBJDUMP) -D -S ./$(MODE_DEBUG_DIR_NAME)/$(APP_NAME).elf > ./$(MODE_DEBUG_DIR_NAME)/$(APP_NAME).lst

ddd:
	$(DDD) --debugger $(GDB) -x $(DDD_INIT_FILENAME) ./$(MODE_DEBUG_DIR_NAME)/$(APP_NAME).elf


#-------------------------------------------------------------------------------
# End of file
#-------------------------------------------------------------------------------
