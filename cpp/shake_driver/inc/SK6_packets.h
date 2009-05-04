#ifndef _SK6_PACKETS_
#define _SK6_PACKETS_

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
#include "shake_thread.h"
#include "shake_mulaw.h"

#include "shake_registers.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SK6_DEFAULT_CHECKSUM 0x00

#define SK6_MAX_PACKET_SIZE 	64
#define SK6_NUM_PACKET_TYPES	39

#define SK6_HEADER_LEN		4	// "$pid," == 4 chars
#define SK6_RAW_HEADER_LEN	3
#define SK6_PACKET_ID_LEN		3

/* packet types */
enum sk6_packet_types {
	/* ASCII packets */

	/* data packets first */
	SK6_DATA_ACC = 0,					// accelerometer
	SK6_DATA_GYRO,					// angular rate (ie gyro readings)
	SK6_DATA_MAG, 					// magnetometer
	SK6_DATA_HEADING,					// compass heading
	SK6_DATA_CAP0,					// capacitive sensor 0
	SK6_DATA_CAP1,					// capacitive sensor 1
	SK6_DATA_ANA0,					// analog input 0
	SK6_DATA_ANA1,					// analog input 1

	SK6_DATA_NVU,						// nav switch up
	SK6_DATA_NVD,						// nav switch down
	SK6_DATA_NVC,						// nav switch centre
	SK6_DATA_NVN,						// nav switch release

	SK6_DATA_CU0,						// cap switch 0 upper threshold trigger
	SK6_DATA_CL0,						// cap switch 0 lower threshold trigger
	SK6_DATA_CU1,						// cap switch 1 upper threshold trigger
	SK6_DATA_CL1,						// cap switch 1 lower threshold trigger

	SK6_DATA_TIMESTAMP,				// timestamped logged data 
	SK6_DATA_PLAYBACK_COMPLETE,		// playback completed (special packet format)

	SK6_DATA_RFID_TID,				// RFID TID packet

	SK6_DATA_SHAKING,					// events from shaking detection algorithm
	SK6_DATA_HEART_RATE,				// events from heart rate monitor
	
	/* command packets */
	SK6_CMD_READ, 					// read command
	SK6_CMD_WRITE, 					// write command
	
	/* acknowledgement packets */
	SK6_ACK_ACK,						// successful ACK
	SK6_ACK_NEG, 						// negative ACK (ie an error occurred)

	SK6_STARTUP_INFO,

	/* Raw packets */
	SK6_RAW_DATA_ACC,
	SK6_RAW_DATA_GYRO,
	SK6_RAW_DATA_MAG,
	SK6_RAW_DATA_HEADING,
	SK6_RAW_DATA_CAP0,
	SK6_RAW_DATA_CAP1,
	SK6_RAW_DATA_ANALOG0,
	SK6_RAW_DATA_ANALOG1,
	SK6_RAW_DATA_EVENT,
	SK6_RAW_DATA_SHAKING,
	SK6_RAW_DATA_AUDIO_EXP, 
	SK6_RAW_DATA_AUDIO_HEADER,
	SK6_RAW_DATA_AUDIO,
};

static char* sk6_packet_type_names[] = {
	"ASCII/Acc",
	"ASCII/Gyro",
	"ASCII/Mag",
	"ASCII/Heading",
	"ASCII/Cap0",
	"ASCII/Cap1",
	"ASCII/Ana0",
	"ASCII/Ana1",
	"ASCII/NavUp",
	"ASCII/NavDown",
	"ASCII/NavCentre",
	"ASCII/NavRelease",
	"ASCII/Cap0Upper",
	"ASCII/Cap0Lower",
	"ASCII/Cap1Upper",
	"ASCII/Cap1Lower",
	"ASCII/Timestamp",
	"ASCII/PlaybackComplete",
	"ASCII/RFID",
	"ASCII/Shaking",
	"ASCII/HeartRate",
	"ASCII/CmdRead",
	"ASCII/CmdWrite",
	"ASCII/ACK",
	"ASCII/NAK",
	"ASCII/StartupInfo",
	"Binary/Acc",
	"Binary/Gyro",
	"Binary/Mag",
	"Binary/Heading",
	"Binary/Cap0",
	"Binary/Cap1",
	"Binary/Analog0",
	"Binary/Analog1",
	"Binary/Event",
	"Binary/Shaking",
	"Binary/AudioSTANE",
	"Binary/AudioHeader",
	"Binary/Audio",
};

