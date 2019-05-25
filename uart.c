#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>

#define _PORT_NAME_LEN  64

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
	printf("[%s] fd [%d]\n", port, fd);
	if (fd < 0) {
        printf("open port [%s] error [%s]\n", port, strerror(errno));
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

int Read_Data(int fd, char* buff)
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
    memset(buff, 0, sizeof(buff));

    if(nselect <= 0)
//        printf("select failed");
        ;
    else if(FD_ISSET(fd, &readfds) >0)
    {
        nread = read(fd, buff, 1);
//        buff[nread] = '\0';
    }

    if (nread == -1) {
        printf("read [%d] error [%s]\n", fd, strerror(errno));
        close_port(fd);
    }
//    int j = 0;
//    while(buff[j] != '\0')
//    {
//        printf("the readable data is 0x%x\n", buff[j]);
//        j++;
//    }

    return nread;
}


int main(int argc,char** argv)
{
    char **portlist = NULL;
    int count = get_available_port(&portlist);
    printf("find [%d] available port\n", count);
    int i = 0;
    while (portlist[i]) {
        printf("[%s]\n", portlist[i]);
        free(portlist[i]);
        i++;
    }
    free(portlist);

	struct termios tio;
	struct termios stdio;
	struct termios old_stdio;
	int tty_fd;
    int len = 1;
	unsigned char c='D';
	tcgetattr(STDOUT_FILENO,&old_stdio);

	printf("Please start with %s /dev/ttyS1 (for example)\n",argv[0]);
	if (argc != 2)
		return 1;
	// tty_fd=open(argv[1], O_RDWR | O_NONBLOCK);
	tty_fd = open_port(argv[1], B115200);
	printf("tty_fd [%d]\n", tty_fd);
	if (tty_fd < 0)
		return 1;
	memset(&stdio,0,sizeof(stdio));
	stdio.c_iflag=0;
	stdio.c_oflag=0;
	stdio.c_cflag=0;
	stdio.c_lflag=0;
	stdio.c_cc[VMIN]=1;
	stdio.c_cc[VTIME]=0;
	tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
	tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking

//	memset(&tio,0,sizeof(tio));
//	tio.c_iflag=0;
//	tio.c_oflag=0;
//	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
//	tio.c_lflag=0;
//	tio.c_cc[VMIN]=1;
//	tio.c_cc[VTIME]=5;
//
//	cfsetospeed(&tio,B115200);            // 115200 baud
//	cfsetispeed(&tio,B115200);            // 115200 baud
//
//	tcsetattr(tty_fd,TCSANOW,&tio);
	while (c!='q')
	{
		if (get_data(tty_fd,&c, &len)>0)        write(STDOUT_FILENO,&c,1);              // if new data is available on the serial port, print it out
		if (read(STDIN_FILENO,&c,1)>0)  send_data(tty_fd,&c,1);                     // if new data is available on the console, send it to the serial port
//        printf("continue\r\n");
	}

	close(tty_fd);
	tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);

	return EXIT_SUCCESS;
}
