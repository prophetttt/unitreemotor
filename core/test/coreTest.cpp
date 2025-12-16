#include "coreThread.h"
#include "unitreeMotor.h"
#include <iostream>



int main(){
    MotorCmdGom cmd;
    unsigned short id = 1;
    unsigned short mode = 0;
    float T = 0.0f;
    float W = 0.0f;
    float POS = 0.0f;
    float K_P = 0.0f;
    float K_W = 0.0f;
    initMotoCmdGom(&cmd, &id, &mode, &T, &W, &POS, &K_P, &K_W);
    std::unordered_map<int, MotorCmdGom> initial_cmds = {
        {id, cmd}
    };
    CoreThread<MotorCmdGom, MotorDataGom> coreThread("/dev/cu.usbserial-FT53478H",1000,nullptr,initial_cmds);
    int i = 0;
    while (i < 1000){
        auto a = coreThread.getCmd();
        std::cout << "a.second: " << a.second.time_since_epoch().count() << std::endl;
        i++;
    }
}