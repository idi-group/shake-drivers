package com.ugla.shake;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.lang.Runnable;

import com.ugla.shake.ShakeConstants.ShakeDeviceType;
import com.ugla.shake.common.Shake;
import com.ugla.shake.sk6.ShakeSk6;
import com.ugla.shake.sk7.ShakeSk7;
import com.ugla.shake.common.ShakeRegister;
import com.ugla.shake.common.ShakeUtils;

// TODO this is a mess, combine with Shake class or something...
public class ShakeDevicePrivate implements Runnable {
	public ShakeSerialInterface port = null;
	public boolean closed = true;
	public boolean checksum = false;
	protected ShakeListener eventCallback = null;
	public boolean waiting_for_ack = false;
	public boolean lastack = false;
	public int lastval = -1;
	public int lastaddr = -1;
	public boolean waiting_for_ack_signal = false;
	public boolean wait_for_acks = false;
	
	public int slot1, slot2;
	public String serial;
	public float fwrev, hwrev, bluetoothfwrev;
	
	public int lastevent;
	public String lasttid;
	
	protected ShakeDebug dbg = null;
	
    private byte[] sendMsg = null;
        
	protected Thread parser = null;
	
	protected int valid_packets = 0;
	protected int[] invalid_packets = new int[10];
	
	protected Shake shake = null;
	
	public boolean peek_flag = false;
	public short peek = 0;

	public ShakeDevicePrivate(ShakeDevice dev, ShakeConstants.ShakeDeviceType devtype, ShakeSerialInterface _port) {
		port = _port;
		
		if(devtype == ShakeDeviceType.SHAKE_SK6)
			shake = new ShakeSk6(dev, this);
		else
			shake = new ShakeSk7(dev, this);
	}
	
	public boolean openConnection(String btaddress) {

		if(!port.openConnection(btaddress))
			return false;
		closed = false;

		parser = new Thread(this);
		parser.start();
		
		return true;
	}
	
	public boolean openConnection(int com_port) {

		if(!port.openConnection(com_port))
			return false;
		closed = false;

		parser = new Thread(this);
		parser.start();
		
		return true;
	}
	
	public boolean closeConnection() {
		closed = true;
		if(!port.closeConnection())
			return false;
		
		return true;
	}
	
	public int readRegister(int address) {
		if(closed)
			return ShakeConstants.SHAKE_ERROR;
		
		String readReg = ShakeRegister.getReadString(address);
		port.writeBytes(readReg.getBytes(), readReg.length(), 0);

		if(dbg != null) dbg.debug("readReg wrote: " + readReg);
		
		this.waiting_for_ack_signal = true;
		this.waiting_for_ack = true;
		int timeout = 250;
		
		while(this.waiting_for_ack_signal) {
			try {
				Thread.sleep(1);
                Thread.yield();
			} catch (InterruptedException e) {
			}
			--timeout;
			if(timeout == 0)
				break;
		}

		this.waiting_for_ack = false;
		if(!this.lastack) {
			return ShakeConstants.SHAKE_ERROR;
		}
		
		return this.lastval;
	}
	
	public boolean writeRegister(int reg, int val) {
            if(closed)
                return false;
	
            String writeReg = ShakeRegister.getWriteString(reg, val);
            if(dbg != null) dbg.debug("writeReg start: " + writeReg);
         
            port.writeBytes(writeReg.getBytes(), writeReg.length(), 0);
			if(!wait_for_acks)
				return true;
            this.waiting_for_ack_signal = true;
            this.waiting_for_ack = true;
            
            if(dbg != null) dbg.debug("writeReg end: " + writeReg);

            int timeout = 150;

            while(this.waiting_for_ack_signal) {
                try {
                        Thread.sleep(1);
                } catch (InterruptedException e) {
                }
                --timeout;
                if(timeout == 0)
                        break;
            }

            if(dbg != null) dbg.debug("writeReg wfas: " + this.waiting_for_ack_signal);

            this.waiting_for_ack = false;
            if(!this.lastack) {
                return false;
            }

            return true;
	}

