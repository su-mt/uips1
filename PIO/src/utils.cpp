
#ifndef UART2_H
#define UART2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

uint8_t checksum(uint8_t* buff, char length) {
    uint8_t checksum = 0;
    for(uint8_t i = 0; i < length; i++) {
        checksum ^= buff[i];
    }
    return checksum;
}



#ifdef __cplusplus
}
#endif

#endif // UART2_H




