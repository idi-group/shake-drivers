package SHAKE;

public interface SHAKESerialInterface {

	public boolean openConnection(String btaddress);
	public boolean openConnection(int com_port);
	
	public boolean closeConnection();
	
	public boolean isRFCOMMSupported();
	public boolean isSerialSupported();
	
	public int readBytes(byte[] readbuf, int len, int offset);
	public int writeBytes(byte[] writebuf, int len, int offset);
	
}
