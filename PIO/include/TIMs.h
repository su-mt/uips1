#ifndef TIMS_H
#define TIMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stm32f4xx_hal.h"

// Глобальные переменные таймеров
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim2;

// Функции инициализации
void MX_TIM3_Init(void);
void MX_TIM2_Init(void);

void SystemClock_Config();


#ifdef __cplusplus
}
#endif

#endif // TIMS_H

