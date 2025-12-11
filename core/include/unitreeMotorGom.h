#ifndef UNITREEMOTORGOM_H
#define UNITREEMOTORGOM_H

//motortype for compatible


bool crc_verify(MotorCmdGom *);

bool initMotoCmdGom(const unsigned short *id, const unsigned short *mode, const float* T,const float* W,const float* Pos,const float* K_P,const float* K_W);
bool depackMotoDataGom(unsigned short*id, unsigned short *mode,  float* T, float* W, float* Pos, float* K_P, float* K_W);


struct MotorCmdGom{
    uint16_t head;
    uint8_t  mod;
    uint16_t T;
    uint16_t W;
    uint32_t pos;
    uint16_t kp;
    uint16_t kd;
    uint16_t crc16;
}

struct MotorDataGom{
    uint16_t head;
    uint8_t mod;
    uint16_t T;
    uint16_t W;
    uint32_t pos;
    uint8_t temp;
    uint16_t reserve;
    uint16_t crc16;
}


#endif