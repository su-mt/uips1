#include "UART2.h"
#include "ADC.hpp"
#include "TIMs.h"
#include "utils.h"
#include <cstdint>
#include <cstring>
#include <stdlib.h>
#include "constants.hpp"
#include "Packets.hpp"
#include "PowerManager.hpp"

extern bool enabled;

// Глобальные переменные UART2
UART_HandleTypeDef huart2;  
uint8_t uart2Buff[21];
uint8_t uart2_rxByte;     
uint8_t uart2_rxCount = 0;
extern PowerManager power_manager;


// Обработчик UART2
void huart2_Handler(bool overflow) {
    if (!overflow) {
        uart2Buff[uart2_rxCount++] = uart2_rxByte;

        if (uart2_rxCount >= sizeof(uart2Buff)) {
            uart2_rxCount = 0;
        }
        

        if (uart2_rxCount == 1) {
            HAL_TIM_Base_Start_IT(&htim3);  
        }

        __HAL_TIM_SET_COUNTER(&htim3, 0);
        
    } else {
        HAL_TIM_Base_Stop_IT(&htim3);


        Packet::process(uart2Buff, uart2_rxCount);

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
    if (!enabled) return;
    
    // Закомментировано для отладки
    // buf_CurrentResponse buff = get_buf_CurrResp(power_manager.get_curr_vol_mv(), \
    // power_manager.is_main_active(), \
    // power_manager.is_reserve_active());

    // ОТЛАДКА: расширенный вывод
    char msg[80];
    snprintf(msg, sizeof(msg), "[GET] V:%.2f M:%d R:%d Sw:%d Cnt:%d\r\n", 
             power_manager.get_curr_vol(),
             (int)power_manager.is_main_active(),
             (int)power_manager.is_reserve_active(),
             (int)power_manager.is_switching(),
             power_manager.get_violation_count());

    HAL_UART_Transmit(&huart2, (const uint8_t*)msg, strlen(msg), 100);

    // 
}

void uips_getResistance() {
    if (!enabled){
        return;
    }
    const uint8_t msg[] = "RESISTANCE: 0\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg), 100);
}

void uips_getConsts() {
    if (!enabled){
        return;
    }
    const uint8_t msg[] = "CONSTS: OK\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg), 100);
}

void uips_err() {
    if (!enabled){
        return;
    }
    const uint8_t msg[] = "Function code ERROR\r\n";
    HAL_UART_Transmit(&huart2, msg, sizeof(msg), 100);
}


void uips_saveConsts() {
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

// На F3 - 0x80, 0x00, 0x01, 0x1X, 0xYZ, 0x5x (X,YZ– двоично-десятичное число; 0x5x- младшие два бита отображают ОСНОВНОЙ и РЕЗЕРВНЫЙ).
buf_CurrentResponse get_buf_CurrResp (uint16_t vol_mv, bool main_status, bool reserve_status ) {
    buf_CurrentResponse resp = {
        .address_1 = 0x80,
        .address_2 = 0x00,
        .address_3 = 0x01  
    };

    uint16_t vol_dv = (vol_mv+50)/100;

    if (vol_dv >999) {
        vol_dv =999;
    }

    uint8_t hundreds = (vol_dv / 100) % 10;
    uint8_t tens = (vol_dv / 10) % 10;
    uint8_t units = vol_dv % 10;

    resp.voltage_high = 0x10 | hundreds;
    resp.voltage_low = (tens << 4) | units;

    resp.status = 0x50 | (reserve_status << 1) | main_status;

    return resp;
}