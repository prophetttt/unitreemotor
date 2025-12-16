#include "coreThread.h"
#include <iostream>
#include <termios.h> // 包含 termios 结构体和函数
#include <unistd.h>  // 包含 read, write, close
#include <fcntl.h>   // 包含 open, O_RDWR, O_NOCTTY
#include "unitreeMotor.h"

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
    // configure_serial_termios(&tty, fd_block);
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

        for (const auto & it : initial_cmds)
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
    // configure_serial_termios(&tty, fd_block);
    // serial_thread = std::thread(&CoreThread<T, R>::serialsendrecive, this);
}

template <typename T, typename R>
void CoreThread<T, R>::serialsendrecive()
{
    using clock = std::chrono::steady_clock;
    const auto period = std::chrono::microseconds(1000000 / fequency);
    auto next_time = clock::now();
    std::cout << "Serial send/receive thread started." << std::endl;
    while (thread_active)
    {
        next_time += period;
        for (int i = 0; i < motor_count; i++)
        {
            std::pair<T, R> cmd_pair;
            {
                std::lock_guard<std::mutex> lock(cmd_mutex);
                if (!cmd_id_map[i])
                {
                    std::cout << "No command set for motor " << i << ", skipping." << std::endl;
                    continue;
                }
                cmd_pair.first = current_cmd[i];
            }
            write(fd_block, &cmd_pair.first, sizeof(T));
            read(fd_block, &cmd_pair.second, sizeof(R));

            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                cmd_queue.push(std::make_pair(cmd_pair, std::chrono::system_clock::now()));
            }

            if (cmd_callback)
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                cmd_callback(cmd_pair.first, cmd_pair.second);
                cmd_queue.pop();
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
std::pair<std::pair<T, R>, std::chrono::time_point<std::chrono::system_clock>> CoreThread<T, R>::getCmd()
{
    if (cmd_callback)
    {
        std::cerr << "Warning: getCmd called while cmd_callback is set. This may lead to unexpected behavior." << std::endl;
        return std::make_pair(std::make_pair(T{}, R{}), std::chrono::system_clock::now());
    }
    std::lock_guard<std::mutex> lock(queue_mutex);
    if (cmd_queue.empty())
    {
        return std::make_pair(std::make_pair(T{}, R{}), std::chrono::system_clock::now());
    }
    auto cmd = cmd_queue.front();
    cmd_queue.pop();
    return cmd;
}

void configure_serial_termios(struct termios *tty, int fd)
{
    // 1. 获取当前设置 (必要步骤)
    if (tcgetattr(fd, tty) != 0)
    {
        std::cerr << "Error getting terminal attributes." << std::endl;
        return;
    }

// ---------------------------------------------
// A. 关键配置：控制模式标志 (c_cflag)
// ---------------------------------------------

// 1. 设置波特率 (4.0 Mbit/s)
// 在某些平台上 CBAUD 未定义，所以在可用时清除当前的波特率设置
#ifdef CBAUD
    tty->c_cflag &= ~CBAUD;
#endif

    // 设置输入和输出波特率
    // 尝试使用 B4000000 宏，如果宏不存在，则 cfsetispeed/cfsetospeed 可能会失败。
    cfsetispeed(tty, 4000000); // 4000000 bps
    cfsetospeed(tty, 4000000); // 4000000 bps

    // 2. 数据位 (8 bit)
    tty->c_cflag &= ~CSIZE; // 清除当前的 CSIZE 位掩码
    tty->c_cflag |= CS8;    // 设置为 8 位数据

    // 3. 奇偶校验 (无奇偶校验位)
    tty->c_cflag &= ~PARENB; // 禁用奇偶校验
    tty->c_cflag &= ~PARODD; // 确保奇偶校验位被清除

    // 4. 停止位 (1 bit)
    tty->c_cflag &= ~CSTOPB; // 设置 1 位停止位 (CSTOPB 设置为 2 位停止位，清除则为 1 位)

    // 5. 其他重要标志：
    // CLOCAL：忽略调制解调器状态行，否则打开操作可能会挂起
    // CREAD：启用接收功能
    tty->c_cflag |= (CLOCAL | CREAD);
    std::cout << "Serial port configured: 8N1, 4Mbps." << std::endl;
    // ---------------------------------------------
    // B. 本地模式标志 (c_lflag): 原始模式
    // ---------------------------------------------
    // ICANON: 关闭规范模式 (Canonical Mode)
    // ECHO: 关闭回显
    // ISIG: 关闭信号字符 (INTR, QUIT, SUSP) 解释
    tty->c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // ---------------------------------------------
    // C. 输入模式标志 (c_iflag): 原始模式
    // ---------------------------------------------
    // IXON/IXOFF/IXANY: 关闭软件流控制
    // IGNBRK/BRKINT/PARMRK/ISTRIP/INLCR/IGNCR/ICRNL: 关闭输入处理
    tty->c_iflag &= ~(IXON | IXOFF | IXANY);
    tty->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    // ---------------------------------------------
    // D. 输出模式标志 (c_oflag): 原始模式
    // ---------------------------------------------
    // OPOST：关闭后处理
    tty->c_oflag &= ~OPOST;

    // ---------------------------------------------
    // E. 控制字符 (c_cc): 设置超时和最小读取字节数
    // ---------------------------------------------
    // 阻塞模式或非阻塞模式下的最小读取字节数和超时设置：
    tty->c_cc[VMIN] = 0;  // 最小接收字节数 (0 表示 read() 立即返回)
    tty->c_cc[VTIME] = 5; // 读取超时时间 (单位：0.1 秒，即 500 毫秒)

    // ---------------------------------------------
    // F. 应用新设置
    // ---------------------------------------------
    if (tcsetattr(fd, TCSANOW, tty) != 0)
    {
        std::cerr << "Error setting terminal attributes: " << std::strerror(errno) << std::endl;
    }
    else
    {
        std::cout << "Serial port configured successfully (attempted B4000000)." << std::endl;
    }
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