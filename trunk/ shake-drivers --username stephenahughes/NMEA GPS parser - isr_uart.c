
/**********************************************************************
*
* FileName:        isr_uart.c
*
**********************************************************************/

#include "common.h"

/*---------------------------------------------------------------------
  Function Name: _U1RXInterrupt
  Description:   UART1 Receive Interrupt Handler
  Inputs:        None
  Returns:       None
-----------------------------------------------------------------------*/
/*
This function decodes NMEA GGA message received over serial port.
Input:
	   $GPGGA,114019.489,5048.9524,N,00511.8268,E,1,08,01.4,57.9,M,45.6,M,,*52
	   where lat/long can be a variable number of digits: from xxx.y over xxx.yyyy to xxxxx.yyyy
	   where height can be a variable number of digits: from x.y to xxxxx.y
Output:
	   gps_latitude_degrees[3],gps_longitude_degrees[3], where [0]*100 + [1]*10 + [2] = decimal degrees
	   gps_latitude_minutes[2],gps_longitude_minutes[2], where [0]*10 + [1] = decimal minutes
	   gps_latitude_decimal_seconds[4],gps_longitude_decimal_seconds[4], where ([0]*1000 + [1]*100 + [2]*10 + [3]) / 10000 * 60 = decimal seconds
	   gps_height[5], gps_height_decimeters, where ([0]*10000 + [1]*1000 + [2]*100 + [3]*10 + [4]) + gps_height_decimeters/1° = decimal height
	   
No functions, subroutines or while loops are used, making the code lenghty, but probably faster
*/
	   
