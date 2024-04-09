

#ifndef CRC_H_
#define CRC_H_

#include "utils/crc32.h"
#include "stdio.h"

uint32_t crc32(const uint8_t *ptr, uint32_t len);


uint8_t crc8(uint8_t const msg[], uint32_t len);

#endif
