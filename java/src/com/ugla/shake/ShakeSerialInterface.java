package com.ugla.shake;

/** 	A simple read/write interface for the driver to use. New connection options can be added
 * 		by writing a new class that conforms to this interface.
 */
public interface ShakeSerialInterface {

	public boolean openConnection(String btaddress);
	public boolean openConnection(int com_port);
	
	public boolean closeConnection();
	
	public boolean isRFCOMMSupported();
	public boolean isSerialSupported();
	
	public int readBytes(byte[] readbuf, int len, int offset);
	public int writeBytes(byte[] writebuf, int len, int offset);
	
}
