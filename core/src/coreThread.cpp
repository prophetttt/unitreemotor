#include "coreThread.h"
#include "serial_initalize.h"
#include "unitreeMotor.h"
#include <fcntl.h> // 包含 open, O_RDWR, O_NOCTTY
#include <iostream>
#include <unistd.h> // 包含 read, write, close

template <typename T, typename R>
CoreThread<T, R>::CoreThread(std::string serial_port, unsigned short freq,
                             std::function<bool(T &, R &)> callback) {
  this->fequency = freq;
  this->cmd_callback = callback;
  this->serial_port = serial_port;
  std::cout << "CoreThread started on port: " << serial_port
            << " with frequency: " << freq << " Hz controlling " << motor_count
            << " motors." << std::endl;
  fd_block = open(serial_port.c_str(), O_RDWR | O_NOCTTY);
  if (fd_block == -1) {
    std::cerr << "Error opening serial port: " << std::strerror(errno)
              << std::endl;
    thread_active = false;
    return;
  }
  configure_serial_termios(fd_block);
  serial_thread = std::thread(&CoreThread<T, R>::serialsendrecive, this);
}

template <typename T, typename R>
CoreThread<T, R>::CoreThread(
    std::string serial_port, unsigned short freq,
    std::function<bool(T &, R &)> callback,
    std::unordered_map<int, MotorCmdGom> initial_cmds) {
  this->fequency = freq;
  this->cmd_callback = callback;
  this->serial_port = serial_port;
  this->motor_count = initial_cmds.size();
  if (!initial_cmds.empty()) {
    // 1. 获取指向第一个元素的迭代器
    auto it = initial_cmds.begin();

    for (const auto &it : initial_cmds) {
      // 2. 处理当前元素
      unsigned short id = it.first;
      T cmd = it.second;
      cmd_id_map[id] = true;
      current_cmd[id] = cmd;
    }
  }

  std::cout << "CoreThread started on port: " << serial_port
            << " with frequency: " << freq << " Hz controlling " << motor_count
            << " motors." << std::endl;
  fd_block = open(serial_port.c_str(), O_RDWR | O_NOCTTY);
  if (fd_block == -1) {
    std::cerr << "Error opening serial port: " << std::strerror(errno)
              << std::endl;
    thread_active = false;
    return;
  }
  configure_serial_termios(fd_block);
  serial_thread = std::thread(&CoreThread<T, R>::serialsendrecive, this);
}

template <typename T, typename R> void CoreThread<T, R>::serialsendrecive() {
  using clock = std::chrono::steady_clock;
  const auto period =
      std::chrono::microseconds(1000000 / fequency * motor_count);
  auto next_time = clock::now();
  std::cout << "Serial send/receive thread started." << std::endl;
  while (thread_active) {
    next_time += period;
    for (int i = 0; i < 15; i++) {
      std::pair<T, R> cmd_pair;
      {
        std::lock_guard<std::mutex> lock(cmd_mutex);
        if (!cmd_id_map[i]) {
          // std::cout << "No command set for motor " << i << ", skipping." <<
          // std::endl;
          continue;
        }
        cmd_pair.first = current_cmd[i];
      }
      ssize_t bytes_written = write(fd_block, &cmd_pair.first, sizeof(T));
      if (bytes_written < (ssize_t)sizeof(T)) {
        std::cerr << "Error: Serial write failed for motor " << i
                  << " (errno: " << errno << ")" << std::endl;
        continue; // 写入失败，不尝试读取，直接处理下一个电机
      }

      ssize_t bytes_read = read(fd_block, &cmd_pair.second, sizeof(R));

      // 如果读取字节数不完整，则视为失败，跳过回调
      if (bytes_read < (ssize_t)sizeof(R)) {
        // 这里可以选填一个调试打印，说明读取不完整
        continue;
      }

      if (!cmd_callback) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        cmd_queue.push(
            std::make_pair(cmd_pair, std::chrono::system_clock::now()));
        if (cmd_queue.size() > 1000) {
          cmd_queue.pop();
        }
      } else {
        cmd_callback(cmd_pair.first, cmd_pair.second);
      }
    }
    std::this_thread::sleep_until(next_time);
  }
}

template <typename T, typename R>
bool CoreThread<T, R>::setCmd(unsigned short id, T cmd) {
  if (id > 15 || id < 0)
    return false;
  motor_count++;

  std::lock_guard<std::mutex> lock(cmd_mutex);
  current_cmd[id] = cmd;
  cmd_id_map[id] = true;
  return true;
}

template <typename T, typename R>
bool CoreThread<T, R>::getData(
    std::pair<std::pair<T, R>,
              std::chrono::time_point<std::chrono::system_clock>> &result) {
  // 1. 回调函数冲突警告
  if (cmd_callback) {
    std::cerr << "Warning: getCmd called while cmd_callback is set. This may "
                 "lead to unexpected behavior."
              << std::endl;
    return false;
  }

  // 2. 队列访问：加锁
  std::lock_guard<std::mutex> lock(queue_mutex);

  // 3. 检查队列是否为空
  if (cmd_queue.empty()) {
    // 队列为空，操作失败
    return false;
  }

  // 4. 获取并移除队列头部数据
  result = cmd_queue.front();
  cmd_queue.pop();

  // 5. 操作成功
  return true;
}

template <typename T, typename R> CoreThread<T, R>::~CoreThread() {
  std::cout << "Shutting down CoreThread." << std::endl;
  // 析构函数

  thread_active = false;

  if (serial_thread.joinable()) {
    serial_thread.join();
  }
  if (fd_block != -1) {
    close(fd_block);
  }
}

// 显式实例化模板类以避免链接错误
template class CoreThread<MotorCmdGom, MotorDataGom>;