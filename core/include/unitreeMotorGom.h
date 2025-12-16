#ifndef UNITREEMOTORGOM_H
#define UNITREEMOTORGOM_H

#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
#include <stddef.h>

#include <stdint.h>

#pragma pack(push, 1)
struct MotorCmdGom
{
    uint16_t head;
    uint8_t mod;
    uint16_t t;
    uint16_t w;
    uint32_t pos;
    uint16_t kp;
    uint16_t kd;
    uint16_t crc16;
};

struct MotorDataGom
{
    uint16_t head;
    uint8_t mod;
    uint16_t t;
    uint16_t w;
    uint32_t pos;
    uint8_t temp;
    uint16_t reserve;
    uint16_t crc16;
};
#pragma pack(pop) 
uint16_t crc_ccitt(uint16_t crc, uint8_t const *buffer, size_t len);
bool generate_crc16_cmd(struct MotorCmdGom *cmd);
bool verify_crc16_data(struct MotorDataGom *data);
bool initMotoCmdGom(struct MotorCmdGom * cmd,const unsigned short *ID, const unsigned short *MODE, const float* T,const float* W,const float* POS,const float* K_P,const float* K_W);
bool depackMotoDataGom(const struct MotorDataGom *, unsigned short*ID, unsigned short *MODE,  float* T, float* W, float* POS, int* TEMP, int* ERROR, int* FOOFORCE);

#ifdef __cplusplus
}
#endif

#endif