/* packet headers (these directly follow the common header above) */
static char* sk6_packet_headers[] = {
	"$ACC",		// data; accelerometer
	"$ARS",		// data; angular rate (gyro)
	"$MAG", 	// data; magnetometer
	"$HED", 	// data; heading
	"$CS0",		// data; capacitive sensor 0
	"$CS1",		// data; capacitive sensor 1
	"$AI0",		// data; analog input 0
	"$AI1",		// data; analog input 1

	"$NVU",		// data; nav switch up
	"$NVD",		// data; nav switch down
	"$NVC",		// data; nav switch centre
	"$NVN",		// data; nav switch released

	"$CU0",		// data; cap switch 0; increasing threshold trigger
	"$CL0",		// data; cap switch 0; decreasing threshold trigger
	"$CU1",		// data; cap switch 1; increasing threshold trigger
	"$CL1",		// data; cap switch 1; decreasing threshold trigger

	"$TIM",		// data; timestamped logged data packet
	"Logg",		// data; playback complete packet ("Logged Data Upload Complete.\r\n")

	"$TID",		// data; RFID TID packet

	"$SHK",		// data; shaking event detected
	"$HR ",		// data; heart rate monitor event

	"$REA",		// command; read
	"$WRI",		// command; write

	"$ACK",		// ack; positive
	"$NAK",		// ack; negative

	"\nSHA",		// SK6 startup info
};

static char sk6_raw_packet_headers[] = { 
	126,	// raw acc
	125,	// raw gyro
	124,	// raw mag
	123,	// raw compass
	122,	// raw cap0
	121,	// raw cap1
	120,	// raw ana0
	119,	// raw ana1
	118,	// nav events
	117,	// shaking events
	114,	// expansion module audio
	113,	// audio (headers)
	112,	// audio (data packets)
};

// length of checksum part of packet in bytes
#define CHECKSUM_LENGTH			3

/* packet lengths (bytes), WITHOUT INCLUDING CHECKSUMS */
static unsigned sk6_packet_lengths[] = {
	27, 		// data; accelerometer
	27,			// data; angular rate (gyro)
	27, 		// data; magnetometer
	14, 		// data; heading
	14, 		// data; capacitive sensor 0
	14,			// data; capacitive sensor 1
	14,			// data: analog input 0
	14,			// data; analog input 1

	6,			// data; nav switch up
	6,			// data; nav switch down
	6,			// data; nav switch centre
	6,			// data; nav switch released

	6,			// data; cap switch 0; increasing threshold trigger
	6,			// data; cap switch 0; decreasing threshold trigger
	6,			// data; cap switch 1; increasing threshold trigger
	6,			// data; cap switch 1; decreasing threshold trigger

	16,			// data; timestamped logged data packet (length corresponds to "$TIM,1234567890," prefix)
	31,			// data; playback complete packet ("Logged Data Upload Complete.\r\n")

	24,			// data; RFID TID packet (length corresponds to "$TID,aabbccddeeffgghh)

	27,			// data; shaking event detected
	14,			// data; heart rate monitor event

	14, 		// command; read
	14, 		// command; write

	14,			// ack; positive
	14, 		// ack; negative

	-1,			// special one: SK6 startup info

	10,			// raw data; accelerometer
	10,			// raw data; angular rate (gyro)
	10,			// raw data; magnetometer 
	6,			// raw data; heading
	6,			// raw data; capacitive sensor 0
	6,			// raw data; capacitive sensor 1
	6,			// raw data; analog input 0
	6,			// raw data; analog input 1
	5,			// raw data; nav switch / cap switch events
	9,			// raw data; shaking event detected
	35,			// raw data; audio sample (extension module contact mic)
	3,			// raw data: audio header (for playback flow control)
	35,			// raw data; (audio sample)
};

