#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "uart.h"

#define _PORT_NAME_LEN  64
//#define BUFSIZ 1024*4


int get_available_port(char ***portlist)
{
    int count = 0;
    char buf[_PORT_NAME_LEN] = {0};
    int i = 0;
    int fd = 0;

    *portlist = (char **)malloc(sizeof(char *));
    (*portlist)[0] = NULL;

    for (i = 0; i < 100; i++) {
        sprintf(buf, "/dev/ttyUSB%d", i);
        fd = open_port(buf, B115200);
        if (fd > 0) {
            close_port(fd);
            printf("Get available port [%s]\n", buf);
            (*portlist)[count] = (char *)malloc(_PORT_NAME_LEN * sizeof(char));
            strncpy((*portlist)[count], buf, _PORT_NAME_LEN);
            count++;
        }
    }

    (*portlist)[count] = NULL;

    return count;
}

int open_port(const char *port, unsigned int baudrate)
{
    int fd = 0;
    struct termios tio;

    fd=open(port, O_RDWR | O_NONBLOCK | O_SYNC);
	// printf("[%s] fd [%d]\n", port, fd);
	if (fd < 0) {
        // printf("open port [%s] error [%s]\n", port, strerror(errno));
		return fd;
	}

	if (lockf(fd, F_TLOCK, 0) == -1) {
        printf("lock port [%s] error [%s]\n", port, strerror(errno));
		return -1;
	}

	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	tio.c_lflag=0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;

	cfsetospeed(&tio,baudrate);            // 115200 baud
	cfsetispeed(&tio,baudrate);            // 115200 baud

	tcsetattr(fd,TCSANOW,&tio);

    return fd;
}

int close_port(int fd)
{
    return close(fd);
}

int set_baudrate(int fd, unsigned int rate)
{
    int sts = 0;
    struct termios tio;

    tcgetattr(fd, &tio);
	cfsetospeed(&tio,rate);            // 115200 baud
	cfsetispeed(&tio,rate);            // 115200 baud
	tcsetattr(fd,TCSANOW,&tio);

    return sts;
}

int get_data(int fd, unsigned char *buffer, int *buf_len)
{
    int nread = 0;
    int fd_max;
    int nselect;
	struct timeval timeout;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd,&readfds);
    fd_max = fd+1;

    timeout.tv_sec = 1;
  	timeout.tv_usec = 1;
    nselect = select(fd_max, &readfds, NULL, NULL, &timeout);
//    memset(buff, 0, sizeof(buff));

    if(nselect <= 0)
//        printf("select failed");
        ;
    else if(FD_ISSET(fd, &readfds) >0)
    {
        nread = read(fd, buffer, *buf_len);
//        buff[nread] = '\0';
        if (nread == -1) {
            printf("read [%d] error [%s]\n", fd, strerror(errno));
            close_port(fd);
            fd = -1;
        }
    }

//    int j = 0;
//    while(buff[j] != '\0')
//    {
//        printf("the readable data is 0x%x\n", buff[j]);
//        j++;
//    }

    return nread;
}

int send_data(int fd, const unsigned char *buffer, int buf_len)
{
    int sts = 0;

    sts = write(fd, buffer, buf_len);
    if (sts == -1) {
        printf("send [%d] error [%s]\n", fd, strerror(errno));
        close_port(fd);
        fd = -1;
    }

    return sts;
}

