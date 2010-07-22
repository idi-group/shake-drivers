package SHAKE;

public class SHAKERegister {
	
	public static String getReadString(int address) {
		return "$REA," + SHAKEUtils.formatHex(address, 4) + ",00";
	}
	
	public static String getWriteString(int address, int value) {
		return "$WRI," + SHAKEUtils.formatHex(address, 4) + "," + SHAKEUtils.formatHex(value, 2);
	}
}
