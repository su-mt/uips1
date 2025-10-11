


#include <cstdint>
#include <cstring>
extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx_hal_gpio.h"
}


extern "C" {

    
    ADC_HandleTypeDef hadc1;

    UART_HandleTypeDef huart1;
    UART_HandleTypeDef huart2;
    UART_HandleTypeDef huart6;
    void SystemClock_Config(void);
    void MX_GPIO_Init(void);
    void MX_ADC1_Init(void);
    void MX_USART6_UART_Init(void);
    void MX_USART1_UART_Init(void);
    void MX_USART2_UART_Init(void);
    
}


uint8_t rx_byte;
int flag = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart != &huart2){
                        HAL_UART_Receive_IT(&huart2, (uint8_t*)&rx_byte, 1);
        return;
    }
        if (rx_byte == 'q') {
            flag = 1;
        } else {
            flag = 2;
        }

        // Перезапускаем приём следующего байта

    
}
int main() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART6_UART_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    HAL_StatusTypeDef status;
    char hi[] = "bye\r\n";
    uint32_t currTime = HAL_GetTick();
    status = HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

    for (;;) {
        if (HAL_GetTick() - currTime >= 300){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            currTime = HAL_GetTick();
        }
        if (flag){
            if (flag == 1){
                HAL_UART_Transmit_IT(&huart2, (uint8_t*)"bye\r\n", 5);

            } else if (flag==2) {
                HAL_UART_Transmit(&huart2, (uint8_t*)"Incorrect!\r\n", 12, 100);

            }
            flag = 0;
        }
        
    }
}


