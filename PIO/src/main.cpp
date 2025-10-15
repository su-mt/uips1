

#include <cstddef>
#include <cstdint>
#include <cstring>
extern "C" {
    #include "main.h"
    #include "stm32f4xx_hal.h"
    #include "stm32f4xx_hal_tim.h"
    #include "stm32f4xx_hal_gpio.h"
    #include "stm32f4xx_hal_uart.h"
    #include "stm32f4xx_hal_usart.h"
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

    void huart2_Handler ();


    
}

void uips_stop() {
    const uint8_t msg[] = "STOP OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}
void uips_getCurrent() {
    const uint8_t msg[] = "CURRENT: 0\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}
void uips_getResistance() {
    const uint8_t msg[] = "RESISTANCE: 0\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}
void uips_getConsts() {
    const uint8_t msg[] = "CONSTS: OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}
void uips_err() {
    const uint8_t msg[] = "ERROR\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}




uint8_t uart2Buff[5];
uint8_t uart2_rxByte;     
uint8_t uart2_rxCount = 0;
int flag = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);



uint8_t checksum(uint8_t* buff, char length) {
    uint8_t checksum = 0;
    for(uint8_t i = 0; i < length-1; i++) {
        checksum ^= buff[i];
    }
    return checksum;
}


bool iscorrect(uint8_t* buff) {

    if (uart2Buff[0] == 0x80 \
    and uart2Buff[1] == 0 \
    and uart2Buff[2] == 0x81 \
    and uart2Buff[4] == checksum(buff, 5) \
    and uart2Buff[3] >= 0xF1 and uart2Buff[3] <= 0xF5 ){
        return true;
    }

    return false;
}


void uips_start() {
    const uint8_t msg[] = "START OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void huart2_Handler (bool overflow) {

    if (!overflow) {
        uart2Buff[uart2_rxCount++] = uart2_rxByte;
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        
    } else {
        // пришел пакет
        if (iscorrect(uart2Buff) or !iscorrect(uart2Buff)) {
            switch (uart2Buff[3]) {
            case 0xF1:
                uips_start();
                break;
            case 0xF2:
                uips_stop();
                break;
            case 0xF3:
                uips_getCurrent();
                break;
            case 0xF4:
                uips_getResistance();
                break;
            case 0xF5:
                uips_getConsts();
                break;
            default: 
                uips_err();
                break;
            }
        }
        // Очистка буфера и сброс счетчика после обработки пакета
        memset(uart2Buff, 0, 5);
        uart2_rxCount = 0;
    }
    // Перезапуск приема следующего байта
    HAL_UART_Receive_IT(&huart2, &uart2_rxByte, 1);

}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2){
        huart2_Handler(false);

    }
    
}



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


    const uint8_t msg[] = "Started\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg), 100);
    HAL_UART_Receive_IT(&huart2, &uart2_rxByte, 1) ;

    while(1){
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

    }



}




void MX_TIM3_Init () {
    uint32_t sysclk = HAL_RCC_GetSysClockFreq();  // Системная частота ядра
    uint32_t hclk   = HAL_RCC_GetHCLKFreq();      // Частота шины AHB
    uint32_t pclk1  = HAL_RCC_GetPCLK1Freq();     // Частота шины APB1
    uint32_t pclk2  = HAL_RCC_GetPCLK2Freq();  
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = sysclk / 100000 - 1 ;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    // Period: 100000 / 2873.56 ≈ 34.8 → 35 тиков
    htim3.Init.Period = 35 - 1;  // ~0.348 мс (период приема 4 байт)
    
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;


    if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
        Error_Handler();
    }


    HAL_TIM_Base_Start_IT(&htim3);
}


// вызывается при переполнении - 0.348 мс
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if(htim->Instance == TIM3) {
        const uint8_t msg[] = "overflow\r\n";
        HAL_UART_Transmit(&huart2, msg, sizeof(msg), 100);
        huart2_Handler(true);
    }
}
