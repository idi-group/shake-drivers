#ifndef _SK7_PACKETS_
#define _SK7_PACKETS_

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

#define SK7_DEFAULT_CHECKSUM 0x00

#define SK7_MAX_PACKET_SIZE 	64
#define SK7_NUM_PACKET_TYPES	64

#define SK7_HEADER_LEN		4	// "$pid," == 4 chars
#define SK7_RAW_HEADER_LEN	3
#define SK7_PACKET_ID_LEN		3


/* packet types */
enum sk7_packet_types {
	/* ASCII packets */

	/* data packets first */
	SK7_DATA_ACC = 0,					// accelerometer
	SK7_DATA_GYRO,					// angular rate (ie gyro readings)
	SK7_DATA_MAG, 					// magnetometer
	SK7_DATA_HEADING,					// compass heading
	SK7_DATA_CAP,					// capacitive sensors
	SK7_DATA_CAP_B,	
	SK7_DATA_CAP_C,
	SK7_DATA_ANA0,					// analog input 0
	SK7_DATA_ANA1,					// analog input 1
	SK7_DATA_RPH,					// roll-pitch-heading

	SK7_DATA_NVU,						// nav switch up
	SK7_DATA_NVD,						// nav switch down
	SK7_DATA_NVC,						// nav switch centre
	SK7_DATA_NVN,						// nav switch release

	SK7_DATA_CU0,						// cap switch 0 upper threshold trigger
	SK7_DATA_CL0,						// cap switch 0 lower threshold trigger
	SK7_DATA_CU1,						// cap switch 1 upper threshold trigger
	SK7_DATA_CL1,						// cap switch 1 lower threshold trigger
	SK7_DATA_CU2,						// cap switch 2 upper threshold trigger
	SK7_DATA_CL2,						// cap switch 2 lower threshold trigger
	SK7_DATA_CU3,						// cap switch 3 upper threshold trigger
	SK7_DATA_CL3,						// cap switch 3 lower threshold trigger
	SK7_DATA_CU4,						// cap switch 3 upper threshold trigger
	SK7_DATA_CL4,						// cap switch 4 lower threshold trigger
	SK7_DATA_CU5,						// cap switch 5 upper threshold trigger
	SK7_DATA_CL5,						// cap switch 5 lower threshold trigger
	SK7_DATA_CU6,						// cap switch 6 upper threshold trigger
	SK7_DATA_CL6,						// cap switch 6 lower threshold trigger
	SK7_DATA_CU7,						// cap switch 7 upper threshold trigger
	SK7_DATA_CL7,						// cap switch 7 lower threshold trigger
	SK7_DATA_CU8,						// cap switch 8 upper threshold trigger
	SK7_DATA_CL8,						// cap switch 8 lower threshold trigger
	SK7_DATA_CU9,						// cap switch 9 upper threshold trigger
	SK7_DATA_CL9,						// cap switch 9 lower threshold trigger
	SK7_DATA_CUA,						// cap switch 10 upper threshold trigger
	SK7_DATA_CLA,						// cap switch 10 lower threshold trigger
	SK7_DATA_CUB,						// cap switch 11 upper threshold trigger
	SK7_DATA_CLB,						// cap switch 11 lower threshold trigger

	SK7_DATA_RPH_QUATERNION,			// RPH data, quaternion format

	SK7_DATA_GYRO_TEMP,					// Gyro temperature data

	SK7_DATA_TIMESTAMP,				// timestamped logged data 
	SK7_DATA_PLAYBACK_COMPLETE,		// playback completed (special packet format)

	SK7_DATA_RFID_TID,				// RFID TID packet

	SK7_DATA_SHAKING,					// events from shaking detection algorithm
	SK7_DATA_HEART_RATE,				// events from heart rate monitor
	
	/* command packets */
	SK7_CMD_READ, 					// read command
	SK7_CMD_WRITE, 					// write command
	
	/* acknowledgement packets */
	SK7_ACK_ACK,						// successful ACK
	SK7_ACK_NEG, 						// negative ACK (ie an error occurred)

	SK7_STARTUP_INFO,

