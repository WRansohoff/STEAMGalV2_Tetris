#ifndef _VVC_SSPI_H
#define _VVC_SSPI_H

#include "global.h"

// Define GPIOB pin mappings for software '4-wire' SPI.
#define PB_MOSI (5)
#define PB_SCK  (3)
#define PB_DC   (4)
#define PA_CS   (15)
#define PA_RST  (12)

// Write a byte of data using software SPI.
inline void sspi_w(uint8_t dat);
// Write a 'command' byte for 4-wire SPI interfaces.
inline void sspi_cmd(uint8_t cdat);

#endif
