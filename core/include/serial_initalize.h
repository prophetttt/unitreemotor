#ifndef SERIAL_INITIALIZE_H
#define SERIAL_INITIALIZE_H
#include <sys/ioctl.h>
#ifdef __APPLE__
#include <IOKit/serial/ioss.h>
#endif

void configure_serial_termios(int fd);

#endif