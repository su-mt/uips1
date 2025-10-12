
#include <cstdint>
#include <cstring>
extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx_hal_gpio.h"
    #include "stm32f4xx_hal_uart.h"
}


extern "C" {

    
    ADC_HandleTypeDef hadc1;

    UART_HandleTypeDef huart1;
    UART_HandleTypeDef huart2;
    UART_HandleTypeDef huart6; // вметсо uart3


    TIM_HandleTypeDef htim3; // таймер 3

    void MX_TIM3_Init ();

    void SystemClock_Config(void);
    void MX_GPIO_Init(void);
    void MX_ADC1_Init(void);
    void MX_USART6_UART_Init(void);
    void MX_USART1_UART_Init(void);
    void MX_USART2_UART_Init(void);
    
}


uint8_t uart2Buff[5];
uint8_t uart2_rx_byte;     
uint8_t uart2_rx_count = 0;
int flag = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2){
        uart2Buff[uart2_rx_count++] = uart2_rx_byte;


    }
    
}


void huart2_Handler ();


HAL_StatusTypeDef status = HAL_UART_Receive_IT(&huart2, uart2Buff, sizeof(uart2Buff));

int main() {
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_USART6_UART_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_TIM3_Init();

    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);


}




void MX_TIM3_Init () {
    uint32_t sysclk = HAL_RCC_GetSysClockFreq();  // Системная частота ядра
    uint32_t hclk   = HAL_RCC_GetHCLKFreq();      // Частота шины AHB
    uint32_t pclk1  = HAL_RCC_GetPCLK1Freq();     // Частота шины APB1
    uint32_t pclk2  = HAL_RCC_GetPCLK2Freq();  
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = sysclk / 10000 - 1 ;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 40-1;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;


    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        Error_Handler();
    }


    HAL_TIM_Base_Start_IT(&htim3);


}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM3)
    {
        // Этот код выполняется при каждом переполнении
    }
}