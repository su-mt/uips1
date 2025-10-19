
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "PowerManager.hpp"
#include "UART2.h"
#include "UART1.h"
#include "UART6.h"
#include "TIMs.h"
#include "ADC.hpp"


extern "C" {
    #include "main.h"
    #include "stm32f411xe.h"
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx_hal_tim.h"
    #include "stm32f4xx_hal_gpio.h"
    #include "stm32f4xx_hal_uart.h"
    #include "stm32f4xx_hal_usart.h"
}

// _main.c
extern "C" {
    void MX_GPIO_Init(void);
    void MX_USART6_UART_Init(void);
    void MX_USART1_UART_Init(void);
}


bool enabled = false;

// Callback при приеме данных по UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void init ();

extern PowerManager power_manager;

int main() {
    init();
    HAL_Delay(500);
    


    // wait for consts

    const uint8_t msg[] = "Wait for start command\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg), 100);

    HAL_UART_Receive_IT(&huart2, &uart2_rxByte, 1);

    while(1) {

        // проверка кан
        power_manager.update();  
        if (power_manager.check_and_clear_flag()) {
            power_manager.check_and_switch(adc_vol_buff);
        }
        
    }
}

void init () {
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_USART6_UART_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    MX_TIM3_Init();
    MX_TIM2_Init();

    MX_DMA_Init();
    MX_ADC1_Init();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) { 
    if (huart->Instance == USART2){
        huart2_Handler(false);
    } else if (huart->Instance == USART1) {
        huart1_Handler(false);
    } else if (huart->Instance == USART6) {
        huart6_Handler(false);
    }
}