	/* Raw packets */
	SK7_RAW_DATA_ACC,
	SK7_RAW_DATA_GYRO,
	SK7_RAW_DATA_MAG,
	SK7_RAW_DATA_HEADING,
	SK7_RAW_DATA_CAP,
	SK7_RAW_DATA_CAP_B,
	SK7_RAW_DATA_CAP_C,
	SK7_RAW_DATA_ANALOG0,
	SK7_RAW_DATA_ANALOG1,
	SK7_RAW_DATA_EVENT,
	SK7_RAW_DATA_SHAKING,
	SK7_RAW_DATA_RPH,
	SK7_RAW_DATA_RPH_QUATERNION,
	SK7_RAW_DATA_GYRO_TEMP,

	// TODO missing raw packet types!
};

// TODO UPDATE (only for debugging anyway)
static char* sk7_packet_type_names[] = {
	"ASCII/Acc",
	"ASCII/Gyro",
	"ASCII/Mag",
	"ASCII/Heading",
	"ASCII/Cap0",
	"ASCII/Cap1",
	"ASCII/Ana0",
	"ASCII/Ana1",
	"ASCII/RPH",
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
	"Binary/RPH",
	"Binary/AudioSTANE",
	"Binary/AudioHeader",
	"Binary/Audio",
};

/* packet headers (these directly follow the common header above) */
static char* sk7_packet_headers[] = {
	"$ACC",		// data; accelerometer
	"$ARS",		// data; angular rate (gyro)
	"$MAG", 	// data; magnetometer
	"$HED", 	// data; heading
	"$CSA",		// data; capacitive sensors
	"$CSB",		// data; external capacitive sensor
	"$CSC",		// data; external capacitive sensor
	"$AI0",		// data; analog input 0
	"$AI1",		// data; analog input 1
	"$RPH",		// data; roll-pitch-heading

	"$NVU",		// data; nav switch up
	"$NVD",		// data; nav switch down
	"$NVC",		// data; nav switch centre
	"$NVN",		// data; nav switch released

	"$CU0",		// data; cap switch 0; increasing threshold trigger
	"$CL0",		// data; cap switch 0; decreasing threshold trigger
	"$CU1",		// data; cap switch 1; increasing threshold trigger
	"$CL1",		// data; cap switch 1; decreasing threshold trigger
	"$CU2",		// data; cap switch 2; increasing threshold trigger
	"$CL2",		// data; cap switch 2; decreasing threshold trigger
	"$CU3",		// data; cap switch 3; increasing threshold trigger
	"$CL3",		// data; cap switch 3; decreasing threshold trigger
	"$CU4",		// data; cap switch 4; increasing threshold trigger
	"$CL4",		// data; cap switch 4; decreasing threshold trigger
	"$CU5",		// data; cap switch 5; increasing threshold trigger
	"$CL5",		// data; cap switch 5; decreasing threshold trigger
	"$CU6",		// data; cap switch 6; increasing threshold trigger
	"$CL6",		// data; cap switch 6; decreasing threshold trigger
	"$CU7",		// data; cap switch 7; increasing threshold trigger
	"$CL7",		// data; cap switch 7; increasing threshold trigger
	"$CU8",		// data; cap switch 8; decreasing threshold trigger
	"$CL8",		// data; cap switch 8; increasing threshold trigger
	"$CU9",		// data; cap switch 9; decreasing threshold trigger
	"$CL9",		// data; cap switch 9; increasing threshold trigger
	"$CUA",		// data; cap switch 10; decreasing threshold trigger
	"$CLA",		// data; cap switch 10; increasing threshold trigger
	"$CUB",		// data; cap switch 11; decreasing threshold trigger
	"$CLB",		// data; cap switch 11; decreasing threshold trigger

	"$QTN",		// data; RPH quaternions
	"$GOT",		// data; gyro temperatures

	"$TIM",		// data; timestamped logged data packet
	"Logg",		// data; playback complete packet ("Logged Data Upload Complete.\r\n")

	"$TID",		// data; RFID TID packet

	"$SHK",		// data; shaking event detected
	"$HR ",		// data; heart rate monitor event

	"$REA",		// command; read
	"$WRI",		// command; write

	"$ACK",		// ack; positive
	"$NAK",		// ack; negative

	"\nSHA",		// SK7 startup info
};

static char sk7_raw_packet_headers[] = { 
	126,	// raw acc
	125,	// raw gyro
	124,	// raw mag
	123,	// raw compass
	122,	// raw cap
	115,	// raw cap (ext)
	114,	// raw cap (ext)
	120,	// raw ana0
	119,	// raw ana1
	118,	// nav events
	117,	// shaking events
	116,	// roll-pitch-heading
	110,	// roll-pitch-heading (quaternion mode)
	111,	// gyro temperature
};

