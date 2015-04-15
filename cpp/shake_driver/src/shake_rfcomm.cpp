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


#define SHAKE_RFCOMM_SUPPORTED 1

#include "shake_rfcomm.h"

#include <math.h>
#include <ctype.h>

static BOOL wsa_initialised = FALSE;
static int rfcomm_socket_count = 0;

static long long btaddr_string2int64(char* str) {
	long long btaddr = 0;

	if(strncmp("0x", str, 2) == 0)
		str = str+2;

	int len = strlen(str);
	int pos = 0, i;
	for(i=len-1;i>=0;i--) {
		int c = tolower(str[i]);

		if(c >= '0' && c <= '9') {
			btaddr += (c - '0') * (pow(16.0, pos));
			pos++;
		} else if(c >= 'a' && c <= 'f') {
			btaddr += (10 + (c - 'a')) * (pow(16.0, pos));
			pos++;
		} 
	}

	//printf("Converted %s to %lld, pos %d\n", str, btaddr, pos);
	return btaddr;
}

#ifndef _WIN32
static bdaddr_t btaddr_i64tobdaddr_t(long long addr) {
	int i=0;
	bdaddr_t ba;

	long long mask = 0xFF;
	for(i=0;i<6;i++) {
		ba.b[i] = (addr & mask) >> (8*i);
		mask <<= 8;
	}
	return ba;
}
#endif

shake_rfcomm_socket* shake_open_rfcomm_i64(shake_rfcomm_socket* port, long long btaddr) {
#ifdef _WIN32
	if(!wsa_initialised) {
		WORD wVersionRequested;
		WSADATA wsaData;
		wVersionRequested = MAKEWORD( 2, 0 );
		WSAStartup( wVersionRequested, &wsaData );
		wsa_initialised = TRUE;
	}

	port->sock = socket (AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if(port->sock == INVALID_SOCKET) {
		WSACleanup();
		port->sock = INVALID_SOCKET;
		return NULL;
	}

	memset(&(port->addr), 0, sizeof(SOCKADDR_BTH));
	port->addr.addressFamily = AF_BTH;
	port->addr.port = 1; // always 1 for the SHAKE I think
	port->addr.btAddr = btaddr;

	if (connect (port->sock, (SOCKADDR *)&(port->addr), sizeof(SOCKADDR_BTH)) == SOCKET_ERROR) {
		closesocket(port->sock);
		WSACleanup();
		port->sock = INVALID_SOCKET;
		return NULL;
	}

	rfcomm_socket_count++;
	
	return port;
#else
	struct sockaddr_rc addr;
	int status;

	port->sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	if(port->sock == -1)
		return NULL;

	addr.rc_family = AF_BLUETOOTH;
	addr.rc_channel = (uint8_t) 1;
	addr.rc_bdaddr = btaddr_i64tobdaddr_t(btaddr);

	status = connect(port->sock, (struct sockaddr*)&addr, sizeof(addr));
	if(status == -1) {
		close(port->sock);
		return NULL;
	}

	return port;
#endif
}

shake_rfcomm_socket* shake_open_rfcomm_str(shake_rfcomm_socket* port, char* btaddr) {
	return shake_open_rfcomm_i64(port, btaddr_string2int64(btaddr));
}


int shake_close_rfcomm(shake_rfcomm_socket* port) {
#ifdef _WIN32
	if(port == NULL)
		return 0;

	closesocket(port->sock);
	rfcomm_socket_count--;
	if(rfcomm_socket_count == 0) 
		WSACleanup();

	return 1;
#else
	close(port->sock);
	return 1;
#endif
}
	
int read_rfcomm_bytes(shake_device_private* devpriv, char* buf, int bytes_to_read) {
	#ifdef SHAKE_RFCOMM_SUPPORTED
	DWORD bytes_read = 0;
	int sleepcounter = 0;
	int remaining_bytes = bytes_to_read;

	while(1) {
		int len = recv(devpriv->port.rfcomm.sock, buf+bytes_read, remaining_bytes, 0);
		if(len == SOCKET_ERROR) {
			break;
		}
		bytes_read += len;
		remaining_bytes -= len;

		if(remaining_bytes == 0)
			break;
		
		/* check if the thread is exiting, and if so break out of the loop and return */
		if(devpriv->rthread_done)
			break;

		/* otherwise just loop back round and read the port again */
		++sleepcounter;
		if(sleepcounter > 10) {
			shake_sleep(1);
			sleepcounter = 0;
		}
		continue;
	}

	devpriv->data_recv += (bytes_read - remaining_bytes);
	return bytes_to_read - remaining_bytes;
	#else
	return 0;
	#endif
}

int write_rfcomm_bytes(shake_device_private* devpriv, char* buf, int bytes_to_write) {
	#ifdef SHAKE_RFCOMM_SUPPORTED
	DWORD bytes_written = 0;
	int remaining_bytes = bytes_to_write;

	while(1) {
		int len = send(devpriv->port.rfcomm.sock, buf+bytes_written, remaining_bytes, 0);

		if(len == SOCKET_ERROR) {

			break;
		}

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= len;
		bytes_written += len;

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

int write_rfcomm_bytes_delayed(shake_device_private* devpriv, char* buf, int bytes_to_write, int chunk_size, int delay_ms) {
	#ifdef SHAKE_RFCOMM_SUPPORTED
	DWORD bytes_written = 0;
	int remaining_bytes = bytes_to_write;

	while(1) {
		int current_remaining_bytes = remaining_bytes;// ((chunk_size >= remaining_bytes) ? chunk_size : remaining_bytes);
		if(remaining_bytes > chunk_size)
			current_remaining_bytes = chunk_size;
		int len = send(devpriv->port.rfcomm.sock, buf+bytes_written, current_remaining_bytes, 0);

		if(len == SOCKET_ERROR) {
			break;
		}

		/* subtract the bytes we just wrote from the total amount we want */
		remaining_bytes -= len;
		bytes_written += len;

		shake_sleep(delay_ms);

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
