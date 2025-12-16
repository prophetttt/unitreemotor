#include "coreThread.h"
#include "unitreeMotor.h"
#include <iostream>
#include <unistd.h>
void print_struct_hex(const void *ptr, size_t size)
{
    const unsigned char *bytes = (const unsigned char *)ptr;

    printf("结构体大小: %zu 字节\n", size);
    printf("16进制表示:\n");

    for (size_t i = 0; i < size; i++)
    {
        printf("%02X ", bytes[i]);

        if ((i + 1) % 16 == 0)
        {
            printf("\n");
        }
        else if ((i + 1) % 8 == 0)
        {
            printf(" ");
        }
    }
    printf("\n");
}

int main()
{
    MotorCmdGom cmd;
    unsigned short id = 1;
    unsigned short mode = 0;
    float T = 0.0f;
    float W = 0.0f;
    float POS = 0.0f;
    float K_P = 0.0f;
    float K_W = 0.0f;
    int error;
    int temp;
    int fooforce;
    initMotoCmdGom(&cmd, &id, &mode, &T, &W, &POS, &K_P, &K_W);
    std::unordered_map<int, MotorCmdGom> initial_cmds = {
        {id, cmd}};
    CoreThread<MotorCmdGom, MotorDataGom> coreThread("/dev/cu.usbserial-FT53478H", 1000, nullptr, initial_cmds);
    int i = 0;
    // getchar();
    std::pair<std::pair<MotorCmdGom, MotorDataGom>, std::chrono::time_point<std::chrono::system_clock>> a;
    while (i < 1000)
    {
        if (coreThread.getCmd(a))
        {
            std::cout << "Received command-response pair:" << std::endl;
            std::cout << "a.second: " << a.second.time_since_epoch().count() << std::endl;
            auto cmd = a.first.first;
            print_struct_hex(&cmd, sizeof(MotorCmdGom));
            
            auto data = a.first.second;
            print_struct_hex(&data, sizeof(MotorDataGom));
            depackMotoDataGom(&data, &id, &mode, &T, &W, &POS, &error, &temp, &fooforce);
            std::cout << "Motor ID: " << id << ", MODE: " << mode << ", T: " << T << ", W: " << W << ", POS: " << POS << ", TEMP: " << temp << std::endl;
        }
        else
        {
            std::cout << "No data available in queue." << std::endl;
        }

        i++;
        sleep(1);
    }
}