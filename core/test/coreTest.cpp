#include "../include/coreThread.h"


int main(){
    CoreThread coreThread("/dev/ttyUSB0", [](){
        // Callback function code here
        
    });
    coreThread.changeMotorCmd();
}