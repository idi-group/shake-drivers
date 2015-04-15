#ifndef _SK7_H_
#define _SK7_H_

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


#include "SHAKE.h"

class SK7 :
	public SHAKE
{
public:
	SK7(shake_device* sd, shake_device_private* sdp);
	virtual ~SK7(void);

	// parses a complete ASCII packet
	virtual int parse_ascii_packet(int packet_type, char* packetbuf, int packetlen, int playback, void* timestamp_packet);

	// reads a complete ASCII packet (minus header)
	virtual int read_ascii_packet(int packet_type, char* packetbuf);

	// parses a complete raw packet
	virtual int parse_raw_packet(int packet_type, char* packetbuf, int packetlen, int has_seq);

	// reads a complete raw packet (minus header)
	virtual int read_raw_packet(int packet_type, char* packetbuf);

	// finds and classifies next packet header in the data stream
	virtual int get_next_packet(char* packetbuf, int bufsize);

	// parses a complete packet with an identified type
	virtual int parse_packet(char* packetbuf, int packet_type);

	virtual int is_ascii_packet(int packet_type);

	virtual int parse_ack_packet(char* packetbuf, int& addr, int& val);

	virtual int extract_ascii_packet(int packet_type, char* rawpacket, int playback, void* timestamp_packet);
	virtual int extract_raw_packet(int packet_type, char* rawpacket, int has_seq);

	virtual int classify_packet_header(char* packetbuf, int header_length, int ascii_packet);

	virtual int read_device_info();
};

#endif