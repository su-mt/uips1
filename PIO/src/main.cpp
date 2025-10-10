extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
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
int main() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART6_UART_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();

    for (;;) {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); // пример: мигание LED
        HAL_Delay(500);
    }
}