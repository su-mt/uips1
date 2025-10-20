
#ifndef UART2_H
#define UART2_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "utils.h"

// Глобальные переменные UART2
extern UART_HandleTypeDef huart2;
extern uint8_t uart2Buff[21];
extern uint8_t uart2_rxByte;
extern uint8_t uart2_rxCount;

// На F3 - 0x80, 0x00, 0x01, 0x1X, 0xYZ, 0x5x (X,YZ– двоично-десятичное число; 0x5x- младшие два бита отображают ОСНОВНОЙ и РЕЗЕРВНЫЙ).

typedef struct __attribute__((packed)) buf_CurrentResponse {
    uint8_t address_1;      // 0x80 - фиксированный
    uint8_t address_2;      // 0x00 - фиксированный
    uint8_t address_3;      // 0x01 - фиксированный
    uint8_t voltage_high;   // 0x1X - старший разряд напряжения
    uint8_t voltage_low;    // 0xYZ - младшие разряды напряжения
    uint8_t status;         // 0x5x - статус каналов
}buf_CurrentResponse;

buf_CurrentResponse get_buf_CurrResp (uint16_t vol_mv, bool main_status, bool reserve_status ) ;


void MX_USART2_UART_Init(void);

// Функции обработки
void huart2_Handler(bool overflow);
bool iscorrect(uint8_t* buff);

// Команды протокола
void uips_start(void);
void uips_stop(void);
void uips_getCurrent(void);
void uips_getResistance(void);
void uips_getConsts(void);
void uips_err(void);


void uips_saveConsts();
void uips_saveCurr();

#ifdef __cplusplus
}
#endif

#endif // UART2_H

