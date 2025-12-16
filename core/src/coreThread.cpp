#include "coreThread.h"
#include <iostream>
#include <termios.h> // 包含 termios 结构体和函数
#include <unistd.h>  // 包含 read, write, close
#include <fcntl.h>   // 包含 open, O_RDWR, O_NOCTTY
#include "unitreeMotor.h"
#include <sys/ioctl.h>
#ifdef __APPLE__
#include <IOKit/serial/ioss.h>
#endif

#define TARGET_BAUDRATE 4000000

termios tty;
void configure_serial_termios(struct termios *tty, int fd);

template <typename T, typename R>
CoreThread<T, R>::CoreThread(std::string serial_port, unsigned short freq, std::function<bool(T &, R &)> callback)
{
    this->fequency = freq;
    this->cmd_callback = callback;
    this->serial_port = serial_port;
    std::cout << "CoreThread started on port: " << serial_port << " with frequency: " << freq << " Hz controlling " << motor_count << " motors." << std::endl;
    fd_block = open(serial_port.c_str(), O_RDWR | O_NOCTTY);
    if (fd_block == -1)
    {
        std::cerr << "Error opening serial port: " << std::strerror(errno) << std::endl;
        thread_active = false;
        return;
    }
    configure_serial_termios(&tty, fd_block);
    serial_thread = std::thread(&CoreThread<T, R>::serialsendrecive, this);
}

template <typename T, typename R>
CoreThread<T, R>::CoreThread(std::string serial_port, unsigned short freq, std::function<bool(T &, R &)> callback, std::unordered_map<int, MotorCmdGom> initial_cmds)
{
    this->fequency = freq;
    this->cmd_callback = callback;
    this->serial_port = serial_port;
    this->motor_count = initial_cmds.size();
    if (!initial_cmds.empty())
    {
        // 1. 获取指向第一个元素的迭代器
        auto it = initial_cmds.begin();

        for (const auto &it : initial_cmds)
        {
            // 2. 处理当前元素
            unsigned short id = it.first;
            T cmd = it.second;
            cmd_id_map[id] = true;
            current_cmd[id] = cmd;
        }
    }

    std::cout << "CoreThread started on port: " << serial_port << " with frequency: " << freq << " Hz controlling " << motor_count << " motors." << std::endl;
    fd_block = open(serial_port.c_str(), O_RDWR | O_NOCTTY);
    if (fd_block == -1)
    {
        std::cerr << "Error opening serial port: " << std::strerror(errno) << std::endl;
        thread_active = false;
        return;
    }
    configure_serial_termios(&tty, fd_block);
    serial_thread = std::thread(&CoreThread<T, R>::serialsendrecive, this);
}

template <typename T, typename R>
void CoreThread<T, R>::serialsendrecive()
{
    using clock = std::chrono::steady_clock;
    const auto period = std::chrono::microseconds(1000000 / fequency * motor_count);
    auto next_time = clock::now();
    std::cout << "Serial send/receive thread started." << std::endl;
    while (thread_active)
    {
        next_time += period;
        for (int i = 0; i < 15; i++)
        {
            std::pair<T, R> cmd_pair;
            {
                std::lock_guard<std::mutex> lock(cmd_mutex);
                if (!cmd_id_map[i])
                {
                    // std::cout << "No command set for motor " << i << ", skipping." << std::endl;
                    continue;
                }
                cmd_pair.first = current_cmd[i];
            }
            write(fd_block, &cmd_pair.first, sizeof(T));
            read(fd_block, &cmd_pair.second, sizeof(R));

            if (!cmd_callback)
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                cmd_queue.push(std::make_pair(cmd_pair, std::chrono::system_clock::now()));
                if(cmd_queue.size() > 1000){
                    cmd_queue.pop();
                }
            }
            else
            {
                cmd_callback(cmd_pair.first, cmd_pair.second);
            }
        }
        std::this_thread::sleep_until(next_time);
    }
}

template <typename T, typename R>
bool CoreThread<T, R>::setCmd(unsigned short id, T cmd)
{
    if (id > 15 || id < 0)
        return false;
    motor_count++;

    std::lock_guard<std::mutex> lock(cmd_mutex);
    current_cmd[id] = cmd;
    cmd_id_map[id] = true;
    return true;
}

template <typename T, typename R>
bool CoreThread<T, R>::getCmd(
    std::pair<std::pair<T, R>, std::chrono::time_point<std::chrono::system_clock>>& result)
{
    // 1. 回调函数冲突警告
    if (cmd_callback)
    {
        std::cerr << "Warning: getCmd called while cmd_callback is set. This may lead to unexpected behavior." << std::endl;
        // 即使有警告，我们仍然尝试从队列中获取数据（如果存在），但返回 false 以表示可能的数据流冲突。
        // 或者，更严格的做法是直接返回 false。这里选择返回 false。
        return false; 
    }
    
    // 2. 队列访问：加锁
    std::lock_guard<std::mutex> lock(queue_mutex);
    
    // 3. 检查队列是否为空
    if (cmd_queue.empty())
    {
        // 队列为空，操作失败
        return false;
    }
    
    // 4. 获取并移除队列头部数据
    result = cmd_queue.front();
    cmd_queue.pop();
    
    // 5. 操作成功
    return true;
}

