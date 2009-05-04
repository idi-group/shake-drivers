#ifndef _SHAKE_H_
#define _SHAKE_H_

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

#include "shake_platform.h"

typedef struct {
	short accx, accy, accz;
	short gyrx, gyry, gyrz;
	short magx, magy, magz;
	short heading;
	union  {
		int cap_sk6[2];	// SK6
		int cap_sk7[12];	// SK7
	};
	short ana0, ana1;
	short shaking_peak_accel, shaking_direction, shaking_timestamp;
	short hr_bpm;

	/* sequence numbers from the various types of data packets */
	unsigned char internal_timestamps[8]; 
	unsigned char sk6seq, hrseq;
	unsigned short sk7seq;

	/*	these sequence numbers are linked to the data access functions, so that an application 
	*	can tell which packet the data it read came from. */
	unsigned char timestamps[8];
} sk_sensor_data;

#include "shake_driver.h"
#include "shake_structs.h"

// defines functionality common to SK6 and SK7
class SHAKE
{
public:
	SHAKE(shake_device* sd, shake_device_private* sdp);
	virtual ~SHAKE(void);

	int device_type;			// type of device (SK6/SK7)
	sk_sensor_data data;		// contains all sensor data

	shake_device* dev;
	shake_device_private* devpriv;

	// parses a complete ASCII packet
	virtual int parse_ascii_packet(int packet_type, char* packetbuf, int packetlen, int playback, void* timestamp_packet) = 0;

	// reads a complete ASCII packet (minus header)
	virtual int read_ascii_packet(int packet_type, char* packetbuf) = 0;

	// parses a complete raw packet
	virtual int parse_raw_packet(int packet_type, char* packetbuf, int packetlen, int has_seq) = 0;

	// reads a complete raw packet (minus header)
	virtual int read_raw_packet(int packet_type, char* packetbuf) = 0;

	// finds and classifies next packet header in the data stream
	virtual int get_next_packet(char* packetbuf, int bufsize) = 0;

	// parses a complete packet with an identified type
	virtual int parse_packet(char* packetbuf, int packet_type) = 0;

	virtual int is_ascii_packet(int packet_type) = 0;

	virtual int parse_ack_packet(char* packetbuf, int& addr, int& val) = 0;

	virtual int extract_ascii_packet(int packet_type, char* rawpacket, int playback, void* timestamp_packet) = 0;
	virtual int extract_raw_packet(int packet_type, char* rawpacket, int has_seq) = 0;

	virtual int classify_packet_header(char* packetbuf, int header_length, int ascii_packet) = 0;

	virtual int read_device_info() = 0;
};

#endif
