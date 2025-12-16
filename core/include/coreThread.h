#ifndef CORETHREAD
#define CORETHREAD

#include <string>
#include <functional>
#include <thread>
#include <queue>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>

#define RUN_IN_THREAD

// motorControl thread abstraction
template <typename T, typename R>
class CoreThread
{
public:
    int fd_block;
    // send/recive fequency in Hz
    unsigned short fequency;
    // map of command ID's
    std::string serial_port;

    std::mutex cmd_mutex;
    bool cmd_id_map[15] = {false};
    T current_cmd[15];
    // how many motors are being controlled
    std::atomic<unsigned short> motor_count = 0;
    std::mutex queue_mutex;
    std::queue<std::pair<std::pair<T, R>, std::chrono::time_point<std::chrono::system_clock>>> cmd_queue;
    // this function's parameters is poped from queue
    std::function<bool(T &, R &)> cmd_callback;

    RUN_IN_THREAD void serialsendrecive();
    std::atomic<bool> thread_active = true;
    CoreThread(std::string serial_port, unsigned short freq, std::function<bool(T &, R &)> callback);
    ~CoreThread();
    bool setCmd(unsigned short id, T cmd);
    std::pair<std::pair<T, R>, std::chrono::time_point<std::chrono::system_clock>> getCmd();
    std::thread serial_thread;
};
#endif