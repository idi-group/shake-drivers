package com.ugla.shake.bluetooth;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import com.ugla.shake.ShakeSerialInterface;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothSocket;
import android.bluetooth.BluetoothDevice;

public class ShakeAndroidBluetooth implements ShakeSerialInterface {

	@Override
	public boolean openConnection(String btaddress) {
		BluetoothDevice btdev = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(btaddress);
		try {
			btsock = btdev.createRfcommSocketToServiceRecord(UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		try {
			btsock.connect();
			os = btsock.getOutputStream();
			is = btsock.getInputStream();
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		}
		
		return true;
	}

	@Override
	public boolean openConnection(int com_port) {
		return false;
	}

	@Override
	public boolean closeConnection() {
		try {
			is.close();
			os.close();
			btsock.close();
		} catch (IOException ioe) {}
		return false;
	}

	@Override
	public boolean isRFCOMMSupported() {
		return true;
	}

	@Override
	public boolean isSerialSupported() {
		return false;
	}

	@Override
	public int readBytes(byte[] readbuf, int len, int offset) {
		int bytes_read = 0;
		try {
			int newbytes = is.read(readbuf, offset, len);
			bytes_read += newbytes;
		} catch (IOException ioe) {
			return -1;
		}
		return bytes_read;
	}

	@Override
	public int writeBytes(byte[] writebuf, int len, int offset) {
		int bytes_written = len;
		try {
			os.write(writebuf, offset, len);
			os.flush();
		} catch (IOException ioe) {
			return -1;
		}
		return bytes_written;
	}
	
	private BluetoothSocket btsock = null;
	private InputStream is = null;
	private OutputStream os = null;
}
