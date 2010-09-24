package com.ugla.shake.bluetooth;

import java.io.IOException;

import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import java.io.InputStream;
import java.io.OutputStream;

import ugla.shake.SHAKESerialInterface;

/* This class is a simple wrapper for managing a Bluetooth Serial Port Profile connection on a Symbian phone */

public class ShakeJ2MESerialPort implements SHAKESerialInterface {
	private StreamConnection port = null;
	private InputStream inp = null;
	private OutputStream outp = null;
	
	protected long bytesRead = 0;

	public ShakeJ2MESerialPort() {
		
	}
	
	public boolean openConnection(String btaddress) {
		if(port != null)
			if(!closeConnection())
				return false;
		
		try {
			// this URL is used to create an unsecured SPP connection to the specified BT address
			// the port number is hardcoded to 1 since that's what all the SHAKEs use
			port = (StreamConnection) Connector.open(
					"btspp://" + btaddress + ":1"
					);
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		
		try {
			inp = port.openInputStream();
			outp = port.openOutputStream();
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}
	
	public boolean openConnection(int com_port) {
		return false;
	}
	
	public boolean closeConnection() {
		if(port == null)
			return true;
		
		try {
			inp.close();
			outp.close();
			port.close();
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}

		port = null;
		inp = null;
		outp = null;
		return true;
	}
	
	public int readBytes(byte[] b, int len, int offset) {
		if(inp == null)
			return -1;
                
                int bytes_read = 0;
                try {
                    int newbytes = inp.read(b, offset, len);
                    bytesRead += newbytes;
                    bytes_read += newbytes;
		} catch (IOException e) {}
		return bytes_read;
	}
	
	public int writeBytes(byte[] b, int len, int offset) {
            if(outp == null)
                return -1;

            int bytes_written = len;
            try {
            	outp.write(b, offset, len);
            	outp.flush();
            } catch (IOException e) {
                    e.printStackTrace();
                    return -1;
            }
            return bytes_written;
	}

	public boolean isRFCOMMSupported() {
		return true;
	}

	public boolean isSerialSupported() {
		return false;
	}
}
