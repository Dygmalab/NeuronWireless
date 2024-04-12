


#-------------------------------------------------------------------------------
# BSP Target board variant
#-------------------------------------------------------------------------------

ifeq ($(BSP_TARGET_BOARD),BSP_DEFY_N2_DEBUG)
  TARGET_MCU = nRF52833_QIAA
else ifeq ($(BSP_TARGET_BOARD),BSP_DEFY_N2)
  TARGET_MCU = nRF52833_QIAA
else
  $(error The target board variant BSP_TARGET_BOARD was not defined.)
endif

# Hardware board selection macro
DEFINES += -D$(BSP_TARGET_BOARD)

#-------------------------------------------------------------------------------
# Definitions for particular target MCUs
#-------------------------------------------------------------------------------

# Target specific MCU definitions
include $(PLATFORM_BUILD_DIR)/MakeDefsMcu.mk

