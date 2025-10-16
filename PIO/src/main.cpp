
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "UART2.h"
#include "UART1.h"
#include "UART6.h"
#include "TIM3.h"
#include "stm32f411xe.h"

extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx_hal_tim.h"
    #include "stm32f4xx_hal_gpio.h"
    #include "stm32f4xx_hal_uart.h"
    #include "stm32f4xx_hal_usart.h"
}

// _main.c
extern "C" {
    ADC_HandleTypeDef hadc1;

    void SystemClock_Config(void);
    void MX_GPIO_Init(void);
    void MX_ADC1_Init(void);
    void MX_USART6_UART_Init(void);
    void MX_USART1_UART_Init(void);
}


bool enabled = false;

// Callback при приеме данных по UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);


int main() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART6_UART_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_TIM3_Init();

    HAL_Delay(500);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    const uint8_t msg[] = "Wait for start command\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg), 100);

    HAL_UART_Receive_IT(&huart2, &uart2_rxByte, 1);

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