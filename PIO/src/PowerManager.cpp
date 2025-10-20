#include "PowerManager.hpp"
#include <cstring>

// Конструктор
PowerManager::PowerManager(float threshold) 
    : flag(false),
      current_source(PowerSource::NONE),
      pending_source(PowerSource::NONE),
      switch_state(SwitchState::IDLE),
      violation_count(0),
      voltage_threshold(threshold),
      main_voltage(0.0f),
      reserve_voltage(0.0f),
      main_voltage_mv(0),
      reserve_voltage_mv(0) {
    
    init_dwt();                      
    begin_switch(PowerSource::NONE);  
}

// Инициализация DWT для точного таймера
void PowerManager::init_dwt() {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  
    DWT->CYCCNT = 0;                                  
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;             
}

// Получить время в микросекундах
uint32_t PowerManager::micros() {
    return DWT->CYCCNT / (SystemCoreClock / 1000000);
}

// Начало переключения источника (break-before-make)
void PowerManager::begin_switch(PowerSource target) {
    if (switch_state != SwitchState::IDLE) {
        return;  
    }
    
    pending_source = target;
    switch_state = SwitchState::WAIT_DEAD_TIME;
    
    // break-before-make: сначала отключаем все
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
    
    switch_start_time = micros();
    
    // ОТЛАДКА: начало переключения
    extern UART_HandleTypeDef huart2;
    char debug[50];
    const char* target_name = (target == PowerSource::MAIN) ? "MAIN" :
                              (target == PowerSource::RESERVE) ? "RESERVE" : "NONE";
    snprintf(debug, sizeof(debug), "[BEGIN] Switching to %s...\r\n", target_name);
    HAL_UART_Transmit(&huart2, (uint8_t*)debug, strlen(debug), 100);
}

// Завершение переключения после dead time
void PowerManager::complete_switch() {
    switch (pending_source) {
        case PowerSource::MAIN:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
            break;
            
        case PowerSource::RESERVE:
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
            break;
            
        case PowerSource::NONE:
            break;
    }
    
    current_source = pending_source;
    switch_state = SwitchState::IDLE;
    violation_count = 0;
    
    // ОТЛАДКА: вывод статуса переключения
    extern UART_HandleTypeDef huart2;
    char debug[60];
    const char* source_name = (current_source == PowerSource::MAIN) ? "MAIN" :
                              (current_source == PowerSource::RESERVE) ? "RESERVE" : "NONE";
    snprintf(debug, sizeof(debug), "[SWITCH] -> %s (%.1fV/%.1fV)\r\n", 
             source_name, main_voltage, reserve_voltage);
    HAL_UART_Transmit(&huart2, (uint8_t*)debug, strlen(debug), 100);
}

// Обновление состояния - проверка dead time
void PowerManager::update() {
    if (switch_state == SwitchState::WAIT_DEAD_TIME) {
        uint32_t elapsed = micros() - switch_start_time;
        
        if (elapsed >= SWITCH_DELAY_US) {
            complete_switch();
        }
    }
}