/* indicates which packet types could have checksums */
static int sk6_packet_has_checksum[] = {
	1,1,1,1,1,1,1,1,		// these are the 8 basic sensor output channels

	0,0,0,0,0,0,0,0,		// nav switch and cap switch events
	0,0,					// logger packets
	0,						// RFID packet
	1,						// shaking event
	1,						// heart rate event
	0,0,					// commands
	1,1,					// acks
	0,0,0,0,0,0,0,0,0,0,0,0,0	// raw data packets
};

#ifdef _WIN32
#pragma pack(1)
#define PACKED
#else
#define PACKED __attribute__ ((__packed__))
#endif

/* packet components structures, used to make it easier to access the fields inside packets */
struct sk6_packet_header {
	char dollar ;
	char packet_id[3] ;
} PACKED;
typedef struct sk6_packet_header sk6_packet_header;

struct sk6_packet_header_simple {
	char dollar ;
	char packet_id[3] ;
} PACKED;
typedef struct sk6_packet_header_simple sk6_packet_header_simple;

struct sk6_packet_data_4d {
	char data[4] ;
} PACKED;
typedef struct sk6_packet_data_4d sk6_packet_data_4d;

struct sk6_packet_data_4ds {
	char data[5] ;
} PACKED;
typedef struct sk6_packet_data_4ds sk6_packet_data_4ds;

struct sk6_packet_data_4h {
	char data[4] ;
} PACKED;
typedef struct sk6_packet_data_4h sk6_packet_data_4h;

struct sk6_packet_data_2h {
	char data[2] ;	
} PACKED;
typedef struct sk6_packet_data_2h sk6_packet_data_2h;

struct sk6_packet_data_2d {
	char data[2] ;
} PACKED;
typedef struct sk6_packet_data_2d sk6_packet_data_2d;

struct sk6_packet_checksum {
	char checksum[2] ;
} PACKED;
typedef struct sk6_packet_checksum sk6_packet_checksum;

struct sk6_packet_terminator
{
	char terminator[2];
} PACKED;
typedef struct sk6_packet_terminator sk6_packet_terminator;

/* packets : with checksums */
struct sk6_data_acc_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds accx ;
	char sep2 ;
	sk6_packet_data_4ds accy ;
	char sep3 ;
	sk6_packet_data_4ds accz ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	char sep5 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_acc_packet sk6_data_acc_packet;

struct sk6_data_gyr_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds gyrx ;
	char sep2 ;
	sk6_packet_data_4ds gyry ;
	char sep3 ;
	sk6_packet_data_4ds gyrz ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	char sep5 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_gyr_packet sk6_data_gyr_packet;

struct sk6_data_mag_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds magx ;
	char sep2 ;
	sk6_packet_data_4ds magy ;
	char sep3 ;
	sk6_packet_data_4ds magz ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	char sep5 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_mag_packet sk6_data_mag_packet;

struct sk6_data_heading_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d heading ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	char sep3 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_heading_packet sk6_data_heading_packet;

struct sk6_data_cap_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d prox ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	char sep3 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_cap_packet sk6_data_cap_packet;

struct sk6_data_analog_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d voltage ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	char sep3 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_analog_packet sk6_data_analog_packet;

struct sk6_ack_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4h addr ;
	char sep2 ;
	sk6_packet_data_2d val ;
	char sep3 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_ack_packet sk6_ack_packet;

struct sk6_data_shake_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds peakaccel ;
	char sep2 ;
	sk6_packet_data_4ds direction ;
	char sep3 ;
	sk6_packet_data_4ds timestamp ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	char sep5 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_shake_packet sk6_data_shake_packet;

struct sk6_data_hr_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d heart_bpm ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	char sep3 ;
	sk6_packet_checksum checksum ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_hr_packet sk6_data_hr_packet;

/* packets which never have checksums */

struct sk6_cmd_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4h addr ;
	char sep2 ;
	sk6_packet_data_2h dataval ;
	//sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_cmd_packet	sk6_cmd_packet;

