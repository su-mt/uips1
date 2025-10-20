#ifndef PACKETS_HPP
#define PACKETS_HPP



#include <cstdint>
#include <sys/types.h>
#include "UART2.h"
#include "sys/stat.h"
#include "utils.h"
#include "constants.hpp"





namespace PacketTypes {
    namespace  Len {
        constexpr uint8_t CONTROL = 5;
        constexpr uint8_t RESPONSE = 6;
        constexpr uint8_t CONSTANTS = 13;
        constexpr uint8_t BROADCAST = 20;
    }
    namespace Address {
        constexpr uint8_t UNICAST = 0x80;
        constexpr uint8_t BROADCAST = 0x96;
    }

    namespace FunctionCode {
        constexpr uint8_t START = 0xF1;
        constexpr uint8_t STOP = 0xF2;
        constexpr uint8_t GET_CURRENT = 0xF3;
        constexpr uint8_t GET_RESISTANCE = 0xF4;
        constexpr uint8_t GET_CONSTS = 0xF5;
    }

}



class __Packet {
protected:
    const uint8_t* buff;
    const uint8_t len;

public:
    void (*process_func) () ;
    __Packet (const uint8_t* b, uint8_t l) : buff(b), len (l) {}

    virtual ~__Packet() = default;

    virtual bool is_valid() const = 0;

    bool operator==(const uint8_t* buff) const {
        return is_valid();
    }
};


class ControlPacket: public __Packet {
private:
    uint8_t FunCode;

    static constexpr uint8_t LEN = PacketTypes::Len::CONTROL;
    static constexpr uint8_t ADDR = PacketTypes::Address::UNICAST;
    static constexpr uint8_t BYTE_1 = 0x00;
    static constexpr uint8_t BYTE_2 = 0x81;
public:
    
    ControlPacket(const uint8_t* buff, uint8_t len) : __Packet(buff, len) {
        FunCode = get_function_code();
        switch (FunCode) {
            case PacketTypes::FunctionCode::START:
                process_func = &uips_start;
            case PacketTypes::FunctionCode::STOP:
                process_func = &uips_stop;
            case PacketTypes::FunctionCode::GET_CONSTS:
                process_func = &uips_getConsts;
            case PacketTypes::FunctionCode::GET_CURRENT:
                process_func = &uips_getCurrent;
            case PacketTypes::FunctionCode::GET_RESISTANCE:
                process_func = &uips_getResistance;

        }
    }

    bool is_valid() const override {
            if (len != LEN) return false;
            
            return buff[0] == ADDR &&
                buff[1] == BYTE_1 &&
                buff[2] == BYTE_2 &&
                buff[3] >= 0xF1 && buff[3] <= 0xF5 &&
                buff[4] == checksum(buff, len-1);
        }
        
        uint8_t get_function_code() const {
            return buff[3];
        }



};


class ConstantsPacket : public __Packet {
private:
    static constexpr uint8_t LEN = PacketTypes::Len::CONSTANTS;
    static constexpr uint8_t ADDR = PacketTypes::Address::UNICAST;
    static constexpr uint8_t BYTE_1 = 0x00;
    static constexpr uint8_t BYTE_2 = 0x01;
    
public:
    ConstantsPacket(const uint8_t* buff, uint8_t len) : __Packet(buff, len) {
        process_func = &uips_saveConsts;
    }
    
    bool is_valid() const override {
        if (len != LEN) return false;
            
        return buff[0] == ADDR &&
            buff[1] == BYTE_1 &&
            buff[2] == BYTE_2 &&
            buff[12] == checksum(buff, len-1);
        }
    
    SystemConstants_t* get_constants() const {
        return (SystemConstants_t*)buff;
    }
};


class BroadcastPacket : public __Packet {
private:
    static constexpr uint8_t LEN = 20;
    static constexpr uint8_t BROADCAST_ADDR = 0x80;
    
public:
    BroadcastPacket(const uint8_t* buff, uint8_t len) : __Packet(buff, len) {
        process_func = &uips_saveCurr;
    }
    
    bool is_valid() const override {
        if (len != LEN) return false;
        
        return buff[0] == BROADCAST_ADDR &&
               buff[19] == checksum(buff, len-1);
    }
};

class Packet {
public:
    enum class Type { CONTROL, CONSTANTS, BROADCAST, UNKNOWN };
    
    static Type identify(const uint8_t* buff, uint8_t len) {
        ControlPacket control(buff, len);
        if (control.is_valid()) return Type::CONTROL;
        
        ConstantsPacket constants(buff, len);
        if (constants.is_valid()) return Type::CONSTANTS;
        
        BroadcastPacket broadcast(buff, len);
        if (broadcast.is_valid()) return Type::BROADCAST;
        
        return Type::UNKNOWN;
    }

    static void process (const uint8_t* buff, const uint8_t len) {
        Type type = identify(buff, len);

        switch (type) {
        case Packet::Type::CONTROL: {
            ControlPacket packet(uart2Buff,uart2_rxCount);
            packet.process_func();
            break;
        }

        case Packet::Type::CONSTANTS: {
            ConstantsPacket packet(uart2Buff,uart2_rxCount);
            packet.process_func();
            break;
        }
        case Packet::Type::BROADCAST: {
            BroadcastPacket packet(uart2Buff,uart2_rxCount);
            packet.process_func();
            break;
        }
        case Packet::Type::UNKNOWN:{
            uips_err();
            break;
        }
        }

    }


};


#endif // PACKETS_HPP