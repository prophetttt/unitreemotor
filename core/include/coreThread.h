#ifndef CORETHREAD
#define CORETHREAD
#include <string>
#include <functional>
#include <thread>

#define RUN_IN_THREAD

class CoreThread
{
public:
    CoreThread(std::string serial, std::function<void()> callback = nullptr);
    ~CoreThread();
    
    bool changeMotorCmd(const motor_cmd&);
    bool getMotorData(motor_data&);

    std::function<void(MotorCmd&, MotorData&)> callbackaction = nullptr;


private:
    // use wxQueueEvent(GetEventHandler(),new wxCommandEvent(MY_CUSTOM_EVENT, GetId()) to  update GUI from thread
    RUN_IN_THREAD void serialReadandSend();
    std::string serial_port;
    MotorCmd motor_cmd;
    MotorData motor_data;
    unsigned int seq = 0;
    mutable std::mutex read_mutex_;
    mutable std::mutex write_mutex_;
    std::thread core_thread;
    bool thread_alive = true;
};






#endif