struct sk6_nav_packet {
	sk6_packet_header_simple hdr ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_nav_packet sk6_nav_packet;

struct sk6_raw_packet_long {
	char fixed_header[2] ;
	char packet_header ;
	unsigned char data[6] ;
	unsigned char seq;
} PACKED;
typedef struct sk6_raw_packet_long sk6_raw_packet_long;

struct sk6_raw_packet_short {
	char fixed_header[2] ;
	char packet_header ;
	unsigned char data[2] ;
	unsigned char seq;
} PACKED;
typedef struct sk6_raw_packet_short sk6_raw_packet_short;

struct sk6_raw_packet_audio {
	char fixed_header[2] ;
	char packet_header ;
	unsigned char data[SHAKE_AUDIO_DATA_LEN];
} PACKED;
typedef struct sk6_raw_packet_audio sk6_raw_packet_audio;

/* packets which may not have checksums */
struct sk6_data_acc_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds accx ;
	char sep2 ;
	sk6_packet_data_4ds accy ;
	char sep3 ;
	sk6_packet_data_4ds accz ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_acc_packet_no_checksum sk6_data_acc_packet_no_checksum;

struct sk6_data_gyr_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds gyrx ;
	char sep2 ;
	sk6_packet_data_4ds gyry ;
	char sep3 ;
	sk6_packet_data_4ds gyrz ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_gyr_packet_no_checksum sk6_data_gyr_packet_no_checksum;

struct sk6_data_mag_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds magx ;
	char sep2 ;
	sk6_packet_data_4ds magy ;
	char sep3 ;
	sk6_packet_data_4ds magz ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_mag_packet_no_checksum sk6_data_mag_packet_no_checksum;

struct sk6_data_heading_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d heading ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_heading_packet_no_checksum sk6_data_heading_packet_no_checksum;

struct sk6_data_cap_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d prox ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_cap_packet_no_checksum sk6_data_cap_packet_no_checksum;

struct sk6_data_analog_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d voltage ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_analog_packet_no_checksum sk6_data_analog_packet_no_checksum;

struct sk6_ack_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4h addr ;
	char sep2 ;
	sk6_packet_data_2d val ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_ack_packet_no_checksum sk6_ack_packet_no_checksum;

struct sk6_data_timestamp_packet {
	sk6_packet_header hdr ;
	char sep1 ;
	char timestamp[8];
	char sep2 ;
} PACKED;
typedef struct sk6_data_timestamp_packet sk6_data_timestamp_packet;

struct sk6_data_rfid_tid_packet {
	sk6_packet_header hdr;
	char sep1;
	char tid[SHAKE_RFID_TAG_LENGTH];
} PACKED;
typedef struct sk6_data_rfid_tid_packet sk6_data_rfid_tid_packet;

struct sk6_data_shake_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4ds peakaccel ;
	char sep2 ;
	sk6_packet_data_4ds direction ;
	char sep3 ;
	sk6_packet_data_4ds timestamp ;
	char sep4 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_shake_packet_no_checksum sk6_data_shake_packet_no_checksum;

struct sk6_data_hr_packet_no_checksum {
	sk6_packet_header hdr ;
	char sep1 ;
	sk6_packet_data_4d heart_bpm ;
	char sep2 ;
	sk6_packet_data_2d seq ;
	sk6_packet_terminator term ;
} PACKED;
typedef struct sk6_data_hr_packet_no_checksum sk6_data_hr_packet_no_checksum;

#ifdef _WIN32
#pragma pack()
#else

#endif

// extracts the address and value from an ack packet
int sk6_parse_ack_packet(sk6_ack_packet* scp, int* addr, int* value);

// Attempts to classify the packet in <packet_data> by examining the header.
// <packet_length> should be the length of <packet_data> in bytes. Returns
// SK6_BAD_PKT if unable to determine the type of the packet or if <packet_length>
// is too short. Otherwise, returns a member of the "packet_types" enumeration
// defined above.
int sk6_classify_packet_header(char* header, unsigned header_length, int ascii_mode);

int sk6_update_data(void* sk6dev, int type, char* rawpacket, BOOL playback, sk6_data_timestamp_packet* tspkt);
int sk6_update_data_no_checksum(void* sk6dev, int type, char* rawpacket, BOOL playback, sk6_data_timestamp_packet* tspkt);
int sk6_update_data_raw(void* sk6dev, int type, char* rawpacket, BOOL has_seq);

#ifdef __cplusplus
}
#endif

#endif /* _SK6_PACKETS_ */
