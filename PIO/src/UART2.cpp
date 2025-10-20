#include "UART2.h"
#include "ADC.hpp"
#include "TIMs.h"
#include "utils.h"
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include "constants.hpp"
#include "Packets.hpp"

extern bool enabled;

// Глобальные переменные UART2
UART_HandleTypeDef huart2;  
uint8_t uart2Buff[21];
uint8_t uart2_rxByte;     
uint8_t uart2_rxCount = 0;


// Обработчик UART2
void huart2_Handler(bool overflow) {
    if (!overflow) {
        uart2Buff[uart2_rxCount++] = uart2_rxByte;
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        
    } else {
        
        Packet().process(uart2Buff, uart2_rxCount);

        // Очистка буфера и сброс счетчика после обработки пакета
        memset(uart2Buff, 0, 21);
        uart2_rxCount = 0;
    }
    // Перезапуск приема следующего байта
    HAL_UART_Receive_IT(&huart2, &uart2_rxByte, 1);
}

// Команды протокола
void uips_start() {
    enabled = true;
    const uint8_t msg[] = "START OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void uips_stop() {
    if (!enabled){
        return;
    }
    enabled = false;
    const uint8_t msg[] = "STOP \033[32m OK \033[0m\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);

    exit(0);


}

void uips_getCurrent() {
    if (!enabled){
        return;
    }
    const uint8_t msg[] = "CURRENT: 0\r\n";
    //HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
    HAL_UART_Transmit(&huart2, (const uint8_t*) &adc_vol_buff, sizeof(adc_vol_buff)-1, 100);
}

void uips_getResistance() {
    if (!enabled){
        return;
    }
    const uint8_t msg[] = "RESISTANCE: 0\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void uips_getConsts() {
    if (!enabled){
        return;
    }
    const uint8_t msg[] = "CONSTS: OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void uips_err() {
    if (!enabled){
        return;
    }
    const uint8_t msg[] = "Function code ERROR\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}


void uips_sendConsts() {
    return;
}

void uips_saveCurr() {
    return;
}

void MX_USART2_UART_Init(void) {

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
    Error_Handler();
    }

}