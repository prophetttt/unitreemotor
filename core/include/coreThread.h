#ifndef CORETHREAD
#define CORETHREAD

#include <string>
#include <functional>
#include <thread>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <chrono>

#define RUN_IN_THREAD

// motorControl thread abstraction
template <typename T, typename R>
class CoreThread
{
    //send/recive fequency in Hz
    unsigned short fequency;
    //map of command ID's

    bool cmd_id_map[15];
    //how many motors are being controlled
    unsigned short motor_count;
    std::mutex cmd_mutex;
    std::queue<std::pair<std::pair<T, R>, std::chrono::system_clock> cmd_queue;
    std::function<bool(T&, R&)> cmd_callback;

    RUN_IN_THREAD void serialsendrecive();
    bool thread_active = true;
    std::thread serial_thread;
    CoreThread() {}
    ~CoreThread()
    {
        thread_active = false;
        serial_thread.join();
    }
};
#endif