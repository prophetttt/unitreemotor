#include "coreThread.h"
#include "unitreeMotor.h"



int main(){
    CoreThread<MotorCmdGom, MotorDataGom> coreThread("/dev/cu.usbserial-FT53478H",1000,nullptr,1);
    getchar();
}