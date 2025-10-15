
#ifndef UART6_H
#define UART6_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>


extern UART_HandleTypeDef huart6;

void MX_USART6_UART_Init(void);

// Функции обработки
void huart6_Handler(bool overflow);

#ifdef __cplusplus
}
#endif

#endif // UART6_H

