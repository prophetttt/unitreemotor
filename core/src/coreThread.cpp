#include "unitreeMotor.h"
#include "coreThread.h"


CoreThread<MotorCmdGom, MotorDataGom>::CoreThread(unsigned short freq, std::function<bool(MotorCmdGom&, MotorDataGom&)> callback, unsigned short motor_count)
{
    this->fequency = freq;
    this->cmd_callback = callback;
    this->motor_count = motor_count;
    serial_thread = std::thread(&CoreThread<MotorCmdGom, MotorDataGom>::serialsendrecive, this);
}

void CoreThread<MotorCmdGom, MotorDataGom>::serial_thread(){
    while(thread_active){
        auto start = std::chrono::system_clock::now();

        //send and receive logic here

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000.0/fequency) - (int)(elapsed_seconds.count()*1000)));
    }
}