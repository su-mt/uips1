#include "ADC.hpp"
#include "PowerManager.hpp"
#include "constants.hpp"
#include "main.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal_gpio.h"
#include <cstring>


// Глобальные переменные АЦП и DMA
ADC_HandleTypeDef hadc1;     
DMA_HandleTypeDef hdma_adc1;  

// volatile struct
ADC_VoltageBuffers_t adc_vol_buff;

PowerManager power_manager;



void MX_DMA_Init() {

    __HAL_RCC_DMA2_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}


void MX_ADC1_Init() {

    ADC_ChannelConfTypeDef sConfig = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
    hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_TRGO;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 4;
    hadc1.Init.DMAContinuousRequests = ENABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(&hadc1) != HAL_OK){ 
        Error_Handler();
    }


    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;

    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }


    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 2;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }


    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = 3;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    sConfig.Channel = ADC_CHANNEL_7;
    sConfig.Rank = 4;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adc_vol_buff, 4) != HAL_OK) {
        Error_Handler();
    }
}


// Callback вызывается когда DMA завершил передачу 4 значений ADC
// Вызывается каждые 100 мс (частота TIM2 TRGO)
extern "C" void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        power_manager.set_flag(true);
        
    }
}