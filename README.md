# NeuronWireless
nRF MCU family version for the Neuron Platform

## Target device
NeuronWireless is built to be loaded into the Nordic Semiconductor nRF52833 microcontroller.

## FLASH Memory Architecture

|                       |                           |
|-----------------------|---------------------------|
| NeuronWireless        | 0x0002E000 - ...          |
|-----------------------|---------------------------|
| rf_host_device        | 0x00027000 - 0x0002DFFF   |
|-----------------------|---------------------------|
| Softdevice S140       | 0x00001000 - 0x00026FFF   |
| MBR                   | 0x00000000 - 0x00000FFF   |

## Requirements
* `make`
* `gcc-arm-none-eabi`
* `python 3.11`
* `IntelHex` Python library

To cover these requirements in Ubuntu based distributions, install the `build-essential` package. For other Linux distros install them independently.

Install the toolchain for the arm chips

```sudo apt install gcc-arm-none-eabi```

For installing the `IntelHex` call

```sudo pip install intelhex```

## Preparations

* Clone this repository on your local drive
* Update submodules `git submodule update --init --recursive`
* Download the Nordic Semiconductor SDK [nrf5_sdk_17.1.0_ddde560](https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_17.1.0_ddde560.zip)
* Unpack the `nrf5_sdk_17.1.0_ddde560` into the `libraries/SDK/nRF5_SDK_17.1.0_ddde560` folder

## Build

`cd NeuronWireless/build`

`make release` or `make debug`
