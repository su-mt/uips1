
#include <cstddef>
#include <cstdint>
#include "utils.h"
#include "Packets.hpp"

uint8_t checksum(const uint8_t* buff, const uint8_t length) {
    uint8_t checksum = 0;   
    for(uint8_t i = 0; i < length -1; i++) {
        checksum ^= buff[i];
    }
    return checksum;
}



