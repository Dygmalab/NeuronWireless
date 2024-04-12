#-------------------------------------------------------------------------------
# Cross-compiler toolchains
#-------------------------------------------------------------------------------

# CodeSourcery cross-compiler toolchain path
CS_CROSS_DIR_PATH =

# CodeSourcery cross-compiler toolchain executable prefixes
# for "ARM NONE EABI" tools
CS_CROSS_PREFIX_ARM_NONE_EABI = "arm-none-eabi-"

#-------------------------------------------------------------------------------
# MCU library packages
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Definitions for particular target MCUs
#-------------------------------------------------------------------------------

ifeq ($(TARGET_MCU),nRF52833_QIAA)
  include $(PLATFORM_BUILD_DIR)/MakeMcu/MakeNrf52833Common.mk
else
  $(error The target mcu variant TARGET_MCU was not defined.)
endif

