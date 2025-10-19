#ifndef CONSTANTS_H
#define CONSTANTS_H


#include <cstddef>
#include <cstdint>
#include <stdlib.h>


constexpr float voltage_lower_bound = 5;


// Константы датчика (13 байт)

typedef struct SystemConstants_t {
    uint8_t  address;        // 0x80 ('A')
    uint8_t  reserved1;      // 0x00
    uint8_t  reserved2;      // 0x01
    
    // Коэффициент индукции (Mui) - 3 байта
    uint8_t  mui_sign_e15;   // Знак (бит 7) + E-15 (биты 6-0)
    uint8_t  mui_e8;         // E-8
    uint8_t  mui_b8;         // B-8 (дробная часть)
    
    // Постоянная составляющая (Ii) - 3 байта
    uint8_t  ii_sign_e15;    // Знак + E-15
    uint8_t  ii_e8;          // E-8
    uint8_t  ii_unused;      // ----- (нет дробной части)
    
    // Коэффициент вихревых токов (Mbi) - 3 байта
    uint8_t  mbi_sign_e15;   // Знак + E-15
    uint8_t  mbi_e8;         // E-8
    uint8_t  mbi_b8;         // B-8
    
    uint8_t  group;          // Группа: 'Б', 'Г' или 'Ш'
    uint8_t  checksum;       // 0x00
} SystemConstants_t; __attribute__((packed));

class SystemConstants {
private:

    float mui;  
    float ii;    
    float mbi;   
    char group;  
    uint8_t checksum;


    float decode_3byte_float(uint8_t sign_e15, uint8_t e8, uint8_t b8) {

        bool is_negative = (sign_e15 & 0x80) != 0;
        uint8_t e15 = sign_e15 & 0x7F;
        uint32_t integer_part = (uint32_t)e15 * 256 + e8;
        float fractional_part = b8 / 256.0f;


        float value = integer_part + fractional_part;
        return is_negative ? -value : value;
    }

public:
    SystemConstants(const SystemConstants_t& data) {
        mui = decode_3byte_float(data.mui_sign_e15, data.mui_e8, data.mui_b8);
        ii = decode_3byte_float(data.ii_sign_e15, data.ii_e8, 0);
        mbi = decode_3byte_float(data.mbi_sign_e15, data.mbi_e8, data.mbi_b8);
        
        group = (char)data.group;
        checksum = data.checksum;
    }

    float get_mui() const { return mui; }
    float get_ii() const { return ii; }
    float get_mbi() const { return mbi; }
    char get_group() const { return group; }
    uint8_t get_checksum() const { return checksum; }
};


#endif // CONSTANTS_H