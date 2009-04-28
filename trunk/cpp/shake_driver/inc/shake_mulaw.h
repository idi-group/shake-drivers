#ifndef _SHAKE_MULAW_H_
#define _SHAKE_MULAW_H_

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

static const short MuLawDecompressTable[256] = { 
	-8159, -7903, -7647, -7391, -7135, -6879, -6623, -6367, -
	6111, -5855, -5599, - 5343, -5087, -4831, -4575, -4319, -
	4063, -3935, -3807, -3679, -3551, -3423, - 3295, -3167, -
	3039, -2911, -2783, -2655, -2527, -2399, -2271, -2143, -
	2015, - 1951, -1887, -1823, -1759, -1695, -1631, -1567, -
	1503, -1439, -1375, -1311, - 1247, -1183, -1119, -1055, -
	991, -959, -927, -895, -863, -831, -799, -767, -735, -703, -
	671, -639, -607, -575, -543, -511, -479, -463, -447, -431, -
	415, -399, - 383, -367, -351, -335, -319, -303, -287, -271, 
	-255, -239, -223, -215, -207, - 199, -191, -183, -175, -167, 
	-159, -151, -143, -135, -127, -119, -111, -103, - 95, -91, -
	87, -83, -79, -75, -71, -67, -63, -59, -55, -51, -47, -43, -
	39, -35, - 31, -29, -27, -25, -23, -21, -19, -17, -15, -13, 
	-11, -9, -7, -5, -3, -1, 7903, 7647, 7391, 7135, 6879, 6623, 
	6367, 6111, 5855, 5599, 5343, 5087, 4831, 4575, 4319, 4063, 
	3935, 3807, 3679, 3551, 3423, 3295, 3167, 3039, 2911, 2783, 
	2655, 2527, 2399, 2271, 2143, 2015, 1951, 1887, 1823, 1759, 
	1695, 1631, 1567, 1503, 1439, 1375, 1311, 1247, 1183, 1119, 
	1055, 991, 959, 927, 895, 863, 831, 799, 767, 735, 703, 671, 
	639, 607, 575, 543, 511, 479, 463, 447, 431, 415, 399, 383, 
	367, 351, 335, 319, 303, 287, 271, 255, 239, 223, 215, 207, 
	199, 191, 183, 175, 167, 159, 151, 143, 135, 127, 119, 111, 
	103, 95, 91, 87, 83, 79, 75, 71, 67, 63, 
	59, 55, 51, 47, 43, 39, 35, 31, 29, 27, 25, 23, 21, 19, 17, 
	15, 13, 11, 9, 7, 5, 3, 1, 0
};

static const int cBias = 0x84;
static const int cClip = 32635;

static char MuLawCompressTable[256] =
{
     0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,
     4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
     5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static unsigned char LinearToMuLawSample(short sample)
{
	int sign = (sample >> 8) & 0x80;
	if (sign)
		sample = (short)-sample;

	if (sample > cClip)
		sample = cClip;

	sample = (short)(sample + cBias);
	
	int exponent = (int)MuLawCompressTable[(sample>>7) & 0xFF];
	int mantissa = (sample >> (exponent+3)) & 0x0F;
	int compressedByte = ~ (sign | (exponent << 4) | mantissa);

	return (unsigned char)compressedByte;
} 

#endif