// length of checksum part of packet in bytes
#define CHECKSUM_LENGTH			3

/* packet lengths (bytes), WITHOUT INCLUDING CHECKSUMS */
static int sk7_packet_lengths[] = {
	27, 		// data; accelerometer
	27,			// data; angular rate (gyro)
	27, 		// data; magnetometer
	14, 		// data; heading
	45, 		// data; capacitive sensors
	45, 		// data; capacitive sensors (ext)
	45, 		// data; capacitive sensors (ext)
	14,			// data: analog input 0
	14,			// data; analog input 1
	27,			// data; roll-pitch-heading

	6,			// data; nav switch up
	6,			// data; nav switch down
	6,			// data; nav switch centre
	6,			// data; nav switch released

	6,			// data; cap switch 0; increasing threshold trigger
	6,			// data; cap switch 0; decreasing threshold trigger
	6,			// data; cap switch 1; increasing threshold trigger
	6,			// data; cap switch 1; decreasing threshold trigger
	6,			// data; cap switch 2; increasing threshold trigger
	6,			// data; cap switch 2; decreasing threshold trigger
	6,			// data; cap switch 3; increasing threshold trigger
	6,			// data; cap switch 3; increasing threshold trigger
	6,			// data; cap switch 4; decreasing threshold trigger
	6,			// data; cap switch 4; increasing threshold trigger
	6,			// data; cap switch 5; increasing threshold trigger
	6,			// data; cap switch 5; decreasing threshold trigger
	6,			// data; cap switch 6; increasing threshold trigger
	6,			// data; cap switch 6; decreasing threshold trigger
	6,			// data; cap switch 7; increasing threshold trigger
	6,			// data; cap switch 7; decreasing threshold trigger
	6,			// data; cap switch 8; increasing threshold trigger
	6,			// data; cap switch 8; increasing threshold trigger
	6,			// data; cap switch 9; decreasing threshold trigger
	6,			// data; cap switch 9; increasing threshold trigger
	6,			// data; cap switch 10; decreasing threshold trigger
	6,			// data; cap switch 10; decreasing threshold trigger
	6,			// data; cap switch 11; decreasing threshold trigger
	6,			// data; cap switch 11; decreasing threshold trigger

	45,			// data; RPH quaternions
	27,			// data; gyro temperatures

	16,			// data; timestamped logged data packet (length corresponds to "$TIM,1234567890," prefix)
	31,			// data; playback complete packet ("Logged Data Upload Complete.\r\n")

	24,			// data; RFID TID packet (length corresponds to "$TID,aabbccddeeffgghh)

	27,			// data; shaking event detected
	14,			// data; heart rate monitor event

	14, 		// command; read
	14, 		// command; write

	14,			// ack; positive
	14, 		// ack; negative

	-1,			// special one: SK7 startup info

	10,			// raw data; accelerometer
	10,			// raw data; angular rate (gyro)
	10,			// raw data; magnetometer 
	6,			// raw data; heading
	15,			// raw data; capacitive sensors (note no timestamp/packet counter here)
	15,			// raw data; capacitive sensors (note no timestamp/packet counter here)
	15,			// raw data; capacitive sensors (note no timestamp/packet counter here)
	6,			// raw data; analog input 0
	6,			// raw data; analog input 1
	5,			// raw data; nav switch / cap switch events (never has timestamps/packet counter)
	9,			// raw data; shaking event detected
	10,			// raw data; RPH
	12,			// raw data; RPH (quaternion)
	12,			// raw data; gyro temperatures
};

// TODO UPDATE
/* indicates which packet types could have checksums */
static int sk7_packet_has_checksum[] = {
	1,1,1,1,1,1,1,1,1,1,	// these are the basic sensor output channels + RPH

	0,0,0,0,				// nav switch events
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,	// cap threshold events
	0,0,					// logger packets
	0,						// RFID packet
	1,						// shaking event
	1,						// heart rate event
	0,0,					// commands
	1,1,					// acks
	0,						// startup info
	0,0,0,0,0,0,0,0,0,0,0,0,0,0 // raw data packets
};

#ifdef _WIN32
#pragma pack(1)
#define PACKED
#else
#define PACKED __attribute__ ((__packed__))
#endif

/* packet components structures, used to make it easier to access the fields inside packets */
struct sk7_packet_header {
	char dollar ;
	char packet_id[3] ;
} PACKED;
typedef struct sk7_packet_header sk7_packet_header;

