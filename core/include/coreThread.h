#ifndef CORETHREAD
#define CORETHREAD

#include "unitreeMotor.h"
#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

#define RUN_IN_THREAD

// motorControl thread abstraction
template <typename T, typename R> class CoreThread {
private:
  int fd_block;
  // send/recive fequency in Hz
  std::atomic<unsigned short> fequency;
  // map of command ID's
  std::string serial_port;

  std::mutex cmd_mutex;
  bool cmd_id_map[15] = {false};
  T current_cmd[15];

  // how many motors are being controlled
  std::atomic<unsigned short> motor_count = 0;
  std::mutex queue_mutex;
  // this queue's data is only used when recall function was not set
  std::queue<std::pair<std::pair<T, R>,
                       std::chrono::time_point<std::chrono::system_clock>>>
      cmd_queue;
  // this function's parameters is poped when data is recived
  std::function<bool(T &, R &)> cmd_callback;

public:
  RUN_IN_THREAD void serialsendrecive();
  std::atomic<bool> thread_active = true;
  CoreThread(std::string serial_port, unsigned short freq,
             std::function<bool(T &, R &)> callback);
  CoreThread(std::string serial_port, unsigned short freq,
             std::function<bool(T &, R &)> callback,
             std::unordered_map<int, MotorCmdGom> initial_cmds);
  ~CoreThread();
  bool setCmd(unsigned short id, T cmd);

  bool setfeq(unsigned short feq) {
    fequency = feq;
    return true;
  }

  bool getData(
      std::pair<std::pair<T, R>,
                std::chrono::time_point<std::chrono::system_clock>> &result);
  std::thread serial_thread;
};
#endif