void __attribute__((__interrupt__,auto_psv)) _U1RXInterrupt (void)  // will be triggered on every character received from UART1
{
	// clear the interrupt flag to avoid looping here
	IFS0bits.U1RXIF = 0;   
	// there will only be 1 character, but check the "character received flag" in a while loop just in case  
	while (U1STAbits.URXDA)
	{
		// we will never have a buffer overrun, but in case we do clear the bit to unblock further reception
		U1STAbits.OERR = 0;
		// read the character from the buffer
		received_from_uart = U1RXREG ;
		// now try to see where this character fits in the NMEA GGA message we're trying to decipher
		switch ( nmea_state )               // evaluate expression 
		{
			case ( 1 ):                    // wait for 'G'
				if ( received_from_uart == 0x47) nmea_state++;
				break;                     
			case ( 2 ):                    // wait for 'G'
				if ( received_from_uart == 0x47) 
					nmea_state++;
				else
					nmea_state=1;
				break;                     
			case ( 3 ):                    // wait for 'A'
				if ( received_from_uart == 0x41) 
					nmea_state++;
				else
					nmea_state=1;
				break;                     
			case ( 4 ):                    // wait for ','
				if ( received_from_uart == 0x2c) nmea_state++;
				break;                     
			case ( 5 ):                    // wait for ','
				if ( received_from_uart == 0x2c) nmea_state++;
				break;                     
			case ( 6 ):                    // minimum digits before the next comma is 3: act as if this is the case
					gps_latitude_degrees[0]=0x30;
					gps_latitude_degrees[1]=0x30;
					gps_latitude_degrees[2]=received_from_uart;
					nmea_state++;
				break;                     
			case ( 7 ):                    // then this will be the first digit of the minutes
					gps_latitude_minutes[0]=received_from_uart;
					nmea_state++;
				break;                     
			case ( 8 ):                    // and this will be the second digit of the minutes
					gps_latitude_minutes[1]=received_from_uart;
					nmea_state++;
				break;                     
			case ( 9 ):                    // if another number follows then we need to do some shifting
				if ( received_from_uart == 0x2e) {          // waiting for a decimal point
					// no more numbers? OK, done with the shifting, move to the next step
					nmea_state++;
					// but first initialize the variable for the next round
					gps_latitude_decimal_seconds[0]=0x30;
					gps_latitude_decimal_seconds[1]=0x30;
					gps_latitude_decimal_seconds[2]=0x30;
					gps_latitude_decimal_seconds[3]=0x30;
					}
				else {
					// oh no, 1 more number! Just shift the digits around and we'll be fine
					gps_latitude_degrees[0]=gps_latitude_degrees[1];
					gps_latitude_degrees[1]=gps_latitude_degrees[2];
					gps_latitude_degrees[2]=gps_latitude_minutes[0];
					gps_latitude_minutes[0]=gps_latitude_minutes[1];
					gps_latitude_minutes[1]=received_from_uart;				
					}
				break;                     
			case ( 10 ):                    // now note the seconds, written as a decimal fraction of minutes
					// this is the most significant digit (it's after the comma)
					gps_latitude_decimal_seconds[0]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
				break;
			case ( 11 ):                    // now note the seconds, written as a decimal fraction of minutes
				if ( received_from_uart == 0x2c) {          // waiting for a comma
					// no more numbers? OK, done with the seconds, move to the next step
					nmea_state = 14;
					}
				else {
					// second digit
					gps_latitude_decimal_seconds[1]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
					}
				break;
			case ( 12 ):                    // now note the seconds, written as a decimal fraction of minutes
				if ( received_from_uart == 0x2c) {          // waiting for a comma
					// no more numbers? OK, done with the seconds, move to the next step
					nmea_state = 14;
					}
				else {
					// third digit
					gps_latitude_decimal_seconds[2]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
					}
				break;
			case ( 13 ):                    // now note the seconds, written as a decimal fraction of minutes
				if ( received_from_uart == 0x2c) {          // waiting for a comma
					// no more numbers? OK, done with the seconds, move to the next step
					nmea_state = 14;
					}
				else {
					// Least significatn digit
					gps_latitude_decimal_seconds[3]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
					}
				break;
			case (14):				// this determines the sign                     
				if ( received_from_uart == 0x53) // waiting for a 'N'
					gps_latitude_is_negative = 0; 
				else
					gps_latitude_is_negative = 1; 			
				nmea_state++;
				break;
			case (15):				// this is a comma                     
				nmea_state++;
				break;
			case (16):				// now store the longitude  :minimum digits before the next comma is 3                 
				gps_longitude_degrees[0]=0x30;
				gps_longitude_degrees[1]=0x30;
				gps_longitude_degrees[2]=received_from_uart;
				nmea_state++;
				break;
			case ( 17 ):                    // then this will be the first digit of the minutes
					gps_longitude_minutes[0]=received_from_uart;
					nmea_state++;
				break;                     
			case ( 18 ):                    // and this will be the second digit of the minutes
					gps_longitude_minutes[1]=received_from_uart;
					nmea_state++;
				break;                     
			case ( 19 ):                    // if another number follows then just add it
				if ( received_from_uart == 0x2e) {          // waiting for a decimal point
					// no more numbers? OK, done with the shifting, move to the next step
					nmea_state++;
					// but first initialize the variable for the next round
					gps_longitude_decimal_seconds[0]=0x30;
					gps_longitude_decimal_seconds[1]=0x30;
					gps_longitude_decimal_seconds[2]=0x30;
					gps_longitude_decimal_seconds[3]=0x30;
					}
				else {
					// oh no, 1 more number! Just shift the digits around and we'll be fine
					gps_longitude_degrees[0]=gps_longitude_degrees[1];
					gps_longitude_degrees[1]=gps_longitude_degrees[2];
					gps_longitude_degrees[2]=gps_longitude_minutes[0];
					gps_longitude_minutes[0]=gps_longitude_minutes[1];
					gps_longitude_minutes[1]=received_from_uart;				
					}
				break;                     
			case ( 20 ):                    // now note the seconds, written as a decimal fraction of minutes
					// this is the most significant digit (it's after the comma)
					gps_longitude_decimal_seconds[0]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
				break;
			case ( 21 ):                    // now note the seconds, written as a decimal fraction of minutes
				if ( received_from_uart == 0x2c) {          // waiting for a comma
					// no more numbers? OK, done with the seconds, move to the next step
					nmea_state = 24;
					}
				else {
					// second digit
					gps_longitude_decimal_seconds[1]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
					}
				break;
			case ( 22 ):                    // now note the seconds, written as a decimal fraction of minutes
				if ( received_from_uart == 0x2c) {          // waiting for a comma
					// no more numbers? OK, done with the seconds, move to the next step
					nmea_state = 24;
					}
				else {
					// 3rd digit
					gps_longitude_decimal_seconds[2]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
					}
				break;
			case ( 23 ):                    // now note the seconds, written as a decimal fraction of minutes
				if ( received_from_uart == 0x2c) {          // waiting for a comma
					// no more numbers? OK, done with the seconds, move to the next step
					nmea_state = 24;
					}
				else {
					// this is the least significant digit (it's after the comma)
					gps_longitude_decimal_seconds[3]=received_from_uart;				
					// write in the next location for the following character
					nmea_state++;
					}
				break;
			case (24):				// this determines the sign                     
				if ( received_from_uart == 0x45) // waiting for a 'E'
					gps_longitude_is_negative = 0; 
				else
					gps_longitude_is_negative = 1; 			
				nmea_state++;
				break;
			case (25):				// this is a comma                     
				nmea_state++;
				break;
			case (26):				// this is the 2nd comma                     
				if ( received_from_uart == 0x2C) nmea_state++;
				break;
			case (27):				// this is the 3th comma                     
				if ( received_from_uart == 0x2C) nmea_state++;
				break;
			case (28):				// this is the 4th comma                     
				if ( received_from_uart == 0x2C) {
					// move to the next state
					nmea_state++;
					// but first prepare the meters input variable
					gps_height[0]= 0x30;
					gps_height[1]= 0x30;
					gps_height[2]= 0x30;
					gps_height[3]= 0x30;
					gps_height[4]= 0x30;
					}
				break;
			case (29):				// this is the heigth in meters                     
				if ( received_from_uart == 0x2e) {          // waiting for a decimal point
					// no more numbers? OK, done with the shifting, move to the next step
					nmea_state++;
					}
				else {
					// oh no, 1 more number! Just shift the digits around and we'll be fine
					gps_height[0]=gps_height[1];
					gps_height[1]=gps_height[2];
					gps_height[2]=gps_height[3];
					gps_height[3]=gps_height[4];
					gps_height[4]=received_from_uart;				
					}
				break;                     
			case (30):				// this is the decimal part of the heigth                     
				gps_height_decimeters = received_from_uart;
				// notify the main loop we have gps data available
				calculateoutputgps = 1;
				// show visually for the enduser that we have a GPS fix
				if (LATCbits.LATC15 == 1) 
					LATCbits.LATC15 = 0; 
				else 
					LATCbits.LATC15 = 1; // toggle the LED			
				// and wait for the next NMEA message
				nmea_state = 1;
				break;
			default:
				nmea_state = 1;                    // we'll never and up here, but in case of a brownout make sure we start at the beginning
				break;                     
		}
	}
}
