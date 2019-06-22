#ifndef __UART_H__
#define __UART_H__

#include <termios.h>

int get_available_port(char ***portlist);
int open_port(const char *port, unsigned int baudrate);
int close_port(int fd);
int set_baudrate(int fd, unsigned int rate);
int get_data(int fd, unsigned char *buffer, int *buf_len);
int send_data(int fd, const unsigned char *buffer, int buf_len);

#endif	/* __UART_H__ */
