#include "UART2.h"
#include "TIM3.h"
#include "utils.h"
#include <cstring>
#include <stdlib.h>


extern bool enable;
// Глобальные переменные UART2
uint8_t uart2Buff[5];
uint8_t uart2_rxByte;     
uint8_t uart2_rxCount = 0;

// Функция вычисления контрольной суммы


// Проверка корректности пакета
bool iscorrectAddr(uint8_t* buff) {
    if (uart2Buff[0] == 0x80 \
    && uart2Buff[1] == 0 \
    && uart2Buff[2] == 0x81 \
    && uart2Buff[3] >= 0xF1 && uart2Buff[3] <= 0xF5 \
    && uart2Buff[4] == checksum(buff, 4) ) {
        return true;
    }
    return false;

}

// Команды протокола
void uips_start() {
    enable = true;
    const uint8_t msg[] = "START OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void uips_stop() {
    if (!enable){
        return;
    }
    enable = false;
    const uint8_t msg[] = "STOP \033[32m OK \033[0m\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);

    exit(0);


}

void uips_getCurrent() {
    if (!enable){
        return;
    }
    const uint8_t msg[] = "CURRENT: 0\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void uips_getResistance() {
    if (!enable){
        return;
    }
    const uint8_t msg[] = "RESISTANCE: 0\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void uips_getConsts() {
    if (!enable){
        return;
    }
    const uint8_t msg[] = "CONSTS: OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

void uips_err() {
    if (!enable){
        return;
    }
    const uint8_t msg[] = "Function code ERROR\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg)-1, 100);
}

// Обработчик UART2
void huart2_Handler(bool overflow) {
    if (!overflow) {
        uart2Buff[uart2_rxCount++] = uart2_rxByte;
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        
    } else {
        // пришел пакет
        if (iscorrectAddr(uart2Buff)) {
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
                //uips_err();
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


