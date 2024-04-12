
  # Name of directory containing target MCU specific content
  TARGET_MCU_DIR_NAME = nRF52833_QIAA

  # Flags common to compiler, assembler and linker
  COMMONFLAGS += -mfloat-abi=hard

  # MCU device
  DEFINES += -DNRF52833_XXAA
  
  DEFINES += -DMCU_ALIGNMENT_SIZE=4


#-------------------------------------------------------------------------------
# SDK defines
#-------------------------------------------------------------------------------

DEFINES += -DNRFX_GPIOTE_CONFIG_NUM_OF_LOW_POWER_EVENTS=42	# P0_PIN_NUM + P1_PIN_NUM .. 32 + 10

#-------------------------------------------------------------------------------
# Application defines
#-------------------------------------------------------------------------------

  DEFINES += -DHAL_CFG_MCU=HAL_MCU_NRF52833

#-------------------------------------------------------------------------------
# Root directories
#-------------------------------------------------------------------------------


#-------------------------------------------------------------------------------
# Library search directiories
#-------------------------------------------------------------------------------

# SDK

#-------------------------------------------------------------------------------
# Include search directories
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# C sources
#-------------------------------------------------------------------------------

# System initialization
SRCSA += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/mdk/gcc_startup_nrf52833.S

# SDK
SRCSC += $(SDK_NRF5_SDK_ROOT_DIR)/modules/nrfx/mdk/system_nrf52833.c