	public void setEventListener(ShakeListener sl) {
		shake.callback = sl;
	}
	
	public boolean uploadVibrationSample(int address, int[] samples) {
		int i, cursample = 0, timeout = 250;

		if(samples.length <= 0 || (samples.length/2) > ShakeConstants.SHAKE_VIB_SAMPLE_MAX_LENGTH) return false;

		String s = "$VIB," + ShakeUtils.formatHex(address, 2) + ",00,00,00,";

		for(i=0;i<(samples.length/2)-1;i++) {
			s += ShakeUtils.formatHex(samples[cursample], 2) + "," + ShakeUtils.formatHex(samples[cursample+1], 2) + ",";
			cursample += 2;
		}

		s += ShakeUtils.formatHex(samples[cursample], 2) + "," + ShakeUtils.formatHex(samples[cursample+1], 2) + "~";

		if(port.writeBytes(s.getBytes(), s.length(), 0) != s.length())
			return false;

		this.waiting_for_ack_signal = true;	
		this.waiting_for_ack = true;

		while(this.waiting_for_ack_signal) {
			try {
				Thread.sleep(1);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			--timeout;
			if(timeout == 0)
				break;
		}

		this.waiting_for_ack = false;
		
		if(!this.lastack)
			return false;

		return true;
	}
	
	public int readBytesWrapper(byte[] buf, int len, int offset) {
		if(dbg != null) dbg.debug("rBW 1: want to read " + len + " bytes");
		int bytes_read = 0;
		if (peek_flag) {
			buf[offset] = (byte)peek;
			peek_flag = false;    
			bytes_read = 1;
			if(dbg != null) dbg.debug("rBW 2: peek byte was set, len now = " + len + ", bytes_read = " + bytes_read);
			if(len == 1) {
		     	// if we only needed one byte can return here
			 	if(dbg != null) dbg.debug("rBW 3: only 1 byte required, returning\n");
		         	return 1;
		     }
		 }

         if(dbg != null) dbg.debug("rBW 4: reading " + len + " bytes, bytes_read = " + bytes_read);
         int retries = 0;
         while(retries < 30 && bytes_read != len) {
             bytes_read += port.readBytes(buf, len - bytes_read, offset + bytes_read);
             if(dbg != null) dbg.debug("rBW 5: bytes_read = " + bytes_read);
             retries++;
             if(retries % 10 == 0) try { Thread.sleep(10); } catch (InterruptedException ie) {}
         }
         if(dbg != null) dbg.debug("rBW 6: completed with " + bytes_read + " bytes after " + retries + " retries\n");

         return bytes_read;
	}
	
	public int read_info_line(byte[] buf, int maxlen) {
		int bufpos = 0;
		byte[] b = new byte[1];

		while(bufpos < maxlen) {
			readBytesWrapper(b, 1, 0);
			
			if(b[0] == 0)
				continue;
			
			buf[bufpos] = b[0];
			
			if(buf[bufpos] == 0xD || buf[bufpos] == 0xA) {
				if(bufpos >= 1) {
					int i;
					
					i = bufpos - 1;
					if((buf[i] == 0xA && buf[i+1] == 0xD) || (buf[i] == 0xD && buf[i+1] == 0xA))
						return i;
				}
			}
			bufpos++;
		}
		
		return -1;
	}
	
	public void run() {
		try {
			while(!this.closed) {
				int packet_type = ShakeConstants.SHAKE_BAD_PACKET;
				byte[] packetbuf = new byte[256];
				do {
					packet_type = shake.get_next_packet(packetbuf, packetbuf.length);
				} while(!this.closed && (packet_type == ShakeConstants.SHAKE_BAD_PACKET));
				
				if(!this.closed)
					shake.parse_packet(packetbuf, packet_type);
			}
        } catch (Exception e) {
           e.printStackTrace();
        }
   }

}
