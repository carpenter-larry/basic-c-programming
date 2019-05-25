#pragma once

#include<windows.h>
#include <string>
#include <vector>

// https://docs.microsoft.com/zh-cn/windows/desktop/Debug/system-error-codes
/*
	ERROR_SUCCESS
	0 (0x0)
	The operation completed successfully.
	ERROR_INVALID_FUNCTION
	1 (0x1)
	Incorrect function.
	ERROR_FILE_NOT_FOUND
	2 (0x2)
	The system cannot find the file specified.
	ERROR_PATH_NOT_FOUND
	3 (0x3)
	The system cannot find the path specified.
	ERROR_TOO_MANY_OPEN_FILES
	4 (0x4)
	The system cannot open the file.
	ERROR_ACCESS_DENIED
	5 (0x5)
	Access is denied.	
*/

class SerialPort
{
public:
	SerialPort(std::string port);
	~SerialPort(void);

public:
	// 返回可用串口数
	static int GetAvailablePort(std::vector<std::string> &portList);
	int OpenPort();
	int ClosePort();
	int SetBaudRate(unsigned int rate);
	int GetData(unsigned char *buffer, int &bufLen);
	int SendData(const unsigned char *buffer, int bufLen);


private:
	std::string mPortName;
	HANDLE mCommHandle;
	DCB mDcb;						// 串口参数
};