struct sk7_packet_header_simple {
	char dollar ;
	char packet_id[3] ;
} PACKED;
typedef struct sk7_packet_header_simple sk7_packet_header_simple;

struct sk7_packet_data_4d {
	char data[4] ;
} PACKED;
typedef struct sk7_packet_data_4d sk7_packet_data_4d;

struct sk7_packet_data_4ds {
	char data[5] ;
} PACKED;
typedef struct sk7_packet_data_4ds sk7_packet_data_4ds;

struct sk7_packet_data_4h {
	char data[4] ;
} PACKED;
typedef struct sk7_packet_data_4h sk7_packet_data_4h;

struct sk7_packet_data_2h {
	char data[2] ;	
} PACKED;
typedef struct sk7_packet_data_2h sk7_packet_data_2h;

struct sk7_packet_data_2d {
	char data[2] ;
} PACKED;
typedef struct sk7_packet_data_2d sk7_packet_data_2d;

struct sk7_packet_checksum {
	char checksum[2] ;
} PACKED;
typedef struct sk7_packet_checksum sk7_packet_checksum;

struct sk7_packet_terminator
{
	char terminator[2];
} PACKED;
typedef struct sk7_packet_terminator sk7_packet_terminator;

/* packets : with checksums */
struct sk7_data_acc_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds accx ;
	char sep2 ;
	sk7_packet_data_4ds accy ;
	char sep3 ;
	sk7_packet_data_4ds accz ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	char sep5 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_acc_packet sk7_data_acc_packet;

struct sk7_data_gyr_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds gyrx ;
	char sep2 ;
	sk7_packet_data_4ds gyry ;
	char sep3 ;
	sk7_packet_data_4ds gyrz ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	char sep5 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_gyr_packet sk7_data_gyr_packet;

struct sk7_data_mag_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds magx ;
	char sep2 ;
	sk7_packet_data_4ds magy ;
	char sep3 ;
	sk7_packet_data_4ds magz ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	char sep5 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_mag_packet sk7_data_mag_packet;

struct sk7_data_heading_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d heading ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	char sep3 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_heading_packet sk7_data_heading_packet;

struct sk7_data_cap_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d prox ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	char sep3 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_cap_packet sk7_data_cap_packet;

struct sk7_data_analog_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d voltage ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	char sep3 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_analog_packet sk7_data_analog_packet;

struct sk7_ack_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4h addr ;
	char sep2 ;
	sk7_packet_data_2d val ;
	char sep3 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_ack_packet sk7_ack_packet;

struct sk7_data_shake_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds peakaccel ;
	char sep2 ;
	sk7_packet_data_4ds direction ;
	char sep3 ;
	sk7_packet_data_4ds timestamp ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	char sep5 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_shake_packet sk7_data_shake_packet;

struct sk7_data_hr_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d heart_bpm ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	char sep3 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_hr_packet sk7_data_hr_packet;

struct sk7_data_rph_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds roll ;
	char sep2 ;
	sk7_packet_data_4ds pitch ;
	char sep3 ;
	sk7_packet_data_4ds heading ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	char sep5 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_rph_packet sk7_data_rph_packet;

struct sk7_data_rph_quaternion_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	char data[35];
	char sep4 ;
	sk7_packet_data_2d seq ;
	char sep5 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_rph_quaternion_packet sk7_data_rph_quaternion_packet;

struct sk7_data_gyro_temp_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds pitchtemp ;
	char sep2 ;
	sk7_packet_data_4ds rolltemp ;
	char sep3 ;
	sk7_packet_data_4ds yawtemp ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	char sep5 ;
	sk7_packet_checksum checksum ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_gyro_temp_packet sk7_data_gyro_temp_packet;

/* packets which never have checksums */

struct sk7_cmd_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4h addr ;
	char sep2 ;
	sk7_packet_data_2h dataval ;
	//sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_cmd_packet	sk7_cmd_packet;

