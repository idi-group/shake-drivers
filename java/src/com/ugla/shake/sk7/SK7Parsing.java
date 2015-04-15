package com.ugla.shake.sk7;

public class SK7Parsing {
	public static final int SK7_NUM_INFO_LINES =	10;
	public static final int SK7_LINE_MAX = 200;

	public static final int SK7_ASCII_READ_OK	=	1;
	public static final int SK7_ASCII_READ_ERROR = -1;
	public static final int SK7_ASCII_READ_CONTINUE	= 2;

	public static final int SK7_RAW_READ_OK	=	1;
	public static final int SK7_RAW_READ_ERROR	= -1;
	public static final int SK7_RAW_READ_CONTINUE = 2;

	public static final int SK7_HEADER = 0;
	public static final int SK7_COPYRIGHT = 1;
	public static final int SK7_HARDWARE_REV = 2;
	public static final int SK7_FIRMWARE_REV = 3;
	public static final int SK7_SERIAL_NUMBER = 4;
	public static final int SK7_SLOT0 = 5;
	public static final int SK7_SLOT1 = 6;
	public static final int SK7_SLOT2 = 7;
	public static final int SK7_SLOT3 = 8;
	public static final int SK7_BLUETOOTH_FIRMWARE = 9; 

	public static String SK7_modules[] = { 
		"No option module",
	};
}
