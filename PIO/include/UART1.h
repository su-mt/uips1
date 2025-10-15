
#ifndef UART1_H
#define UART1_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>


extern UART_HandleTypeDef huart6;

void MX_USART1_UART_Init(void);

// Функции обработки
void huart1_Handler(bool overflow);

#ifdef __cplusplus
}
#endif

#endif // UART1_H

