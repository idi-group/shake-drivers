package com.ugla.shake.sk6;

public class SK6Parsing {
	public static final int SK6_NUM_INFO_LINES	 = 7;
	public static final int SK6_LINE_MAX  = 200;

	public static final int SK6_ASCII_READ_OK		=	1;
	public static final int SK6_ASCII_READ_ERROR	=	-1;
	public static final int SK6_ASCII_READ_CONTINUE =	2;

	public static final int SK6_RAW_READ_OK		=	1;
	public static final int SK6_RAW_READ_ERROR	=	-1;
	public static final int SK6_RAW_READ_CONTINUE	=	2;

	public static final int SK6_HEADER = 0;
	public static final int 	SK6_COPYRIGHT = 1;
	public static final int 	SK6_FIRMWARE_REV = 2;
	public static final int 	SK6_HARDWARE_REV = 3;
	public static final int 	SK6_SERIAL_NUMBER = 4;
	public static final int 	SK6_EXPANSION_SLOT1 = 5;
	public static final int 	SK6_EXPANSION_SLOT2 = 6;

	public static String SK6_modules[] = { 
		"No option module",
		"Gyro Module SK6-G01",
		"Gyro Module SK6-G02",
		"RFID Module SK6-R01",
		"No option module",
		"Audio module SK6-A01",
		"External Vibration Module SK6-V01",
		"Polar Heart Rate Receiver Module SK6-H01",
		"USB Module SK6-U01",
		"Expansion Module SK6-E01",
	};
}