struct sk7_nav_packet {
	sk7_packet_header_simple hdr ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_nav_packet sk7_nav_packet;

struct sk7_raw_packet_long {
	char fixed_header[2] ;
	char packet_header ;
	unsigned char data[6] ;
	unsigned char seq;
} PACKED;
typedef struct sk7_raw_packet_long sk7_raw_packet_long;

struct sk7_raw_packet_extra_long {
	char fixed_header[2] ;
	char packet_header ;
	unsigned char data[8] ;
	unsigned char seq;
} PACKED;
typedef struct sk7_raw_packet_extra_long sk7_raw_packet_extra_long;

struct sk7_raw_packet_short {
	char fixed_header[2] ;
	char packet_header ;
	unsigned char data[2] ;
	unsigned char seq;
} PACKED;
typedef struct sk7_raw_packet_short sk7_raw_packet_short;

struct sk7_raw_packet_audio {
	char fixed_header[2] ;
	char packet_header ;
	unsigned char data[SHAKE_AUDIO_DATA_LEN];
} PACKED;
typedef struct sk7_raw_packet_audio sk7_raw_packet_audio;

/* packets which may not have checksums */
struct sk7_data_acc_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds accx ;
	char sep2 ;
	sk7_packet_data_4ds accy ;
	char sep3 ;
	sk7_packet_data_4ds accz ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_acc_packet_no_checksum sk7_data_acc_packet_no_checksum;

struct sk7_data_gyr_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds gyrx ;
	char sep2 ;
	sk7_packet_data_4ds gyry ;
	char sep3 ;
	sk7_packet_data_4ds gyrz ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_gyr_packet_no_checksum sk7_data_gyr_packet_no_checksum;

struct sk7_data_mag_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds magx ;
	char sep2 ;
	sk7_packet_data_4ds magy ;
	char sep3 ;
	sk7_packet_data_4ds magz ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_mag_packet_no_checksum sk7_data_mag_packet_no_checksum;

struct sk7_data_heading_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d heading ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_heading_packet_no_checksum sk7_data_heading_packet_no_checksum;

struct sk7_data_cap_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d prox ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_cap_packet_no_checksum sk7_data_cap_packet_no_checksum;

struct sk7_data_analog_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d voltage ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_analog_packet_no_checksum sk7_data_analog_packet_no_checksum;

struct sk7_ack_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4h addr ;
	char sep2 ;
	sk7_packet_data_2d val ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_ack_packet_no_checksum sk7_ack_packet_no_checksum;

struct sk7_data_timestamp_packet {
	sk7_packet_header hdr ;
	char sep1 ;
	char timestamp[10];
	char sep2 ;
} PACKED;
typedef struct sk7_data_timestamp_packet sk7_data_timestamp_packet;

struct sk7_data_rfid_tid_packet {
	sk7_packet_header hdr;
	char sep1;
	char tid[SHAKE_RFID_TAG_LENGTH];
} PACKED;
typedef struct sk7_data_rfid_tid_packet sk7_data_rfid_tid_packet;

struct sk7_data_shake_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4ds peakaccel ;
	char sep2 ;
	sk7_packet_data_4ds direction ;
	char sep3 ;
	sk7_packet_data_4ds timestamp ;
	char sep4 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_shake_packet_no_checksum sk7_data_shake_packet_no_checksum;

struct sk7_data_hr_packet_no_checksum {
	sk7_packet_header hdr ;
	char sep1 ;
	sk7_packet_data_4d heart_bpm ;
	char sep2 ;
	sk7_packet_data_2d seq ;
	sk7_packet_terminator term ;
} PACKED;
typedef struct sk7_data_hr_packet_no_checksum sk7_data_hr_packet_no_checksum;

#ifdef _WIN32
#pragma pack()
#else

#endif

// extracts the address and value from an ack packet
int sk7_parse_ack_packet(sk7_ack_packet* scp, int* addr, int* value);

// Attempts to classify the packet in <packet_data> by examining the header.
// <packet_length> should be the length of <packet_data> in bytes. Returns
// SK7_BAD_PKT if unable to determine the type of the packet or if <packet_length>
// is too short. Otherwise, returns a member of the "packet_types" enumeration
// defined above.
int sk7_classify_packet_header(char* header, unsigned header_length, int ascii_mode);

int sk7_update_data(void* sk7dev, int type, char* rawpacket, BOOL playback, sk7_data_timestamp_packet* tspkt);
int sk7_update_data_no_checksum(void* sk7dev, int type, char* rawpacket, BOOL playback, sk7_data_timestamp_packet* tspkt);
int sk7_update_data_raw(void* sk7dev, int type, char* rawpacket, BOOL has_seq);

#ifdef __cplusplus
}
#endif

#endif /* _SK7_PACKETS_ */
