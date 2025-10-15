#ifndef TIM3_H
#define TIM3_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"

// Глобальная переменная TIM3
extern TIM_HandleTypeDef htim3;

// Функции инициализации
void MX_TIM3_Init(void);

#ifdef __cplusplus
}
#endif

#endif // TIM3_H
