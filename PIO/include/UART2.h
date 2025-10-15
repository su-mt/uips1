
#ifndef UART2_H
#define UART2_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

// Глобальные переменные UART2
extern UART_HandleTypeDef huart2;
extern uint8_t uart2Buff[5];
extern uint8_t uart2_rxByte;
extern uint8_t uart2_rxCount;

// Функции инициализации (объявлена в CubeMX _main.c)
void MX_USART2_UART_Init(void);

// Функции обработки
void huart2_Handler(bool overflow);
uint8_t checksum(uint8_t* buff, char length);
bool iscorrect(uint8_t* buff);

// Команды протокола
void uips_start(void);
void uips_stop(void);
void uips_getCurrent(void);
void uips_getResistance(void);
void uips_getConsts(void);
void uips_err(void);

#ifdef __cplusplus
}
#endif

#endif // UART2_H