// Проверка напряжения и переключение источников
void PowerManager::check_and_switch(const ADC_VoltageBuffers_t& raw_buffer) {
    // ОТЛАДКА: вход в функцию
    extern UART_HandleTypeDef huart2;
    static uint8_t entry_counter = 0;
    if (++entry_counter >= 10) {
        entry_counter = 0;
        const char* entry_msg = "[DEBUG] check_and_switch() called\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)entry_msg, strlen(entry_msg), 100);
    }
    
    if (switch_state != SwitchState::IDLE) {
        static uint8_t skip_counter = 0;
        if (++skip_counter >= 10) {
            skip_counter = 0;
            const char* skip_msg = "[DEBUG] Skipped: switching in progress\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)skip_msg, strlen(skip_msg), 100);
        }
        return;
    }
    
    ADC_VoltageBuffers voltages(raw_buffer);
    
    main_voltage = voltages.get_main();
    reserve_voltage = voltages.get_reserve();
    main_voltage_mv = voltages.get_main_mv();
    reserve_voltage_mv = voltages.get_reserve_mv();
    
    bool main_ok = main_voltage >= voltage_threshold;
    bool reserve_ok = reserve_voltage >= voltage_threshold;
    
    // ОТЛАДКА: вывод текущего состояния
    static uint8_t debug_counter = 0;
    if (++debug_counter >= 10) {  // Каждый 10-й вызов (раз в секунду при 100мс)
        debug_counter = 0;
        char debug[80];
        const char* src_name = (current_source == PowerSource::MAIN) ? "MAIN" :
                               (current_source == PowerSource::RESERVE) ? "RES" : "NONE";
        snprintf(debug, sizeof(debug), "[CHECK] Src:%s M:%.1fV(%d) R:%.1fV(%d) Cnt:%d Thr:%.1f\r\n", 
                 src_name, main_voltage, main_ok, reserve_voltage, reserve_ok, 
                 violation_count, voltage_threshold);
        HAL_UART_Transmit(&huart2, (uint8_t*)debug, strlen(debug), 100);
    }
    
    // Если оба источника недоступны
    if (!main_ok && !reserve_ok) {
        violation_count++;
        if (violation_count >= VIOLATION_THRESHOLD) {
            begin_switch(PowerSource::NONE);
        }
        return;
    }
    
    switch (current_source) {
        case PowerSource::MAIN:
            if (!main_ok) {
                violation_count++;
                if (violation_count >= VIOLATION_THRESHOLD && reserve_ok) {
                    begin_switch(PowerSource::RESERVE);
                }
            } else {
                violation_count = 0;
            }
            break;
            
        case PowerSource::RESERVE:
            if (main_ok) {
                violation_count++;
                if (violation_count >= VIOLATION_THRESHOLD) {
                    begin_switch(PowerSource::MAIN);
                }
            } else if (!reserve_ok) {
                violation_count++;
                if (violation_count >= VIOLATION_THRESHOLD) {
                    begin_switch(PowerSource::NONE);
                }
            } else {
                violation_count = 0;
            }
            break;
            
        case PowerSource::NONE:
            if (main_ok) {
                violation_count++;
                if (violation_count >= VIOLATION_THRESHOLD) {
                    begin_switch(PowerSource::MAIN);
                }
            } else if (reserve_ok) {
                violation_count++;
                if (violation_count >= VIOLATION_THRESHOLD) {
                    begin_switch(PowerSource::RESERVE);
                }
            } 
            break;
    }
}

// Проверка и очистка флага
bool PowerManager::check_and_clear_flag() {
    if (flag) {
        flag = false;
        return true;
    }
    return false;
}

// Установка флага
void PowerManager::set_flag(bool f) { 
    flag = f; 
}

// Геттеры
PowerSource PowerManager::get_current_source() const { 
    return current_source; 
}

uint8_t PowerManager::get_violation_count() const { 
    return violation_count; 
}

float PowerManager::get_threshold() const { 
    return voltage_threshold; 
}

bool PowerManager::get_flag() const { 
    return flag; 
}

bool PowerManager::is_switching() const { 
    return switch_state != SwitchState::IDLE; 
}

bool PowerManager::is_main_active() const { 
    return current_source == PowerSource::MAIN; 
}

bool PowerManager::is_reserve_active() const { 
    return current_source == PowerSource::RESERVE; 
}

bool PowerManager::is_none_mode() const { 
    return current_source == PowerSource::NONE; 
}

uint16_t PowerManager::get_curr_vol_mv() const { 
    if (current_source == PowerSource::MAIN) {
        return main_voltage_mv;
    } else if (current_source == PowerSource::RESERVE) {
        return reserve_voltage_mv;
    } else {
        return 0;
    }
}

float PowerManager::get_curr_vol() const { 
    if (current_source == PowerSource::MAIN) {
        return main_voltage;
    } else if (current_source == PowerSource::RESERVE) {
        return reserve_voltage;
    } else {
        return 0.0f;
    }
}
