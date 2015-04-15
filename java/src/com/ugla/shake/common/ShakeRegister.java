package com.ugla.shake.common;

public class ShakeRegister {
	
	public static String getReadString(int address) {
		return "$REA," + ShakeUtils.formatHex(address, 4) + ",00";
	}
	
	public static String getWriteString(int address, int value) {
		return "$WRI," + ShakeUtils.formatHex(address, 4) + "," + ShakeUtils.formatHex(value, 2);
	}
}
