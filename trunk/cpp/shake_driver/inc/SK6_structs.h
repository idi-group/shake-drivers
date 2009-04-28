#ifndef _SK6_STRUCTS_H_
#define _SK6_STRUCTS_H_

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

/*	one of these structs created per sk6 device, holds all sensor data in numeric form for
*	easy access by application code. The values in the struct will be updated as new data packets
*	come in from the device. */
typedef struct {
	short accx, accy, accz;
	short gyrx, gyry, gyrz;
	short magx, magy, magz;
	short heading;
	short cap0, cap1;
	short ana0, ana1;
	short shaking_peak_accel, shaking_direction, shaking_timestamp;
	short hr_bpm;

	/* sequence numbers from the various types of data packets */
	unsigned char internal_timestamps[8]; //accseq, gyrseq, magseq, hdgseq, cap0seq, cap1seq, ana0seq, ana1seq;
	unsigned char sk6seq, hrseq;

	/*	these sequence numbers are linked to the data access functions, so that an application 
	*	can tell which packet the data it read came from. */
	unsigned char timestamps[8];
} sk6_sensor_data;

#endif