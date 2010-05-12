/*	Copyright (c) 2006-2009, University of Glasgow
*	All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
*
*		* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
*		* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
*			in the documentation and/or other materials provided with the distribution.
*		* Neither the name of the University of Glasgow nor the names of its contributors may be used to endorse or promote products derived 
*			from this software without specific prior written permission.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
*	THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
*	BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
*	GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
*	LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "shake_serial_win32.h"
#include <stdio.h>

#include "shake_platform.h"

/* Windows only */
#ifdef _WIN32
typedef LONG (__stdcall *_RegOpenKeyEx)(HKEY, LPCTSTR, DWORD, REGSAM, PHKEY);
typedef LONG (__stdcall *_RegQueryValueEx)(HKEY, LPCTSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
typedef LONG (__stdcall *_RegCloseKey)(HKEY);

/*	Scans registry for WIDCOMM Bluetooth settings and tries to find outgoing COM port
*	numbers. */
SHAKE_API int shake_scan_for_comm_ports(int** ports, int* portcount) {
	HKEY basekey, key;
	TCHAR GUIDBuf[50];
	TCHAR keyNameBuf[256];
	DWORD keyNameSize;
	DWORD commPortNum = -1;
	DWORD valueType;
	int i, *portarray;
	HMODULE dll_handle;
	_RegOpenKeyEx __RegOpenKeyEx;
	_RegQueryValueEx __RegQueryValueEx;
	_RegCloseKey __RegCloseKey;

	#ifdef _WIN32_WCE
	basekey = HKEY_LOCAL_MACHINE;
	dll_handle = LoadLibrary(_T("coredll.dll"));
	if(dll_handle == NULL) {
		return -1;
	}
	
	// get function addresses
	__RegOpenKeyEx = (_RegOpenKeyEx)GetProcAddress(dll_handle, _T("RegOpenKeyExW"));
	__RegQueryValueEx = (_RegQueryValueEx)GetProcAddress(dll_handle, _T("RegQueryValueExW"));
	__RegCloseKey = (_RegCloseKey)GetProcAddress(dll_handle, _T("RegCloseKey"));
	#else
	basekey = HKEY_CURRENT_USER;
	dll_handle = LoadLibrary("advapi32.dll");
	if(dll_handle == NULL) {
		return -1;
	}
	
	// get function addresses
	__RegOpenKeyEx = (_RegOpenKeyEx)GetProcAddress(dll_handle, "RegOpenKeyExA");
	__RegQueryValueEx = (_RegQueryValueEx)GetProcAddress(dll_handle, "RegQueryValueExA");
	__RegCloseKey = (_RegCloseKey)GetProcAddress(dll_handle, "RegCloseKey");
	#endif
	
	// check for WIDCOMM driver key under HKCU/HKLM
	if (__RegOpenKeyEx(basekey, _T("Software\\Widcomm\\BTConfig\\Applications"),
											0, KEY_READ, &key) != ERROR_SUCCESS) {
		FreeLibrary(dll_handle);
		return -1;
	}

	__RegCloseKey(key);

	memset(GUIDBuf, 0, sizeof(TCHAR)*50);
	*portcount = 0;
	*ports = portarray = (int*)malloc(sizeof(int) * 20);

	// scan the subkeys of Software/Widcomm/BTConfig/Applications, each of which will be a
	// 4 digit number with leading zeroes, corresponding to a service profile.
	for(i=1;i<20; i++){
		memset(keyNameBuf, 0, 256);
		memset(GUIDBuf, 0, 50);
		_stprintf(keyNameBuf, _T("Software\\Widcomm\\BTConfig\\Applications\\%04d"), i);

		// try to open the subkey
		if (__RegOpenKeyEx(basekey, keyNameBuf, 0, KEY_READ, &key) != ERROR_SUCCESS)
			continue;

		// if the subkey was opened OK, check for the "GUID" value
		keyNameSize = sizeof(GUIDBuf);
		if (__RegQueryValueEx(key, _T("GUID"), 0, &valueType, (LPBYTE)GUIDBuf, &keyNameSize) != ERROR_SUCCESS) {
			__RegCloseKey(key);
			continue;
		}

		// Each serial_win32.port entry should have a GUID value of "{00001101-0000-1000-8000-00805F9B34FB}"
		if((_tcscmp(_T("{00001101-0000-1000-8000-00805F9B34FB}"), GUIDBuf) == 0)) {
			keyNameSize = sizeof(commPortNum);

			// if it looks like this is the serial_win32.port entry, read the "ComPortNumber" value
			if (__RegQueryValueEx(key, _T("ComPortNumber"), 0, &valueType, (unsigned char*) &commPortNum, &keyNameSize) == ERROR_SUCCESS) {
				__RegCloseKey(key);
				portarray[*portcount] = commPortNum;
				*portcount += 1;
			}
		}
	}

	//printf("Couldn't automatically find which COM port to use\n");
	if(*portcount == 0) {
		free(portarray);
		FreeLibrary(dll_handle);
		return -1;
	}

	FreeLibrary(dll_handle);
	return 1;
}
#endif

shake_serial_port_win32* shake_open_serial_win32(shake_serial_port_win32* port, int number, int device_type) {
#ifdef _WIN32
	TCHAR portname[24];

	if(number == -1) {
		int* portarray = NULL, portcount, res;
		res = shake_scan_for_comm_ports(&portarray, &portcount);

		if(res == -1) 
			return NULL;

		number = portarray[0];
		free(portarray);
	}
	// NOTE: the "COMx" style of device name only seems to work when x < 10
	// but the "\\\\.\\COMx" style works for higher numbers too
	#ifndef _WIN32_WCE
	_stprintf(portname, _T("\\\\.\\COM%d"), number);
	#else
	// on PocketPC/Windows Mobile, port numbers should be under 10 and require the
	// trailing colon. Not sure if the above works on these OSes
	_stprintf(portname, _T("COM%d:"), number);
	#endif

	port->port = CreateFile(portname, GENERIC_READ | GENERIC_WRITE, 0,
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(port->port == INVALID_HANDLE_VALUE) {
		SHAKE_DBG("CreateFile failed (%d)\n", GetLastError());
		//printf("*** ERROR: Failed to open serial_win32.port, error = %d\n", GetLastError());
		return NULL;
	}

	memset(&(port->dcb), 0, sizeof(DCB));
	memset(&(port->cto), 0, sizeof(COMMTIMEOUTS));

	// MSDN: ReadIntervalTimeout to MAXWORD and set both ReadTotalTimeoutMultiplier 
	// and ReadTotalTimeoutConstant to zero.
	port->cto.ReadIntervalTimeout = MAXDWORD;
	port->cto.ReadTotalTimeoutMultiplier = 0;
	port->cto.ReadTotalTimeoutConstant = 0;
	port->cto.WriteTotalTimeoutConstant = 0;
	port->cto.WriteTotalTimeoutMultiplier = 0;
	/*port->cto.ReadIntervalTimeout = 10000; // time in ms allowed between successive bytes
	port->cto.ReadTotalTimeoutMultiplier = 1; // timeout multiplier
	port->cto.ReadTotalTimeoutConstant = 0; // additional timeout constant
	port->cto.WriteTotalTimeoutConstant = 10000;
	port->cto.WriteTotalTimeoutMultiplier = 1;
	*/

	port->dcb.DCBlength = sizeof(DCB);
	GetCommState(port->port, &(port->dcb));

	if(device_type == 0) {
		port->dcb.BaudRate = sk6_serial_params[SHAKE_SERIAL_BAUDRATE];
		port->dcb.fBinary = TRUE;
		port->dcb.fParity = FALSE;
		port->dcb.ByteSize = 8;
		port->dcb.Parity = sk6_serial_params[SHAKE_SERIAL_PARITY];
		port->dcb.StopBits = sk6_serial_params[SHAKE_SERIAL_STOP_BITS];
	} else {
		port->dcb.BaudRate = sk7_serial_params[SHAKE_SERIAL_BAUDRATE];
		port->dcb.fBinary = TRUE;
		port->dcb.fParity = FALSE;
		port->dcb.ByteSize = 8;
		port->dcb.Parity = sk7_serial_params[SHAKE_SERIAL_PARITY];
		port->dcb.StopBits = sk7_serial_params[SHAKE_SERIAL_STOP_BITS];
	}

	
	SetCommTimeouts(port->port, &(port->cto));
	SetCommState(port->port, &(port->dcb));
	SetupComm(port->port, 10000, 10000);

	return port;
#else
	return NULL;
#endif
}

int shake_close_serial_win32(shake_serial_port_win32* port) {
#ifdef _WIN32
	PurgeComm(port->port, PURGE_RXABORT | PURGE_TXABORT | PURGE_RXCLEAR | PURGE_TXCLEAR);

	if(CloseHandle(port->port) == 0) 
		return 0;
#endif
	return 1;
}

/*	utility func, reads <bytes_to_read> bytes from the port associated with <devpriv> 
*	into <buf>, handling any timeouts that occur during this operation.
*	Returns number of bytes read. */
int read_serial_bytes_win32(shake_device_private* devpriv, char* buf, int bytes_to_read) {
#ifdef _WIN32
	DWORD bytes_read;
	int sleepcounter = 0;
	int attempts = 0;
	int remaining_bytes = bytes_to_read;

	/* read the port in a loop to deal with timeouts */
	while(1) {
		int res = ReadFile(devpriv->port.serial_win32.port, buf + (bytes_to_read - remaining_bytes), remaining_bytes, &bytes_read, NULL);
		if(res != TRUE) {
			return bytes_to_read - remaining_bytes;
		}

		/* subtract the bytes we just read from the total amount we want */
		remaining_bytes -= bytes_read;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* check if the thread is exiting, and if so break out of the loop and return */
			if(devpriv->rthread_done)
				break;

			/* otherwise just loop back round and read the port again */
			++sleepcounter;
			if(sleepcounter > 15) {
				shake_sleep(1);
				attempts++;
				sleepcounter = 0;

				if(attempts > 30)
					break;
			}
			continue;
		} else
			/* if all bytes have been read, break out of loop and return */
			break;
	}
	devpriv->data_recv += (bytes_to_read - remaining_bytes);
	return bytes_to_read - remaining_bytes;
#else
	return 0;
#endif
}

/*	utility func, writes <bytes_to_write> bytes to the port associated with <devpriv>
*	from <buf>, handling any timeouts that occur during this operation.
*	Returns number of bytes written. */
int write_serial_bytes_win32(shake_device_private* devpriv, char* buf, int bytes_to_write) {
#ifdef _WIN32
	DWORD bytes_written;
	int remaining_bytes = bytes_to_write;

	/* write the port in a loop to deal with timeouts */
	while(1) {
		if(!WriteFile(devpriv->port.serial_win32.port, buf + (bytes_to_write - remaining_bytes), remaining_bytes, &bytes_written, NULL))
			return bytes_to_write - remaining_bytes;

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= bytes_written;

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* loop back round and write the port again */
			continue;
		} else
			/* if all bytes have been written, break out of loop and return */
			break;
	}
	return bytes_to_write - remaining_bytes;
#else
	return 0;
#endif
}

int write_serial_bytes_delayed_win32(shake_device_private* devpriv, char* buf, int bytes_to_write, int chunk_size, int delay_ms) {
#ifdef _WIN32
	DWORD bytes_written;
	int remaining_bytes = bytes_to_write;

	#ifdef _WIN32
	/* write the port in a loop to deal with timeouts */
	while(1) {
 		int current_remaining_bytes = remaining_bytes;// ((chunk_size >= remaining_bytes) ? chunk_size : remaining_bytes);
		if(remaining_bytes > chunk_size)
			current_remaining_bytes = chunk_size;
		if(!WriteFile(devpriv->port.serial_win32.port, buf + (bytes_to_write - remaining_bytes), current_remaining_bytes, &bytes_written, NULL))
			return bytes_to_write - remaining_bytes;

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= bytes_written;

		/* delay */
		shake_sleep(delay_ms);

		/* if we didn't get them all.. */
		if(remaining_bytes != 0) {
			/* loop back round and write the port again */
			continue;
		} else
			/* if all bytes have been written, break out of loop and return */
			break;
	}
	#endif
	return bytes_to_write - remaining_bytes;
#else
	return 0;
#endif
}
