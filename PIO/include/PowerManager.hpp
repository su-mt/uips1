#ifndef POWER_MANAGER_HPP
#define POWER_MANAGER_HPP

#include "ADC.hpp"
#include "constants.hpp"

extern "C" {
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
}


enum class PowerSource {
    MAIN,      
    RESERVE,   
    NONE       
};

enum class SwitchState {
    IDLE,              
    WAIT_DEAD_TIME,    
    COMPLETED          
};

class PowerManager {
private:
    volatile bool flag = false;

    PowerSource current_source;
    PowerSource pending_source;    
    SwitchState switch_state;
    
    uint8_t violation_count;
    uint32_t switch_start_time;    
    
    const float voltage_threshold;
    static constexpr uint8_t VIOLATION_THRESHOLD = 3;
    static constexpr uint32_t SWITCH_DELAY_US = 100; 
    
    
    void init_dwt() {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;  
        DWT->CYCCNT = 0;                                  
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;             
    }
    
    uint32_t micros() {
        return DWT->CYCCNT / (SystemCoreClock / 1000000);
    }
    
    void begin_switch(PowerSource target) {
        if (switch_state != SwitchState::IDLE) {
            return;  
        }
        
        pending_source = target;
        switch_state = SwitchState::WAIT_DEAD_TIME;
        
        // break-before-make
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        
        switch_start_time = micros();
    }
    
    void complete_switch() {
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
    }

public:

    PowerManager(float threshold = voltage_lower_bound) 
        : current_source(PowerSource::NONE),
          pending_source(PowerSource::NONE),
          switch_state(SwitchState::IDLE),
          violation_count(0),
          voltage_threshold(threshold) {
        
        init_dwt();                      
        begin_switch(PowerSource::MAIN);  
    }
    
    void update() {
        if (switch_state == SwitchState::WAIT_DEAD_TIME) {
            uint32_t elapsed = micros() - switch_start_time;
            
            if (elapsed >= SWITCH_DELAY_US) {
                complete_switch();
            }
        }
    }
    

    void check_and_switch(const ADC_VoltageBuffers_t& raw_buffer) {
        if (switch_state != SwitchState::IDLE) {
            return;
        }
        
        ADC_VoltageBuffers voltages(raw_buffer);
        
        float main_voltage = voltages.get_main();
        float reserve_voltage = voltages.get_reserve();
        
        bool main_ok = main_voltage >= voltage_threshold;
        bool reserve_ok = reserve_voltage >= voltage_threshold;
        
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
                } else {
                    violation_count = 0;
                }
                break;
        }
    }
    
    bool check_and_clear_flag() {
        if (flag) {
            flag = false;
            return true;
        }
        return false;
    }
    
    void set_flag(bool f) { flag = f; }

    PowerSource get_current_source() const { return current_source; }
    uint8_t get_violation_count() const { return violation_count; }
    float get_threshold() const { return voltage_threshold; }
    bool get_flag() const { return flag; }
    bool is_switching() const { return switch_state != SwitchState::IDLE; }

    bool is_main_active() const { return current_source == PowerSource::MAIN; }
    bool is_reserve_active() const { return current_source == PowerSource::RESERVE; }
    bool is_emergency_mode() const { return current_source == PowerSource::NONE; }
};

#endif // POWER_MANAGER_HPP
