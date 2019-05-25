#include "stdafx.h"
#include "SerialPort.h"
#include <atlstr.h>
#include <atlconv.h>
#include <vector>

#define _PORT_COUNT		100
#define _BUF_LEN		1024

void PrintCommState(DCB dcb)
{
    //  Print some of the DCB structure values
    _tprintf( TEXT("\nBaudRate = %d, ByteSize = %d, Parity = %d, StopBits = %d\n"), 
              dcb.BaudRate, 
              dcb.ByteSize, 
              dcb.Parity,
              dcb.StopBits );
}


SerialPort::SerialPort(std::string port)
{
	mCommHandle = INVALID_HANDLE_VALUE;
	mPortName = port;
	SecureZeroMemory(&mDcb, sizeof(DCB));
	//  Fill in some DCB values and set the com state: 
	//  57,600 bps, 8 data bits, no parity, and 1 stop bit.
	mDcb.BaudRate = CBR_115200;     //  baud rate
	mDcb.ByteSize = 8;              //  data size, xmit and rcv
	mDcb.Parity   = NOPARITY;       //  parity bit
	mDcb.StopBits = ONESTOPBIT;     //  stop bit
}


SerialPort::~SerialPort(void)
{
	if (mCommHandle != INVALID_HANDLE_VALUE) {
		ClosePort();
	}
}

int SerialPort::GetAvailablePort(std::vector<std::string> &portList)
{
	int count = 0;
	int i = 0;

	for (i = 0; i < _PORT_COUNT; i++)
	{
		HANDLE hCom;
		// BOOL fSuccess;
		char name[64] = {0};
	//	TCHAR pcCommPort[64];

		sprintf_s(name, "\\\\.\\COM%d", i);
		//USES_CONVERSION;
		//wcscpy_s(pcCommPort, A2T(name));

		//  Open a handle to the specified com port.
		hCom = CreateFile(name,
						  GENERIC_READ | GENERIC_WRITE,
						  0,      //  must be opened with exclusive-access
						  NULL,   //  default security attributes
						  OPEN_EXISTING, //  must use OPEN_EXISTING
						  0,      //  not overlapped I/O
						  NULL); //  hTemplate must be NULL for comm devices

		if (hCom == INVALID_HANDLE_VALUE) 
		{
			// Handle the error.
			int err = GetLastError();
			// printf ("CreateFile failed with error %d.\n", err);
			if (err == 2)	// ERROR_FILE_NOT_FOUND
				continue;
		}
		//int curSize = portList.size();
		//portList[curSize] = name;
		printf ("find available serial port COM%d.\n", i);
		count++;
		portList.insert(portList.end(), name);
		CloseHandle(hCom);
	}

	return count;
}

int SerialPort::OpenPort()
{
	int sts = 0;
	//TCHAR pcCommPort[64];

	//  Open a handle to the specified com port.
	//USES_CONVERSION;
	//wcscpy_s(pcCommPort, A2T(mPortName.c_str()));
	mCommHandle = CreateFile(mPortName.c_str(),
						GENERIC_READ | GENERIC_WRITE,
						0,      //  must be opened with exclusive-access
						NULL,   //  default security attributes
						OPEN_EXISTING, //  must use OPEN_EXISTING
						0,      //  not overlapped I/O
						NULL ); //  hTemplate must be NULL for comm devices

	if (mCommHandle == INVALID_HANDLE_VALUE) 
	{
		//  Handle the error.
		sts = GetLastError();
		printf ("CreateFile failed with error %d.\n", sts);
		return sts;
	} else {
		// set baud rate, default 115200
		SetBaudRate(0);
		printf ("OpenPort [%s] success.\n", mPortName.c_str());
	}

	COMMTIMEOUTS timeouts;
	memset(&timeouts, 0, sizeof(timeouts));

	timeouts.ReadIntervalTimeout     = 3000;
	timeouts.ReadTotalTimeoutMultiplier  = 4;
	timeouts.ReadTotalTimeoutConstant   = 3000;
	timeouts.WriteTotalTimeoutMultiplier = 4;
	timeouts.WriteTotalTimeoutConstant  = 3000;

	//////// Set the timeout parameters for all read and write operations
	//////// on the port. 
	if (!SetCommTimeouts(mCommHandle, &timeouts))
	{
		//  Handle the error.
		sts = GetLastError();
		printf ("SetCommTimeouts failed with error %d.\n", sts);
	}
	else {
		printf("\nSuccess setting parameters.\n");
	}

	return sts;
}

int SerialPort::ClosePort()
{
	int sts = 0;

	CloseHandle(mCommHandle);
	mCommHandle = INVALID_HANDLE_VALUE;

	return sts;
}

int SerialPort::SetBaudRate(unsigned int rate)
{
	int sts = 0;
	BOOL fSuccess;
	DCB dcb;

	if (rate > 0)
		mDcb.BaudRate = rate;

	if (mCommHandle == INVALID_HANDLE_VALUE)
		return 1;

	fSuccess = SetCommState(mCommHandle, &mDcb);
	if (!fSuccess) 
	{
		//  Handle the error.
		sts = GetLastError();
		printf ("SetCommState failed with error %d.\n", sts);
		// return (3);
	} else {
		//  Get the comm config again.
		fSuccess = GetCommState(mCommHandle, &dcb);

		if (!fSuccess) 
		{
			//  Handle the error.
			printf ("GetCommState failed with error %d.\n", GetLastError());
			return (2);
		}

		PrintCommState(dcb);       //  Output to console

		printf ("Serial port %s successfully reconfigured.\n", mPortName.c_str());
	}

	return sts;
}