void configure_serial_termios(struct termios *tty, int fd)
{
    // 1. 获取当前设置
    if (tcgetattr(fd, tty) != 0)
    {
        std::cerr << "Error getting terminal attributes: " << std::strerror(errno) << std::endl;
        return;
    }

    // ---------------------------------------------
    // A. 关键配置：控制模式标志 (c_cflag)
    // ---------------------------------------------

    // 1. 设置波特率（macOS/Linux POSIX 兼容部分）
    // tty->c_cflag &= ~CBAUD;
    // 使用标准函数设置波特率，这在许多 Linux/POSIX 系统上是必要的。
    // 在 macOS 上，这对超高波特率可能会失败，但先进行设置。
    cfsetispeed(tty, B38400); // 暂时设置为一个标准高波特率
    cfsetospeed(tty, B38400); // 因为 CBAUD 可能不支持 4M

    // 2. 数据位 (8 bit)
    tty->c_cflag &= ~CSIZE;
    tty->c_cflag |= CS8;

    // 3. 奇偶校验 (无奇偶校验位)
    tty->c_cflag &= ~PARENB;
    tty->c_cflag &= ~PARODD;

    // 4. 停止位 (1 bit)
    tty->c_cflag &= ~CSTOPB;

    // 5. 其他重要标志：
    tty->c_cflag |= (CLOCAL | CREAD);
    // std::cout << "Serial port configured: 8N1." << std::endl;

    // ---------------------------------------------
    // B. 本地模式标志 (c_lflag): 原始模式
    // ---------------------------------------------
    tty->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // ---------------------------------------------
    // C. 输入模式标志 (c_iflag): 原始模式
    // ---------------------------------------------
    tty->c_iflag &= ~(IXON | IXOFF | IXANY);
    tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    // ---------------------------------------------
    // D. 输出模式标志 (c_oflag): 原始模式
    // ---------------------------------------------
    tty->c_oflag &= ~OPOST;

    // ---------------------------------------------
    // E. 控制字符 (c_cc): 设置超时和最小读取字节数
    // ---------------------------------------------
    // 非阻塞模式下的最小读取字节数和超时设置：
    tty->c_cc[VMIN] = 0;
    tty->c_cc[VTIME] = 5; // 500 毫秒超时

    // ---------------------------------------------
    // F. 应用标准设置
    // ---------------------------------------------
    if (tcsetattr(fd, TCSANOW, tty) != 0)
    {
        std::cerr << "Error setting standard terminal attributes: " << std::strerror(errno) << std::endl;
        return;
    }

    // ---------------------------------------------
    // G. macOS/Darwin 特有：使用 IOSSIOSPEED 设置超高波特率
    // ---------------------------------------------
#ifdef __APPLE__
    speed_t speed = TARGET_BAUDRATE; // 4000000
    std::cout << "Attempting to set macOS-specific IOSSIOSPEED to " << TARGET_BAUDRATE << " bps." << std::endl;
    // 注意：IOSSIOSPEED 需要文件描述符是打开状态
    if (ioctl(fd, IOSSIOSPEED, &speed) == -1)
    {
        std::cerr << "Warning: Failed to set non-standard baud rate (" << TARGET_BAUDRATE << " bps) using IOSSIOSPEED. Error: " << std::strerror(errno) << std::endl;
        // 尝试另一种兼容性设置（如果需要）
    }
    else
    {
        std::cout << "Serial port configured successfully using IOSSIOSPEED: " << TARGET_BAUDRATE << " bps." << std::endl;
    }
#else
// 对于非 macOS 系统，尝试使用 cfsetispeed/cfsetospeed 的高波特率宏（例如 B4000000），如果它存在
#ifdef B4000000
    cfsetispeed(tty, B4000000);
    cfsetospeed(tty, B4000000);
    if (tcsetattr(fd, TCSANOW, tty) != 0)
    {
        std::cerr << "Error setting B4000000 terminal attributes: " << std::strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "Serial port configured successfully (attempted B4000000 on non-Apple system)." << std::endl;
    }
#else
    std::cerr << "Warning: System does not support B4000000 macro or IOSSIOSPEED. Baud rate might be incorrect." << std::endl;
#endif // B4000000
#endif // __APPLE__
}

template <typename T, typename R>
CoreThread<T, R>::~CoreThread()
{
    std::cout << "Shutting down CoreThread." << std::endl;
    // 析构函数

    thread_active = false;

    if (serial_thread.joinable())
    {
        serial_thread.join();
    }
    if (fd_block != -1)
    {
        close(fd_block);
    }
}

// 显式实例化模板类以避免链接错误
template class CoreThread<MotorCmdGom, MotorDataGom>;