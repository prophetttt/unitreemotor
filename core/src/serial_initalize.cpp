#include "serial_initalize.h"
#include <iostream>
#include <termios.h> // 包含 termios 结构体和函数
#include <unistd.h>  // 包含

#define TARGET_BAUDRATE 4000000

termios tty;
void configure_serial_termios(int fd) {
  // 1. 获取当前设置
  if (tcgetattr(fd, &tty) != 0) {
    std::cerr << "Error getting terminal attributes: " 
              << std::endl;
    return;
  }

  // ---------------------------------------------
  // A. 关键配置：控制模式标志 (c_cflag)
  // ---------------------------------------------

  // 1. 设置波特率（macOS/Linux POSIX 兼容部分）
  // tty->c_cflag &= ~CBAUD;
  // 使用标准函数设置波特率，这在许多 Linux/POSIX 系统上是必要的。
  // 在 macOS 上，这对超高波特率可能会失败，但先进行设置。
  cfsetispeed(&tty, B38400); // 暂时设置为一个标准高波特率
  cfsetospeed(&tty, B38400); // 因为 CBAUD 可能不支持 4M

  // 2. 数据位 (8 bit)
  tty.c_cflag &= ~CSIZE;
  tty.c_cflag |= CS8;

  // 3. 奇偶校验 (无奇偶校验位)
  tty.c_cflag &= ~PARENB;
  tty.c_cflag &= ~PARODD;
  // 4. 停止位 (1 bit)
  tty.c_cflag &= ~CSTOPB;

  // 5. 其他重要标志：
  tty.c_cflag |= (CLOCAL | CREAD);
  // std::cout << "Serial port configured: 8N1." << std::endl;

  // ---------------------------------------------
  // B. 本地模式标志 (c_lflag): 原始模式
  // ---------------------------------------------
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // ---------------------------------------------
  // C. 输入模式标志 (c_iflag): 原始模式
  // ---------------------------------------------
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

  // ---------------------------------------------
  // D. 输出模式标志 (c_oflag): 原始模式
  // ---------------------------------------------
  tty.c_oflag &= ~OPOST;

  // ---------------------------------------------
  // E. 控制字符 (c_cc): 设置超时和最小读取字节数
  // ---------------------------------------------
  // 非阻塞模式下的最小读取字节数和超时设置：
  tty.c_cc[VMIN] = 0;
  tty.c_cc[VTIME] = 5; // 500 毫秒超时

  // ---------------------------------------------
  // F. 应用标准设置
  // ---------------------------------------------
  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    std::cerr << "Error setting standard terminal attributes: "
             << std::endl;
    return;
  }

  // ---------------------------------------------
  // G. macOS/Darwin 特有：使用 IOSSIOSPEED 设置超高波特率
  // ---------------------------------------------
#ifdef __APPLE__
  speed_t speed = TARGET_BAUDRATE; // 4000000
  std::cout << "Attempting to set macOS-specific IOSSIOSPEED to "
            << TARGET_BAUDRATE << " bps." << std::endl;
  // 注意：IOSSIOSPEED 需要文件描述符是打开状态
  if (ioctl(fd, IOSSIOSPEED, &speed) == -1) {
    std::cerr << "Warning: Failed to set non-standard baud rate ("
              << TARGET_BAUDRATE
              << " bps) using IOSSIOSPEED. Error: " << std::strerror(errno)
              << std::endl;
    // 尝试另一种兼容性设置（如果需要）
  } else {
    std::cout << "Serial port configured successfully using IOSSIOSPEED: "
              << TARGET_BAUDRATE << " bps." << std::endl;
  }
#else
// 对于非 macOS 系统，尝试使用 cfsetispeed/cfsetospeed 的高波特率宏（例如
// B4000000），如果它存在
#ifdef B4000000
  cfsetispeed(&tty, B4000000);
  cfsetospeed(&tty, B4000000);
  if (tcsetattr(fd, TCSANOW, &tty) != 0) {
    std::cerr << "Error setting B4000000 terminal attributes: "
             << std::endl;
  } else {
    std::cout << "Serial port configured successfully (attempted B4000000 on "
                 "non-Apple system)."
              << std::endl;
  }
#else
  std::cerr << "Warning: System does not support B4000000 macro or "
               "IOSSIOSPEED. Baud rate might be incorrect."
            << std::endl;
#endif // B4000000
#endif // __APPLE__
}