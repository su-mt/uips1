
#ifndef UTILS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

uint8_t checksum(uint8_t* buff, char length);

#ifdef __cplusplus
}
#endif

#endif // UTILS_H




