#include "UART6.h"

// Глобальная переменная UART6
UART_HandleTypeDef huart6;  // ОПРЕДЕЛЕНИЕ


void huart6_Handler(bool overflow) {
    return;
}





void MX_USART6_UART_Init()
{
    huart6.Instance = USART6;
    huart6.Init.BaudRate = 115200;
    huart6.Init.WordLength = UART_WORDLENGTH_8B;
    huart6.Init.StopBits = UART_STOPBITS_1;
    huart6.Init.Parity = UART_PARITY_NONE;
    huart6.Init.Mode = UART_MODE_TX_RX;
    huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart6.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart6) != HAL_OK) {
    Error_Handler();
    }


}