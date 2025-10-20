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
    volatile bool flag;

    PowerSource current_source;
    PowerSource pending_source;    
    SwitchState switch_state;
    
    uint8_t violation_count;
    uint32_t switch_start_time;    
    
    const float voltage_threshold;
    static constexpr uint8_t VIOLATION_THRESHOLD = 3;
    static constexpr uint32_t SWITCH_DELAY_US = 100; 
    
    void init_dwt();
    uint32_t micros();
    void begin_switch(PowerSource target);
    void complete_switch();

public:
    float main_voltage; 
    float reserve_voltage; 
    uint16_t main_voltage_mv; 
    uint16_t reserve_voltage_mv; 
    
    PowerManager(float threshold = voltage_lower_bound);
    
    void update();
    void check_and_switch(const ADC_VoltageBuffers_t& raw_buffer);
    
    bool check_and_clear_flag();
    void set_flag(bool f);

    PowerSource get_current_source() const;
    uint8_t get_violation_count() const;
    float get_threshold() const;
    bool get_flag() const;
    bool is_switching() const;

    bool is_main_active() const;
    bool is_reserve_active() const;
    bool is_none_mode() const;

    uint16_t get_curr_vol_mv() const;
    float get_curr_vol() const;
};

#endif // POWER_MANAGER_HPP
