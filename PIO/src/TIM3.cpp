#include "TIM3.h"
#include "UART2.h"

// Глобальная переменная TIM3
TIM_HandleTypeDef htim3;

// Инициализация TIM3
void MX_TIM3_Init() {
    uint32_t sysclk = HAL_RCC_GetSysClockFreq();  // Системная частота ядра
    
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = sysclk / 100000 - 1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    // Period: 100000 / 2873.56 ≈ 34.8 → 35 тиков
    htim3.Init.Period = 35 - 1;  // ~0.348 мс (период приема 4 байт)
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        Error_Handler();
    }

    HAL_TIM_Base_Start_IT(&htim3);
}

// Callback при переполнении таймера - вызывается при переполнении - 0.348 мс
extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance == TIM3) {
        huart2_Handler(true);
    }
}
