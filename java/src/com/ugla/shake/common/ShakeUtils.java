package com.ugla.shake.common;

public class ShakeUtils {

	public static String formatHex(int val, int len) {
		String s = "00000000" + Integer.toHexString(val);
		return s.substring(s.length() - len).toUpperCase();
	}
	
	public static boolean memcmp(byte[] b1, byte[] b2) {
		if (b1.length != b2.length) 
			return false;
		for (int i=b1.length; --i>=0; ) {
			if (b1[i] != b2[i]) 
				return false;
		}
		return true;
	}
	
	public static boolean memcmp(byte[] b1, byte[] b2, int startIndex, int endIndex) {
        if (b1.length != (endIndex-startIndex)) 
        	return false;
        
        for (int i=b1.length; --i>=0; ) {
            if (b1[i] != b2[i+startIndex]) 
            	return false;
        }
        return true;
    }

	public static int convertASCIIToInt(byte[] buf, int offset, int len, int digits) {
		int decval = 0, i, mult = 1;
		for(i=(len-1)+offset;i>=(len-digits)+offset;i--) {
			//System.out.println("> " + buf[i] + "(" + (buf[i] - '0') + ")");
			decval += (buf[i] - '0') * mult;
			mult *= 10;
		}
		if(digits == len - 1 && buf[offset] == '-')
			decval *= -1;
		
		return decval;
	}

	public static int convertHexASCIIToInt(byte[] buf, int offset, int len, int digits) {
		int hexval = 0, i, mult = 1;

		for(i=(len-1)+offset;i>=(len-digits)+offset;i--) {
			if(buf[i] >= '0' && buf[i] <= '9')
				hexval += (buf[i] - '0') * mult;
			else 
				hexval += ((buf[i] - 'A') + 10) * mult;
		
			mult *= 16;
		}
		if(digits == len - 1 && buf[offset] == '-')
			hexval *= -1;
		
		return hexval;
	}
}
