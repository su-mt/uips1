#ifndef ADC_H
#define ADC_H

#include "TIMs.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "stdio.h"
#include "main.h"

struct ADC_PINS_BUFF {
    volatile uint16_t main;
    volatile uint16_t reserve;
    volatile uint16_t out1;
    volatile uint16_t out2;
};

// Глобальные переменные АЦП и DMA
extern struct ADC_PINS_BUFF buff;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void MX_ADC1_Init(void);

void MX_DMA_Init(void);





#ifdef __cplusplus
}
#endif

#endif // ADC_H

