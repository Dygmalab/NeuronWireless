/* -*- mode: c++ -*-
 * kaleidoscope::device::dygma::Wired -- Kaleidoscope device plugin for Dygma Wired
 * Copyright (C) 2017-2019  Keyboard.io, Inc
 * Copyright (C) 2017-2019  Dygma Lab S.L.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "SpiPort.h"

#include "Fifo_buffer.h"


// SPI0
#if COMPILE_SPI0_SUPPORT
Spi_slave spi0_slave(0,                /* Chip SPI port used */
                     PIN_MISO0,        /* MISO0 */
                     PIN_MOSI0,        /* MOSI0 */
                     PIN_CLK0,         /* Clock0 */
                     PIN_CS0,          /* Chip Select 0 */
                     NRF_SPIS_MODE_1); /* -> CPOL = 0 / CPHA = 1 */
#endif

// SPI1
#if COMPILE_SPI1_SUPPORT
Spi_slave spi1_slave(1,                /* Chip SPI port used */
                     PIN_MISO1,        /* MISO1 */
                     PIN_MOSI1,        /* MOSI1 */
                     PIN_CLK1,         /* Clock1 */
                     PIN_CS1,          /* Chip Select 1 */
                     NRF_SPIS_MODE_1); /* -> CPOL = 0 / CPHA = 1 */
#endif

// SPI2
#if COMPILE_SPI2_SUPPORT
Spi_slave spi2_slave(2,                /* Chip SPI port used */
                     PIN_MISO2,        /* MISO2 */
                     PIN_MOSI2,        /* MOSI2 */
                     PIN_CLK2,         /* Clock2 */
                     PIN_CS2,          /* Chip Select 2 */
                     NRF_SPIS_MODE_1); /* -> CPOL = 0 / CPHA = 1 */
#endif


SpiPort::SpiPort(uint8_t _spi_port_used)
  : spi_port_used(_spi_port_used) {
#if COMPILE_SPI0_SUPPORT
    if (_spi_port_used == 0) {
        spi_slave = &spi0_slave;
    }
#endif

#if COMPILE_SPI1_SUPPORT
    if (_spi_port_used == 1) {
        spi_slave = &spi1_slave;
    }
#endif

#if COMPILE_SPI2_SUPPORT
    if (_spi_port_used == 2) {
        spi_slave = &spi2_slave;
    }
#endif
}

void SpiPort::init() {
    if (spi_slave == nullptr) return;

    spi_slave->init();  // Initialice SPI slave.
}

void SpiPort::deInit() {
    if (spi_slave == nullptr) return;

    spi_slave->deinit();
}

bool SpiPort::sendPacket(Packet &packet) {
    if (spi_slave == nullptr) return false;

    spi_slave->tx_fifo->put(&packet);

    return true;
}

void SpiPort::clearSend() {
    if (spi_slave == nullptr) return ;

    while (!spi_slave->tx_fifo->is_empty()){
        spi_slave->tx_fifo->removeOne();
    }

}

void SpiPort::clearRead() {
    if (spi_slave == nullptr) return ;

    while (!spi_slave->rx_fifo->is_empty()){
        spi_slave->rx_fifo->removeOne();
    }

}

bool SpiPort::readPacket(Packet &packet) {
    if (spi_slave == nullptr) return false;

    if (spi_slave->rx_fifo->is_empty()) return false;

    spi_slave->rx_fifo->get(&packet);

    return true;
}
