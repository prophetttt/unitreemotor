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

    // unsigned short depack_id;
    // unsigned short depack_mode;
    // float depack_T;
    // float depack_W;
    // float depack_POS;
    // int depack_temp;
    
    // if(depackMotoDataGom(&data, &depack_id, &depack_mode, &depack_T, &depack_W, &depack_POS, &depack_temp, 0, 0)){
    //     printf("MotorDataGom 解包成功！\n");
    //     print_struct_hex(&data, sizeof(data));
    // } else {
    //     printf("MotorDataGom 解包失败！\n");
    // }

    return 0;

}