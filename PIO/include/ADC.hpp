#ifndef ADC_H
#define ADC_H

#include "TIMs.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "stdio.h"
#include "main.h"

#ifdef __cplusplus
}
#endif



struct ADC_VoltageBuffers_t {
    volatile uint16_t main;
    volatile uint16_t reserve;
    volatile uint16_t out1;
    volatile uint16_t out2;
};

class ADC_VoltageBuffers {
private:
    float main;
    float reserve;
    float out1;
    float out2;

    float adc_to_voltage(uint16_t adc_value) const {
        constexpr float V_REF = 3.3f;
        constexpr uint16_t ADC_MAX = 4095;  
        return (adc_value * V_REF) / ADC_MAX;
    }

public:

    ADC_VoltageBuffers(const ADC_VoltageBuffers_t& raw_data) {

        main = adc_to_voltage(raw_data.main);
        reserve = adc_to_voltage(raw_data.reserve);
        out1 = adc_to_voltage(raw_data.out1);
        out2 = adc_to_voltage(raw_data.out2);
    }

    float get_main() const { return main; }
    float get_reserve() const { return reserve; }
    float get_out1() const { return out1; }
    float get_out2() const { return out2; }

    uint16_t get_main_mv() const { return (uint16_t)(main * 1000); }
    uint16_t get_reserve_mv() const { return (uint16_t)(reserve * 1000); }
    uint16_t get_out1_mv() const { return (uint16_t)(out1 * 1000); }
    uint16_t get_out2_mv() const { return (uint16_t)(out2 * 1000); }
};


// Глобальные переменные АЦП и DMA
extern struct ADC_VoltageBuffers_t adc_vol_buff;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void MX_ADC1_Init(void);

void MX_DMA_Init(void);







#endif // ADC_H

