#ifndef UNITREEMOTOR_H
#define UNITREEMOTOR_H

//motortype for compatible





struct MotorCmd{
    uint16_t head = 0xFEEE;
    uint8_t  mod;
    uint16_t T;
    uint16_t W;
    uint32_t pos;
    uint16_t kp;
    uint16_t kd;
    uint16_t crc16;
    MotorCmd(uint8_t mod, uint16_t T, uint16_t W, uint32_t pos, uint16_t kp, uint16_t kd):mod(mod), T(T), W(W), pos(pos), kp(kp), kd(kd){}
    bool crc_verify(){
        // 计算MotorCmd前0-119位（二进制，等于前15字节）CRC16_CCITT，并校验
        const uint8_t* data = reinterpret_cast<const uint8_t*>(&(*this));
        uint16_t crc = 0xFFFF;
        for (int i = 0; i < 15; ++i) {
            crc ^= (uint16_t)data[i] << 8;
            for (int j = 0; j < 8; ++j) {
                if (crc & 0x8000)
                    crc = (crc << 1) ^ 0x1021;
                else
                    crc <<= 1;
            }
        }
        // crc16成员偏移量15，校验
        return crc == this->crc16;
    }
}

struct MotorData{
    uint16_t head = 0xFDEE;
    uint8_t mod;
    uint16_t T;
    uint16_t W;
    uint32_t pos;
    uint8_t temp;
    uint16_t reserve;
    uint16_t crc16;
    MotorData(){}
    bool crc_verify(){
        // 计算MotorCmd前0-119位（二进制，等于前15字节）CRC16_CCITT，并校验
        const uint8_t* data = reinterpret_cast<const uint8_t*>(&(*this));
        uint16_t crc = 0xFFFF;
        for (int i = 0; i < 15; ++i) {
            crc ^= (uint16_t)data[i] << 8;
            for (int j = 0; j < 8; ++j) {
                if (crc & 0x8000)
                    crc = (crc << 1) ^ 0x1021;
                else
                    crc <<= 1;
            }
        }
        // crc16成员偏移量15，校验
        return crc == this->crc16;
    }


}


#endif