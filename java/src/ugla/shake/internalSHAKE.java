package SHAKE;

import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;
import java.lang.Runnable;

/* This class contains the code for parsing and updating sensor data etc */

public class internalSHAKE implements Runnable {
	protected SHAKESerialInterface port = null;
	private boolean closed = true;
	protected SHAKEListener eventCallback = null;
	protected boolean checksum = false;
	private boolean waiting_for_ack = false;
	private boolean lastack = false;
	protected int lastval = -1;
	protected int lastaddr = -1;
	private boolean waiting_for_ack_signal = false;
	protected boolean wait_for_acks = false;
	
	protected int[] acc = new int[3];
	protected int[] gyro = new int[3];
	protected int[] mag = new int[3];
	protected int heading ;
	protected int[] cap = new int[2];
	protected int[] ana = new int[2];
	protected int shaking_peak_accel;
	protected int shaking_direction;
	protected int shaking_timestamp;
	protected int hr_bpm;
	
    protected int[] timestamps = new int[8];
	protected int[] current_timestamps = new int[8];

	protected int slot1, slot2;
	protected String serial;
	protected float fwrev, hwrev;
	
	protected int lastevent;
	protected String lasttid;
	
	protected SHAKEDebug dbg = null;
	
    private byte[] sendMsg = null;
        
	protected Thread parser = null;
	
	protected int valid_packets = 0;
	protected int[] invalid_packets = new int[10];

	private static final int SHAKE_ASCII_READ_OK = 1;
	private static final int SHAKE_ASCII_READ_ERROR = -1;
	private static final int SHAKE_ASCII_READ_CONTINUE = 2;
	
	private static final int SHAKE_RAW_READ_OK = 1;
	private static final int SHAKE_RAW_READ_ERROR = -1;
	private static final int SHAKE_RAW_READ_CONTINUE = 2;
        	
	public boolean peek_flag = false;
	public short peek = 0;

	public internalSHAKE(SHAKESerialInterface _port) {
		SHAKEPackets.initialiseHeaders();
		port = _port;
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
			return SHAKEConstants.SHAKE_ERROR;
		
		String readReg = SHAKERegister.getReadString(address);
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
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			--timeout;
			if(timeout == 0)
				break;
		}

		this.waiting_for_ack = false;
		if(!this.lastack) {
			return SHAKEConstants.SHAKE_ERROR;
		}
		