int SerialPort::GetData(unsigned char *buffer, int &bufLen)
{
	int sts = 0;
	OVERLAPPED o;
	BOOL fSuccess;

	if (mCommHandle == INVALID_HANDLE_VALUE) {
		sts = OpenPort();
		if (sts != 0)
			return sts;
	}

    fSuccess = SetCommMask(mCommHandle, EV_RXCHAR | EV_ERR | EV_BREAK);

    if (!fSuccess) 
    {
        // Handle the error. 
		sts = GetLastError();
        printf("SetCommMask failed with error %d.\n", sts);
		if (sts == ERROR_ACCESS_DENIED) {
			ClosePort();
		}
        return sts;
    }

	// Create an event object for use by WaitCommEvent. 
	memset(&o,0,sizeof(OVERLAPPED));
    o.hEvent = CreateEvent(
        NULL,   // default security attributes 
        TRUE,   // manual-reset event 
        FALSE,  // not signaled 
        NULL    // no name
         );
#if _WAIT_EVENT
	DWORD dwEvtMask;
	if (WaitCommEvent(mCommHandle, &dwEvtMask, NULL)) 
    {
        if (dwEvtMask & EV_RXCHAR) 
        {
#endif
             // To do.
			//unsigned char buf[101] = {0};
			DWORD nLenOut = 0;
			OVERLAPPED osRead; 
			memset(&osRead,0,sizeof(OVERLAPPED));   
			osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL); 
#if _WAIT_EVENT
			DWORD dwTrans;
			DWORD dwError;
			COMSTAT cs;

			if (!ClearCommError(mCommHandle, &dwError, &cs))
			{
				DWORD dw = GetLastError(); 

				printf("ClearCommError() failed:%d\n",dw);
				// CloseHandle(mCommHandle);
				if (dw == ERROR_ACCESS_DENIED) {
					ClosePort();
				}

				return dw;
			}
#endif
			BOOL bReadStatus = ReadFile(mCommHandle, buffer, bufLen, &nLenOut, NULL);
			if(!bReadStatus || nLenOut == 0)    
			{
				bufLen = nLenOut;
				sts = GetLastError();
				printf("Read failed with error %d.\n", sts);
			}
			else//操作已完成
			{
				//To do
				bufLen = nLenOut;
				printf("Recv : ");
				for (int i = 0; i < nLenOut; i++)
					printf("%02X ", buffer[i]);
				printf("\n");
			}
#if _WAIT_EVENT
        }

    }
    else
    {
        DWORD dwRet = GetLastError();
        if( ERROR_IO_PENDING == dwRet)
        {
            printf("I/O is pending...\n");
			sts = dwRet;
            // To do.
        }
        else {
			sts = GetLastError();
            printf("Wait failed with error %d.\n", sts);
		}
    }
#endif
	if (sts == ERROR_ACCESS_DENIED) {
		ClosePort();
	}

	return sts;
}

int SerialPort::SendData(const unsigned char *buffer, int bufLen)
{
	int sts = 0;
	BOOL bErrorFlag = FALSE;
	//char DataBuffer[] = "This is some test data to write to the file.";
	DWORD dwBytesToWrite = bufLen;
	DWORD dwBytesWritten = 0;
	OVERLAPPED o;

	if (mCommHandle == INVALID_HANDLE_VALUE) {
		sts = OpenPort();
		if (sts != 0)
			return sts;
	}

	// Create an event object for use by WaitCommEvent. 
	memset(&o,0,sizeof(OVERLAPPED));
    o.hEvent = CreateEvent(
        NULL,   // default security attributes 
        TRUE,   // manual-reset event 
        FALSE,  // not signaled 
        NULL    // no name
         );

	printf("Send : ");
	for (int i = 0; i < bufLen; i++)
		printf("%02X ", buffer[i]);
	printf("\n");


	bErrorFlag = WriteFile( 
		mCommHandle,           // open file handle
		buffer,      // start of data to write
		dwBytesToWrite,  // number of bytes to write
		&dwBytesWritten, // number of bytes that were written
		NULL);            // no overlapped structure
	if (FALSE == bErrorFlag)
	{
		printf("Terminal failure: Unable to write to file.\n");
		sts =  GetLastError();
        printf("Send failed with error %d.\n", sts);
	}
	else
	{
		if (dwBytesWritten != dwBytesToWrite)
		{
			// printf("Error: dwBytesWritten != dwBytesToWrite\n");
			sts =  GetLastError();
            printf("Send not compltete [%d].\n", sts);
		}
		else
		{
			printf("Wrote %d bytes successfully.\n", dwBytesWritten);
		}
	}

	if (sts == ERROR_ACCESS_DENIED) {
		ClosePort();
	}

	return sts;
}
