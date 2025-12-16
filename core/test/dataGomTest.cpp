#include "unitreeMotorGom.h"
#include <cstdio>
#include <iostream>

struct MotorCmdGom cmd;
struct MotorDataGom data;

void print_struct_hex(const void *ptr, size_t size) {
    const unsigned char *bytes = (const unsigned char *)ptr;
    
    printf("结构体大小: %zu 字节\n", size);
    printf("16进制表示:\n");
    
    for (size_t i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
        
        if ((i + 1) % 16 == 0) {
            printf("\n");
        } else if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}

int main(){
    unsigned short id = 1;
    unsigned short mode = 0;
    float T = 0.0f;
    float W = 0.0f;
    float POS = 0.0f;
    float K_P = 0.0f;
    float K_W = 0.0f;

    if(initMotoCmdGom(&cmd, &id, &mode, &T, &W, &POS, &K_P, &K_W)){
        printf("MotorCmdGom 初始化成功！\n");
        print_struct_hex(&cmd, sizeof(cmd));
    } else {
        printf("MotorCmdGom 初始化失败！\n");
    }

    // 用memcpy将指定hex数据写入MotorDataGom结构体
    uint8_t raw_data[] = {0xfd, 0xee, 0x01, 0x00, 0x00, 0x0a, 0x00, 0x05, 0x75, 0x00, 0x00, 0x17, 0x40, 0x02, 0x98, 0xd8};
    memcpy(&data, raw_data, sizeof(data));

    unsigned short depack_id;
    unsigned short depack_mode;
    float depack_T;
    float depack_W;
    float depack_POS;
    int depack_temp;
    int depack_error;
    int depack_fooforce;
    
    if(depackMotoDataGom(&data, &depack_id, &depack_mode, &depack_T, &depack_W, &depack_POS, &depack_temp, &depack_error, &depack_fooforce)){
        printf("MotorDataGom 解包成功！\n");
        printf("ID: %u\n", depack_id);
        printf("MODE: %u\n", depack_mode);
        printf("T: %.4f\n", depack_T);
        printf("W: %.4f\n", depack_W);
        printf("POS: %.4f\n", depack_POS);
        printf("TEMP: %d\n", depack_temp);
        
    } else {
        printf("MotorDataGom 解包失败！\n");
    }

    return 0;

}