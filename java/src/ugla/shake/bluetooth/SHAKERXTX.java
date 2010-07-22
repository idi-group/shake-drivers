package ugla.shake.bluetooth;

import ugla.shake.SHAKESerialInterface;
import gnu.io.*;
import java.util.Enumeration;
import java.io.IOException;
import java.io.PrintStream;
import java.io.DataInputStream;

public class SHAKERXTX implements SHAKESerialInterface {
	private SerialPort port = null;
	private PrintStream outp = null;
	private DataInputStream inp = null;
	
	public boolean closeConnection() {
		if(port != null) {
			try {
				if(inp != null) inp.close();
				if(outp != null) outp.close();
				if(port != null) port.close();
			} catch (IOException e) {
				return false;
			}
		} else {
			return false;
		}
		return true;
	}

	/*public boolean openConnection(Object target) {
		String wantedPortName = (String)target;
		Enumeration portIdentifiers = CommPortIdentifier.getPortIdentifiers();
		CommPortIdentifier portId = null; 
		while (portIdentifiers.hasMoreElements())
		{
  	  		CommPortIdentifier pid = (CommPortIdentifier) portIdentifiers.nextElement();
    		if(pid.getPortType() == CommPortIdentifier.PORT_SERIAL &&
       			pid.getName().equals(wantedPortName)) 
    		{
		        portId = pid;
       		 	break;
   			} else {
			}
		}
		
		if(portId == null)
		{
			System.err.println("Failed to find specified port: " + wantedPortName);
		    return false;
		}

		port = null;
	   	try {
			port = (SerialPort)portId.open("SHAKERXTX", 1000);
		} catch (PortInUseException pe) {
			System.out.println("Port in use");
			return false;
		}

		try {
			port.setSerialPortParams(230400, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
		} catch (UnsupportedCommOperationException ucoe) {
			System.out.println("Port setup failed");
			return false;
		}

		try {
			inp = new DataInputStream(port.getInputStream());
		} catch (IOException ie) {
			System.out.println("Failed to get input stream");
			return false;
		}

		try {
			// true means automatically flush on every write
			outp = new PrintStream(port.getOutputStream(), true);
		} catch (IOException ie) {
			System.out.println("Failed to get output stream");
			return false;
		}
		return true;
	}*/

	public int readBytes(byte[] readbuf, int len, int offset) {
		int readbytes = 0;
		try {
			readbytes = inp.read(readbuf, offset, len);
		} catch (IOException ioe) {
			return 0;
		}
		return readbytes;
	}

	public int writeBytes(byte[] writebuf, int len, int offset) {
		outp.write(writebuf, offset, len);
		return len;
	}

	public boolean isRFCOMMSupported() {
		// TODO Auto-generated method stub
		return false;
	}

	public boolean isSerialSupported() {
		// TODO Auto-generated method stub
		return true;
	}

	public boolean openConnection(String btaddress) {
		String wantedPortName = (String)btaddress;
		Enumeration portIdentifiers = CommPortIdentifier.getPortIdentifiers();
		CommPortIdentifier portId = null; 
		while (portIdentifiers.hasMoreElements())
		{
  	  		CommPortIdentifier pid = (CommPortIdentifier) portIdentifiers.nextElement();
    		if(pid.getPortType() == CommPortIdentifier.PORT_SERIAL &&
       			pid.getName().equals(wantedPortName)) 
    		{
		        portId = pid;
       		 	break;
   			} else {
			}
		}
		
		if(portId == null)
		{
			System.err.println("Failed to find specified port: " + wantedPortName);
		    return false;
		}

		port = null;
	   	try {
			port = (SerialPort)portId.open("SHAKERXTX", 1000);
		} catch (PortInUseException pe) {
			System.out.println("Port in use");
			return false;
		}

		try {
			port.setSerialPortParams(230400, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
		} catch (UnsupportedCommOperationException ucoe) {
			System.out.println("Port setup failed");
			return false;
		}

		try {
			inp = new DataInputStream(port.getInputStream());
		} catch (IOException ie) {
			System.out.println("Failed to get input stream");
			return false;
		}

		try {
			// true means automatically flush on every write
			outp = new PrintStream(port.getOutputStream(), true);
		} catch (IOException ie) {
			System.out.println("Failed to get output stream");
			return false;
		}
		return true;
	}
	
	public boolean openConnection(int com_port) {
		return false;
	}

}
