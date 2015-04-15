package com.ugla.shake.bluetooth;

import com.ugla.shake.ShakeSerialInterface;
import jserial.JSerial;
 
public class ShakeSerialPort implements ShakeSerialInterface {

	JSerial port = new JSerial();
	
	public boolean closeConnection() {
		return port.close();
	}

	public int readBytes(byte[] readbuf, int len, int offset) {
		int bytes_read = 0, bytes_remaining = len;

		int newbytes = port.readBytes(readbuf, bytes_remaining, offset + bytes_read);
		bytes_read += newbytes;
		bytes_remaining -= newbytes;
		
		return bytes_read;
	}

	public int writeBytes(byte[] writebuf, int len, int offset) {
		return port.writeBytes(writebuf, len, offset);
	}

	public boolean isRFCOMMSupported() {
		return false;
	}

	public boolean isSerialSupported() {
		return true;
	}

	public boolean openConnection(String btaddress) {
		// TODO Auto-generated method stub
		return false;
	}
	
	public boolean openConnection(int com_port) {
		boolean ret = false;
		try {
			ret = port.open(com_port);
		} catch (Exception e) {
			ret = false;
		}
		return ret;
	}

}