		return this.lastval;
	}
	
	public boolean writeRegister(int reg, int val) {
            if(closed)
                return false;
	
            String writeReg = SHAKERegister.getWriteString(reg, val);
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
                        e.printStackTrace();
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

	public void setEventListener(SHAKEListener sl) {
		eventCallback = sl;
	}
	
	public boolean uploadVibrationSample(int address, int[] samples) {
		int i, cursample = 0, timeout = 250;

		if(samples.length <= 0 || (samples.length/2) > SHAKEConstants.SHAKE_VIB_SAMPLE_MAX_LENGTH) return false;

		String s = "$VIB," + SHAKEUtils.formatHex(address, 2) + ",00,00,00,";

		for(i=0;i<(samples.length/2)-1;i++) {
			s += SHAKEUtils.formatHex(samples[cursample], 2) + "," + SHAKEUtils.formatHex(samples[cursample+1], 2) + ",";
			cursample += 2;
		}

		s += SHAKEUtils.formatHex(samples[cursample], 2) + "," + SHAKEUtils.formatHex(samples[cursample+1], 2) + "~";

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
	
	public boolean playvib(int channel, int address) {
		if(channel < SHAKEConstants.SHAKE_VIB_MAIN || channel > SHAKEConstants.SHAKE_VIB_RIGHT)
			return false;

		if(address < 1) 
			return false;

		return writeRegister(SHAKEConstants.VO_REG_VIB_MAIN + channel, address);
	}
	
	private static int SHAKE_HEADER = 0;
	private static int SHAKE_COPYRIGHT = 1;
	private static int SHAKE_FIRMWARE_REV = 2; 
	private static int SHAKE_HARDWARE_REV = 3;
	private static int SHAKE_SERIAL_NUMBER = 4;
	private static int SHAKE_EXPANSION_SLOT1 = 5;
	private static int SHAKE_EXPANSION_SLOT2 = 6;

	
	protected static String[] modules = {
		"No option module",
		"Gyro Module SK6-G01",
		"Gyro Module SK6-G02",
		"RFID Module SK6-R01",
		"No option module",
		"Audio module SK6-A01",
		"External Vibration Module SK6-V01",
		"Polar Heart Rate Receiver Module SK6-H01",
		"USB Module SK6-U01",
		"Expansion Module SK6-E01",
	};
        
        private int readBytesWrapper(byte[] buf, int len, int offset) {
            if(dbg != null) dbg.debug("rBW 1: want to read " + len + " bytes");
            int bytes_read = 0;
            if (this.peek_flag) {
                buf[offset] = (byte)this.peek;
                len--;
                offset++;
                peek_flag = false;
               
                bytes_read = 1;
                if(dbg != null) dbg.debug("rBW 2: peek byte was set, len now = " + len + ", bytes_read = " + bytes_read);
                if(len == 0) {
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
	
	private int shake_read_info_line(byte[] buf, int maxlen) {
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
	
	private int read_shake_device_info() {
		byte[][] buf = new byte[7][200];
		int linecount = 0, len;
		
		for(linecount=0;linecount<7;linecount++) {
			len = shake_read_info_line(buf[linecount], 200);
			if(len == -1)
				return SHAKEConstants.SHAKE_ERROR;
			
			if(linecount == SHAKE_FIRMWARE_REV) {
				int plen = 0;
				byte[] ptr = buf[linecount];
				while((ptr[plen] != 0xA && ptr[plen] != 0xD) && (ptr[plen] < '0' || ptr[plen] > '9')) {
					plen++;
				}
				try {
                                    this.fwrev = Float.parseFloat(new String(ptr).substring(plen, plen+4));
                                } catch (NumberFormatException nfe) {}
			} else if(linecount == SHAKE_HARDWARE_REV) {
				int plen = 0;
				byte[] ptr = buf[linecount];
				while((ptr[plen] != 0xA && ptr[plen] != 0xD) && (ptr[plen] < '0' || ptr[plen] > '9')) {
					plen++;
				}
				this.hwrev = Float.parseFloat(new String(ptr).substring(plen, plen+4));
			} else if(linecount == SHAKE_SERIAL_NUMBER) {
				byte[] ptr = buf[linecount];
				int spacecount = 0, plen = 0;
				while(spacecount < 2) {
					if(ptr[plen] == ' ') 
						spacecount++;
					plen++;
				}
				
				this.serial = new String(ptr).substring(plen);
				this.serial = this.serial.substring(0, this.serial.indexOf('\r'));
			} else if(linecount == SHAKE_EXPANSION_SLOT1) {
				int i;
				String module = new String(buf[linecount]);
				module = module.substring(0, module.lastIndexOf('\r'));
				for(i=SHAKEConstants.SHAKE_MODULE_NONE;i<=SHAKEConstants.SHAKE_MODULE_EXPANSION;i++) {
					if(module.indexOf(modules[i]) != -1) {
						this.slot1 = i;
						break;
					}
				}
			} else if(linecount == SHAKE_EXPANSION_SLOT2) {
				int i;
				String module = new String(buf[linecount]);
				module = module.substring(0, module.lastIndexOf('\r'));
				for(i=SHAKEConstants.SHAKE_MODULE_NONE;i<=SHAKEConstants.SHAKE_MODULE_EXPANSION;i++) {
					if(module.indexOf(modules[i]) != -1) {
						this.slot2 = i;
						break;
					}
				}
			}
		}

		
		return 1;
	}
	
	private int parse_ascii_packet(int packet_type, byte[] packetbuf, int packetlen, boolean playback) {
            if(dbg != null) dbg.debug("parse_ascii_packet(" + packet_type + ")");
		if(packet_type != SHAKEPackets.SHAKE_ACK_ACK && packet_type != SHAKEPackets.SHAKE_ACK_NEG) {
			if(checksum) 
				if(dbg != null) dbg.debug("Parsing ASCII with checksum");
			else
				if(dbg != null) dbg.debug("Parsing ASCII no checksum");
			SHAKEPackets.parseASCIIPacket(this, packet_type, packetbuf, checksum);
		} else {
                    
			if(!waiting_for_ack) {
				if(dbg != null) dbg.debug("Skipped ACK");
                             
				return SHAKE_ASCII_READ_ERROR;
			}
				
			if(packet_type == SHAKEPackets.SHAKE_ACK_ACK)
				lastack = true;
			else
				lastack = false;
			
			SHAKEPackets.parseACK(this, packetbuf);
			
			waiting_for_ack_signal = false;
			if(dbg != null) dbg.debug("ACK signalled: " + new String(packetbuf));
		
		}
		return SHAKE_ASCII_READ_OK;
	}
	
	private int read_ascii_packet(int packet_type, byte[] packetbuf) {
		int packet_size = 0, bytes_left, bytes_read = 0;
		boolean playback = false;
                if(dbg != null) dbg.debug("read_ascii_packet(" + packet_type + ")");
                
		if(packet_type == SHAKEPackets.SHAKE_DATA_TIMESTAMP) {
			// TODO full timestamp packet support (store the timestamp data etc)

			readBytesWrapper(packetbuf, 
					SHAKEPackets.shake_packet_lengths[SHAKEPackets.SHAKE_DATA_TIMESTAMP] - SHAKEPackets.SHAKE_HEADER_LEN,
					SHAKEPackets.SHAKE_HEADER_LEN);
			
			// parse timestamp header here
			
			playback = true;
			readBytesWrapper(packetbuf, SHAKEPackets.SHAKE_HEADER_LEN, 0);
			packet_type = SHAKEPackets.classifyHeader(packetbuf, SHAKEPackets.SHAKE_HEADER_LEN, true);
		} else if (packet_type == SHAKEPackets.SHAKE_DATA_PLAYBACK_COMPLETE) {
			readBytesWrapper(packetbuf, 
					SHAKEPackets.shake_packet_lengths[packet_type] - SHAKEPackets.SHAKE_HEADER_LEN, 
					SHAKEPackets.SHAKE_HEADER_LEN);
			
			if(this.eventCallback != null) {
				lastevent = SHAKEConstants.SHAKE_PLAYBACK_COMPLETE;
				eventCallback.SHAKE_Event(lastevent);
			}
			return SHAKE_ASCII_READ_CONTINUE;
		} else if (packet_type == SHAKEPackets.SHAKE_DATA_RFID_TID) {
			if(dbg != null) dbg.debug("RFID TAG FOUND");
			readBytesWrapper(packetbuf, 
					SHAKEPackets.shake_packet_lengths[packet_type] - SHAKEPackets.SHAKE_HEADER_LEN, 
					SHAKEPackets.SHAKE_HEADER_LEN);
			String tmp = new String(packetbuf);
			lasttid = tmp.substring(SHAKEPackets.SHAKE_HEADER_LEN+1, 16);
			
			if(eventCallback != null) {
				lastevent = SHAKEConstants.SHAKE_RFID_TID_EVENT;
				eventCallback.SHAKE_Event(lastevent);
			}
			return SHAKE_ASCII_READ_CONTINUE;
		} else if (packet_type == SHAKEPackets.SHAKE_STARTUP_INFO) {
			read_shake_device_info();
			return SHAKE_ASCII_READ_CONTINUE;
		}
		
		bytes_left = SHAKEPackets.shake_packet_lengths[packet_type] - SHAKEPackets.SHAKE_HEADER_LEN;
		
		if(playback)
			bytes_left -= 3;
		
		bytes_read = readBytesWrapper(packetbuf, bytes_left, SHAKEPackets.SHAKE_HEADER_LEN);
		if(bytes_read != bytes_left) {
			if(dbg != null) dbg.debug("Failed to get full packet (" + bytes_read + "/" + bytes_left + ")");
			return SHAKE_ASCII_READ_ERROR;
		}
		
		if(playback) {
			int offset = bytes_read + SHAKEPackets.SHAKE_HEADER_LEN - 2;
			packetbuf[offset] = ',';
			packetbuf[offset+1] = '0';
			packetbuf[offset+2] = '0';
			packetbuf[offset+3] = '\r';
			packetbuf[offset+4] = '\n';
			bytes_read += 3;
		}
		
		if(SHAKEPackets.shake_packet_has_checksum[packet_type] == 1 
				&& packetbuf[bytes_read + SHAKEPackets.SHAKE_HEADER_LEN - 1] != 0xA) {
			checksum = true;
			
			readBytesWrapper(packetbuf, SHAKEPackets.CHECKSUM_LENGTH, SHAKEPackets.SHAKE_HEADER_LEN + bytes_read);
			bytes_read += 3;
		} else if(SHAKEPackets.shake_packet_has_checksum[packet_type] == 1 
				&& packetbuf[bytes_read + SHAKEPackets.SHAKE_HEADER_LEN - 1] == 0xA 
				&& checksum) {
			checksum = false;
		}
		
		packet_size = bytes_read + SHAKEPackets.SHAKE_HEADER_LEN;
		
		if(closed) 
			return SHAKE_ASCII_READ_ERROR;
		
		return parse_ascii_packet(packet_type, packetbuf, packet_size, playback);
	}
	
	private void compress_and_send_audio() {
		// TODO
	}
	
	private int parse_raw_packet(int packet_type, byte[] packetbuf, boolean has_seq) {
		  if(dbg != null) dbg.debug("parse_raw_packet(" + packet_type + ")");
		SHAKEPackets.parseRawPacket(this, packet_type, packetbuf, has_seq);
		if(packet_type == SHAKEPackets.SHAKE_RAW_DATA_AUDIO_HEADER) {
			compress_and_send_audio();
		}
		return SHAKE_RAW_READ_OK;
	}
	
	private int read_raw_packet(int packet_type, byte[] packetbuf) {
		if(dbg != null) dbg.debug("read_raw_packet(" + packet_type + ")");
		int bytes_left, bytes_read = 0;
		bytes_left = SHAKEPackets.shake_packet_lengths[packet_type] - SHAKEPackets.SHAKE_RAW_HEADER_LEN;
		//if(dbg != null) dbg.debug("bytes_left for type " + packet_type + " = " + bytes_left);
		bytes_read = readBytesWrapper(packetbuf, bytes_left, SHAKEPackets.SHAKE_RAW_HEADER_LEN);
		//if(dbg != null) dbg.debug("bytes_read = " + bytes_read);
		if(bytes_left - bytes_read > 1) {
                   if(dbg != null) dbg.debug("Error reading raw packet (" + bytes_read + "/" + bytes_left + ")");
                   return SHAKE_RAW_READ_ERROR;
		}
		
		boolean has_seq = false;
                this.peek_flag = false;
                
		if(bytes_left == bytes_read) {
			int packet_len = SHAKEPackets.shake_packet_lengths[packet_type];
			int trailing_byte = packetbuf[packet_len-1];
			
			if(trailing_byte == 0x7F) {
				// 0x7F is ALWAYS a header value in a new data packet
                                this.peek = 0x7F;
                                this.peek_flag = true;
			} else if(trailing_byte == 0x24 || trailing_byte == 0x0A) {
				int adjtype = packet_type - SHAKEPackets.SHAKE_RAW_DATA_ACC;
				if(trailing_byte == this.current_timestamps[adjtype] + 1 || (trailing_byte == 0 && this.current_timestamps[adjtype] == 255)) {
					has_seq = true;
				} else {
                                        this.peek = (short)trailing_byte;
                                        this.peek_flag = true;
				}
			} else {
				has_seq = true;
			}
		} else {
                    // no trailing byte
		}

		return parse_raw_packet(packet_type, packetbuf, has_seq);
	}
	
	private boolean is_ascii_packet(int packet_type) {
		if(packet_type >= SHAKEPackets.SHAKE_DATA_ACC && packet_type < SHAKEPackets.SHAKE_RAW_DATA_ACC) 
			return true;
		return false;
	}
	
	public void run() {
            try {
		int packet_type = SHAKEPackets.SHAKE_BAD_PACKET;
		int bytes_read;
		boolean valid_header = false;
		int sleepcounter = 20;
		
		while(!closed) {
                    valid_header = false;
                    packet_type = SHAKEPackets.SHAKE_BAD_PACKET;
                    byte[] packetbuf = new byte[128];
                    
                    //if(sleepcounter-- == 0) 
                    //	try { sleepcounter = 20; Thread.sleep(2);  } catch(InterruptedException ie) {} ;
                        
                    do {
                        if(dbg != null) dbg.debug("\nNEW LOOP\n");
                        bytes_read = readBytesWrapper(packetbuf, 3, 0);

                        if(bytes_read == 3) {
                                if((packetbuf[0] == 0x7F) && (packetbuf[1] == 0x7F)) {
                                        packet_type = SHAKEPackets.classifyHeader(packetbuf, SHAKEPackets.SHAKE_RAW_HEADER_LEN, false);
                                } else if(packetbuf[0] == '$' || packetbuf[0] == '\n') {
                                        readBytesWrapper(packetbuf, 1, 3);
                                        packet_type = SHAKEPackets.classifyHeader(packetbuf, SHAKEPackets.SHAKE_HEADER_LEN, true);
                                }
                        }
				
                        if(packet_type == SHAKEPackets.SHAKE_BAD_PACKET) {
                                byte[] c = new byte[1];

                                int read_count = 0;
                                while(read_count < 50 && (c[0] != '$' && c[0] != 0x7F)) {
                                        readBytesWrapper(c, 1, 0);
                                        read_count++;
                                }

                                packetbuf[0] = c[0];
                                if(c[0] == '$') {
                                        readBytesWrapper(packetbuf, SHAKEPackets.SHAKE_HEADER_LEN - 1, 1);
                                        packet_type = SHAKEPackets.classifyHeader(packetbuf, SHAKEPackets.SHAKE_HEADER_LEN, true);
                                } else if(c[0] == 0x7F) {
                                        readBytesWrapper(packetbuf, SHAKEPackets.SHAKE_RAW_HEADER_LEN - 1, 1);
                                        packet_type = SHAKEPackets.classifyHeader(packetbuf, SHAKEPackets.SHAKE_RAW_HEADER_LEN, false);
                                }
                        }
				
                        if(packet_type != SHAKEPackets.SHAKE_BAD_PACKET)
                            valid_header = true;
                        else
                            if(dbg != null) dbg.debug("Bad header: " + packetbuf[0] + packetbuf[1] + packetbuf[2] + packetbuf[3]);
				
                    } while (!closed && !valid_header);
			
                    if(closed)
                            return;

                    if(dbg != null) dbg.debug("\nPacket type: " + packet_type);

                    if(is_ascii_packet(packet_type)) {
                        read_ascii_packet(packet_type, packetbuf);
                    } else {
                        read_raw_packet(packet_type, packetbuf);
                    }
                }
		
		if(dbg != null)  dbg.debug("Thread exiting");
                } catch (Exception e) {
                    
                       e.printStackTrace();
                }
           }

}
