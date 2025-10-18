
#include <cstddef>
#include <cstdint>
#include "utils.h"

uint8_t checksum(uint8_t* buff, char length) {
    uint8_t checksum = 0;
    for(uint8_t i = 0; i < length; i++) {
        checksum ^= buff[i];
    }
    return checksum;
}





