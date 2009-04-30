# SHAKE packet handling stuff. This is where all the parsing etc is done.

from pyshake_constants import *
import thread, re, string, os

[SHAKE_HEADER, SHAKE_COPYRIGHT, SHAKE_FIRMWARE_REV, SHAKE_HARDWARE_REV, 
SHAKE_SERIAL_NUMBER, SHAKE_EXPANSION_SLOT1,SHAKE_EXPANSION_SLOT2] = range(7)

SHAKE_ASCII_READ_OK = 1
SHAKE_ASCII_READ_ERROR = -1
SHAKE_ASCII_READ_CONTINUE = 2

SHAKE_RAW_READ_OK = 1
SHAKE_RAW_READ_ERROR = -1
SHAKE_RAW_READ_CONTINUE = 2

platform = "S60"
if os.name == 'nt' or os.name == 'ce':
	platform = "Windows"
	from time import sleep as ssleep
	import pyshake_serial_pc as pyshake_serial
elif os.name == 'posix':
	platform = "Posix"
	from time import sleep as ssleep
	import pyshake_serial_pc as pyshake_serial
else:
	from e32 import ao_callgate
	from e32 import ao_sleep as ssleep
	import pyshake_serial_s60 as pyshake_serial
	
debugfp = None
debugging = False
def debug(str, opennew=False):
	if not debugging:
		return

	f = None
	if opennew:
		f = open("c:/debug.txt", "w")
	else:
		f = open("c:/debug.txt", "a")

	f.write(str + "\n")
	f.close()

def sleep(period):
	ssleep(period)

# class which defines a variable for each item of data the SHAKE can return, plus sequence numbers
class shake_sensor_data:
	def __init__(self):
		self.accx, self.accy, self.accz = 0,0,0
		self.gyrx, self.gyry, self.gyrz = 0,0,0
		self.magx, self.magy, self.magz = 0,0,0
		self.heading = 0
		self.cap0, self.cap1 = 0,0
		self.ana0, self.ana1 = 0,0
		self.peakaccel = 0
		self.timestamp = 0
		self.direction = 0
		self.hrbpm = 0
		self.tid = ""
		self.accseq, self.gyrseq, self.magseq, self.headingseq, self.cap0seq, self.cap1seq, self.ana0seq, self.ana1seq = 0,0,0,0,0,0,0,0
		self.timestamps = [0,0,0,0,0,0,0,0]

# equivalent to the shake_device_private structure in the C driver, although not really private
# in the Python impl. Takes either a COM port number or a BT addr (keyworded appropriately), 
# then sets up the required state and launches a thread to read from the serial port (and write,
# in the case of the S60 where sockets can't be shared between threads)
class shake_device_private:
	modules = ["No option module",
	"Gyro Module SK6-G01",
	"Gyro Module SK6-G02",
	"RFID Module SK6-R01",
	"No option module",
	"Audio module SK6-A01",
	"External Vibration Module SK6-V01",
	"Polar Heart Rate Receiver Module SK6-H01",
	"USB Module SK6-U01",
	"Expansion Module SK6-E01" ]
	
	def __init__(self, bt_addr):
		self.data = shake_sensor_data()

		#self.dbgmode = "starting up"
		self.sendmsg = None
		self.lastack = False
		self.lastaddr, self.lastval = 0,0
		self.thread_done = False
		self.thread_exit = False
		self.waiting_for_ack = False
		self.synced = False
		self.logging = False
		self.logfile_handle = None
		# ASCII mode
		self.datamode = 0
		# no checksum
		self.checksum = False
		self.peek = 0
		self.peek_flag = False

		self.eventcallback = None
		
		self.write_to_port = None

		self.fwrev = None
		self.hwrev = None
		self.serial = None
		self.lasttid = ""
		self.slot1 = None
		self.slot2 = None
		self.read_startup = False

		# workaround for not being able to create a socket in one thread
		# and access it in another on the S60. The socket/port creation
		# is all done in the thread that's spawned below.
		self.s60 = True
		self.bluetooth_addr = bt_addr
		self.port = None

		# launch the thread
		debug("starting thread", True)
		thread.start_new_thread(self.run, ())
		#self.run()

	# 	sends the given byte string through the internal port object
	def write(self, bytes):
		self.port.write(bytes)

	def read(self, bytes_to_read):
		bytes = ""
		if self.peek_flag:
			bytes += chr(self.peek)
			bytes_to_read = bytes_to_read - 1
			self.peek_flag = False

			# in case we only needed to read 1 byte
			if bytes_to_read == 0:
				return 1

		allbytes = bytes + self.port.read(bytes_to_read)

		if len(allbytes) > 0:
			self.synced = True
		return allbytes
			
	# 	closes the internal port object and terminates reader thread
	def close(self):
		self.thread_done = True
		
		totaltime = 0.0
		while totaltime < 1.0 and not self.thread_exit:
			ssleep(0.1)
			totaltime += 0.1
		debug("thread exited event")
	
	def shake_read_info_line(self):
		pos = 0
		buf = ""
		while pos < 512:
			tmp = self.read(1)
			
			# skip nulls
			if ord(tmp) == 0:
				continue
		
			buf += tmp
		
			if len(buf) > 0 and (ord(buf[pos]) == 0xD or ord(buf[pos]) == 0xA):
				# compare last 2 bytes to delimiter
				if pos >= 1:
					i = pos - 1
					if (ord(buf[i]) == 0xA and ord(buf[i+1]) == 0xD) or (ord(buf[i]) == 0xD and ord(buf[i+1]) == 0xA):
						return buf
					
			pos += 1
		
		return None
	
	def read_shake_device_info(self):
		self.read_startup = False
		for i in range(7):
			line = self.shake_read_info_line()
			if line == None:
				debug("FAILED TO READ LINE")
				return False
			
			debug("line " + str(i) + ": '" + line + "'")

			if i == SHAKE_FIRMWARE_REV: 	# firmware revision line
				# find start of revision number and get the value
				pos = 0
				while (ord(line[pos]) != 0xA and ord(line[pos]) != 0xD) and (line[pos] < '0' or line[pos] > '9'):
					pos += 1

				try:
					self.fwrev = float((line[pos:pos+4]))
				except ValueError:
					self.fwrev = None
				debug("got fw rev: " + str(self.fwrev))
			elif i == SHAKE_HARDWARE_REV: 	# hardware revision line
				# find start of revision number and get the value
				pos = 0
				while (ord(line[pos]) != 0xA and ord(line[pos]) != 0xD) and (line[pos] < '0' or line[pos] > '9'):
					pos += 1

				try:
					self.hwrev = float((line[pos:pos+4]))
				except ValueError:
					self.hwrev = None
				debug("got hw rev: " + str(self.hwrev))
			elif i == SHAKE_SERIAL_NUMBER: 	# serial number
				# find start of serial number and read remainder of string
				pos = 0
				spacecount = 0
				while spacecount < 2:
					if line[pos] == ' ':
						spacecount += 1
					pos += 1

				self.serial = line[pos:]
				pos = 0
				while pos < len(self.serial):
					if ord(self.serial[pos]) == 0xA:
						break
					pos += 1

				self.serial = self.serial[:pos]
				debug("got serial: " + self.serial)
			elif i == SHAKE_EXPANSION_SLOT1:
				module = line
				for i in range(len(shake_device_private.modules)):
					if module[:len(shake_device_private.modules[i])] == shake_device_private.modules[i]:
						self.slot1 = i
						break
				debug("got slot1: " + str(self.slot1))
			elif i == SHAKE_EXPANSION_SLOT2:
				module = line
				for i in range(len(shake_device_private.modules)):
					if module[:len(shake_device_private.modules[i])] == shake_device_private.modules[i]:
						self.slot2 = i
						break
				debug("got slot2: " + str(self.slot2))
				
		self.read(1)
		debug("read info completed OK")
		self.synced = True
		self.read_startup = True
		return True
	
	def port_setup(self):
		# create the port inside the current thread
		try:	
			self.port = pyshake_serial.serial_port(self.bluetooth_addr)
			if not self.port.open():
				debug("port creation failed")
				self.thread_done = True
				return SHAKE_ERROR

			if platform == "S60":
				self.write_to_port = ao_callgate(self.port.write)
			else:
				self.write_to_port = self.port.write
		except pyshake_serial.pyshake_serial_error:
			debug(u'error; port creation failed')
			self.thread_done = True
			return SHAKE_ERROR


		
	def send_waiting_packet(self):
		self.write(self.sendmsg)
		self.sendmsg = None
		
	def is_ascii_packet(self, t):
		if t >= SHAKE_DATA_ACC and t < SHAKE_RAW_DATA_ACC:
			return True
		return False
	
	def read_ascii_packet(self, pkttype, packet):
		packet_size = 0
		bytes_left = 0 
		bytes_read = 0
		playback = False

		debug("read_ascii_packet: " + str(pkttype))
		
		if pkttype == SHAKE_DATA_TIMESTAMP:
			packet += self.read(shake_packet_lengths[SHAKE_DATA_TIMESTAMP] - SHAKE_HEADER_LEN)
			# TODO parse packet
			playback = True
			packet = ""
			packet += self.read(SHAKE_HEADER_LEN)
			pkttype = classify_packet_header(self, packet, True)
			debug("read_ascii_packet: timestamp type " + str(pkttype))
		elif pkttype == SHAKE_DATA_PLAYBACK_COMPLETE:
			packet += self.read(shake_packet_lengths[SHAKE_DATA_TIMESTAMP] - SHAKE_HEADER_LEN)
			playback = False
			
			debug("read_ascii_packet: playback complete")

			if self.eventcallback != None:
				self.eventcallback(SHAKE_PLAYBACK_COMPLETE)
			
			return SHAKE_ASCII_READ_CONTINUE
		elif pkttype == SHAKE_DATA_RFID_TID:
			packet += self.read(shake_packet_lengths[SHAKE_DATA_TIMESTAMP] - SHAKE_HEADER_LEN)
			self.lasttid = packet[SHAKE_HEADER_LEN+1:]
			if self.eventcallback != None:
				self.eventcallback(SHAKE_RFID_TID_EVENT)
			debug("read_ascii_packet: RFID detection")
			return SHAKE_ASCII_READ_CONTINUE
		elif pkttype == SHAKE_STARTUP_INFO:
			debug("SHAKE_STARTUP_INFO packet")
			self.read_shake_device_info()
			return SHAKE_ASCII_READ_CONTINUE
		
		debug("packet before: '" + packet + "'")
		bytes_left = shake_packet_lengths[pkttype] - SHAKE_HEADER_LEN
		debug("there are " + str(bytes_left) + " bytes left to read from a total of " + str(shake_packet_lengths[pkttype]))
		
		if playback:
			bytes_left -= 3
			
		packet += self.read(bytes_left)
		debug("packet now: '" + packet + "'")
		if len(packet) - SHAKE_HEADER_LEN != bytes_left:
			return SHAKE_ASCII_READ_ERROR

		bytes_read = bytes_left
		
		if playback:
			offset = len(packet) - 2
			packet = packet[offset:] + ",00\r\n"
			bytes_read += 3
			
		if shake_packet_has_checksum[pkttype] == 1 and ord(packet[bytes_read + SHAKE_HEADER_LEN - 1]) != 0xA:
			self.checksum = True
			debug("reading checksum bytes")
			packet += self.read(CHECKSUM_LENGTH)
			bytes_read += 3
		elif shake_packet_has_checksum[pkttype] == 1 and ord(packet[bytes_read + SHAKE_HEADER_LEN - 1]) == 0xA and self.checksum:
			self.checksum = False
			debug("no checksum found")
			 
		packet_size = bytes_read + SHAKE_HEADER_LEN
		if self.thread_done:
			return SHAKE_ASCII_READ_ERROR
		
		return self.parse_ascii_packet(pkttype, packet, packet_size, playback)
			
	def parse_ascii_packet(self, pkttype, packet, packetlen, playback):
		if pkttype != SHAKE_ACK_ACK and pkttype != SHAKE_ACK_NEG:
			if self.checksum:
				debug("updating ASCII + checksum with packet '" + packet + "'")
				shake_update_data_checksums(self, pkttype, packet)
			else:
				debug("updating ASCII no checksum with packet '" + packet + "'")
				shake_update_data_normal(self, pkttype, packet)
		else:
			debug("GOT ACK " + packet)
			if not self.waiting_for_ack:
				self.waiting_for_ack_signal = False
				debug("ACK DISCARDED")
				return SHAKE_ASCII_READ_ERROR
			
			if pkttype == SHAKE_ACK_ACK:
				self.lastack = True
			else:
				self.lastack = False
				
			debug("parsing ACK: " + packet)
			(self.lastaddr, self.lastval) = shake_parse_ack_packet(packet)
			
			self.waiting_for_ack_signal = False
			
		return SHAKE_ASCII_READ_OK
		
	def read_raw_packet(self, pkttype, packet):
		debug("read_raw_packet, type = " + str(pkttype))
		packet_size, bytes_left = 0, 0
		bytes_left = shake_packet_lengths[pkttype] - SHAKE_RAW_HEADER_LEN
		packet += self.read(bytes_left)
		if (bytes_left - (len(packet) - SHAKE_RAW_HEADER_LEN)) > 1:
			debug("read_raw_packet: failed to read full packet")
			return SHAKE_RAW_READ_ERROR
		
		fullpacket = False
		if len(packet) == SHAKE_RAW_HEADER_LEN + bytes_left:
			fullpacket = True
			debug("read_raw_packet: fullpacket = True")

		has_seq = False
		self.peek_flag = False
		
		if fullpacket:
			debug("full packet received, getting length")
			packet_len = shake_packet_lengths[pkttype]
			debug("got length " + str(packet_len) + " bytes")
			trailing_byte = ord(packet[packet_len-1])
			debug("trailing_byte = " + str(trailing_byte))

			if trailing_byte == 0x7F:
				self.peek = trailing_byte
				self.peek_flag = True
				debug("set peek flag to true as 0x7F found")
			elif trailing_byte == 0x24 or trailing_byte == 0x0A:
				adjtype = pkttype - SHAKE_RAW_DATA_ACC
				debug("adjtype = " + str(adjtype))
				if trailing_byte == self.data.timestamps[adjtype] + 1 or (trailing_byte == 0 and self.data.timestamps[adjtype] == 255):
					has_seq = True
					debug("has seq = True")
				else:
					debug(">>> preserving trailing byte for ASCII packet")
					self.peek = trailing_byte
					self.peek_flag = True
			else:
				has_seq = True
		else:
			# no trailing byte means sequence numbers are off (probably)
			pass

		debug("parsing raw packet with type " + str(pkttype) + " / " + str(has_seq))
		return self.parse_raw_packet(pkttype, packet, has_seq)
		
	def parse_raw_packet(self, pkttype, packet, has_seq):
		debug("update for raw packet type " + str(pkttype))
		shake_update_data_raw(self, pkttype, packet, has_seq)
		if pkttype == SHAKE_RAW_DATA_AUDIO_HEADER:
			pass # TODO shake_compress_and_send_audio
		return SHAKE_RAW_READ_OK

	# this function runs in a thread and deals with reading (and on the S60 platform, writing)
	# data from the serial port/bt socket.
	def run(self):
		packet = ""
		debug("setting up port")
		if self.port_setup() == SHAKE_ERROR:
			debug("port setup failed")
			return 

		self.thread_done = False
		debug("port setup done")
		
		#self.write("$WRI,0002,00")

		debug("entering main loop")
		while not self.thread_done:
			debug("start of main loop")
			valid_header = False
			packet_type = -1
		
			if self.thread_done:
				break

			#if self.sendmsg != None:
			#	debug("sending packet")
			#	self.send_waiting_packet()
				
			debug("starting check for header")
			while not self.thread_done and not valid_header:
				packet = self.read(3)
				
				debug("initial header: " + packet + "(" + str(len(packet)) + "), " + str(len(packet)) + " bytes")
				if len(packet) == 3:
					if ord(packet[0]) == 0x7F and ord(packet[1]) == 0x7F:
						packet_type = classify_packet_header(self, packet, False)
						debug("classified raw " + str(packet_type))
					elif packet[0] == '$' or ord(packet[0]) == 0xA:
						packet += self.read(1)
						debug("Full ascii header: " + packet)
						packet_type = classify_packet_header(self, packet, True)
						debug("classified ASCII " + str(packet_type))
				
				if self.thread_done:
					break
			
				if packet_type == SHAKE_BAD_PKT:
					debug("in error handler")
					read_count = 0
					packet = ['\0']
					while read_count < 50 and (packet[0] != '$' and ord(packet[0]) != 0x7F):
						packet = self.read(1)
						read_count += 1
					
					if packet[0] == '$':
						debug("error handler: first char is ASCII header")
						packet += self.read(SHAKE_HEADER_LEN - 1)
						debug("error handler: new header is  "+ packet)
						packet_type = classify_packet_header(self, packet, True)
					elif ord(packet[0]) == 0x7F:					
						debug("error handler: first char is 0x7F")
						packet += self.read(SHAKE_RAW_HEADER_LEN - 1)
						debug("error handler: new header is  "+ packet)
						packet_type = classify_packet_header(self, packet, False)
					else:
						debug("error handler: this shouldn't happen")
						
				if packet_type != SHAKE_BAD_PKT:
					valid_header = True
			
			debug("Packet type = " + str(packet_type))
			if self.is_ascii_packet(packet_type):
				debug("Parsing ascii packet")
				self.read_ascii_packet(packet_type, packet)
			else:
				debug("Parsing raw packet")
				self.read_raw_packet(packet_type, packet)
	
			self.synced = True

		self.port.close()
		self.thread_exit = True
		debug("closing port and exiting thread")

# 	The following set of classes parse a raw packet into
# 	the appropriate numeric values

class shake_data_acc_packet:
	def __init__(self, string):
		# fmt: $ACC,+dddd,+dddd,+dddd,ss*CS\r\n
		self.accx = int(string[5:10])
		self.accy = int(string[11:16])
		self.accz = int(string[17:22])
		self.seq = int(string[23:25])

class shake_data_gyr_packet:
	def __init__(self, string):
		# fmt: $ARS,+dddd,+dddd,+dddd,ss*CS\r\n
		self.gyrx = int(string[5:10])
		self.gyry = int(string[11:16])
		self.gyrz = int(string[17:22])
		self.seq = int(string[23:25])

class shake_data_mag_packet:
	def __init__(self, string):
		# fmt: $MAG,+dddd,+dddd,+dddd,ss*CS\r\n
		self.magx = int(string[5:10])
		self.magy = int(string[11:16])
		self.magz = int(string[17:22])
		self.seq = int(string[23:25])

class shake_data_heading_packet:
	def __init__(self, string):
		# $HED,dddd,dd*CS
		self.heading = int(string[5:9])
		self.seq = int(string[10:12])

class shake_data_cap_packet:
	def __init__(self, string):
		# $CS0,dddd,dd*CS[CR][LF]
		self.prox = int(string[5:9])
		self.seq = int(string[10:12])

class shake_data_analog_packet:
	def __init__(self, string):
		# $AI0,dddd,dd*CS
		self.voltage = int(string[5:9])
		self.seq = int(string[10:12])

class shake_data_shaking_packet:
	def __init__(self, string):
		self.peakaccel = int(string[5:10])
		self.direction = int(string[11:16])
		self.timestamp = int(string[17:22])
	
class shake_heart_rate_packet:
	def __init__(self, string):
		self.bpm = int(string[5:9])

class shake_ack_packet:
	def __init__(self, s):
		# int() doesn't like hex values so have to do things differently here
		#self.addr = int((s[5:9]))
		try:
			self.addr = string.atoi((s[5:9]), 16)
		except ValueError:
			# handles VIB acks, set random valid address
			self.addr = 100
		self.val = string.atoi((s[10:12]), 16)
		
# returns an (addr, value) tuple containing those values from the <ack_packet> string
def shake_parse_ack_packet(packet):
	ap = shake_ack_packet(packet)
	return (ap.addr, ap.val)

def classify_packet_header(devpriv, header, ascii_mode):

	debug("classify len = " + str(len(header)) + " first char = " + header[0])
	if ascii_mode and (len(header) != SHAKE_HEADER_LEN):
		return SHAKE_BAD_PKT
	if not ascii_mode and (len(header) != SHAKE_RAW_HEADER_LEN or ord(header[0]) != 0x7F or ord(header[1]) != 0x7F):
		return SHAKE_BAD_PKT

	debug("classify: initial tests OK")
	i = 0
	if ascii_mode:
		i = 0
	else:
		i = SHAKE_RAW_DATA_ACC
		
	for i in range(i, SHAKE_NUM_PACKET_TYPES):
		if ascii_mode:
			if shake_packet_headers[i] == header[:SHAKE_HEADER_LEN]:
				return i
			if i >= SHAKE_STARTUP_INFO:
				return SHAKE_BAD_PKT
		else:
			if ord(header[2]) == shake_raw_packet_headers[i - SHAKE_RAW_DATA_ACC]:
				return i
			
	return SHAKE_BAD_PKT
		

def shake_update_data_checksums(dev, packet_type, packet):
	# update data member of dev depending on value of <type>, reading info from <packet>
	# would be nice if python had a switch statement here ...
	if packet_type == SHAKE_DATA_ACC:
		shake_update_acc(dev, packet)
	elif packet_type == SHAKE_DATA_GYRO:
		shake_update_gyr(dev, packet)
	elif packet_type == SHAKE_DATA_MAG:
		shake_update_mag(dev, packet)
	elif packet_type == SHAKE_DATA_HEADING:
		shake_update_heading(dev, packet)
	elif packet_type == SHAKE_DATA_CAP0:
		shake_update_cap0(dev, packet)
	elif packet_type == SHAKE_DATA_CAP1:
		shake_update_cap1(dev, packet)
	elif packet_type == SHAKE_DATA_ANA0:
		shake_update_ana0(dev, packet)
	elif packet_type == SHAKE_DATA_ANA1:
		shake_update_ana1(dev, packet)
	elif packet_type >= SHAKE_DATA_NVU and packet_type <= SHAKE_DATA_NVN:
		if dev.eventcallback != None:
			type = -1
			if packet[3] == 'U':
				type = SHAKE_NAV_UP
			elif packet[3] == 'D':
				type = SHAKE_NAV_DOWN
			elif packet[3] == 'C':
				type = SHAKE_NAV_CENTRE
			elif packet[3] == 'N':
				type = SHAKE_NAV_NORMAL
			dev.eventcallback(type)
	elif packet_type == SHAKE_DATA_CU0:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS0_UPPER)
	elif packet_type == SHAKE_DATA_CL0:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS0_LOWER)
	elif packet_type == SHAKE_DATA_CU1:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS1_UPPER)
	elif packet_type == SHAKE_DATA_CL1:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS1_LOWER)
	elif packet_type == SHAKE_DATA_SHAKING:
		shake_update_shaking(dev, packet)
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_SHAKING_EVENT)
	elif packet_type == SHAKE_DATA_HEART_RATE:
		shake_update_heart_rate(dev, packet)
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_HEART_RATE_EVENT)
	elif packet_type == SHAKE_DATA_RFID_TID:
		dev.lasttid = packet[5:21]
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_RFID_TID_EVENT)
	else:
		return SHAKE_ERROR

	return SHAKE_SUCCESS

def shake_update_data_normal(dev, packet_type, packet):
	# update data member of dev depending on value of <type>, reading info from <packet>
	# would be nice if python had a switch statement here ...
	debug("shake_update_data_normal: " + str(packet_type) + "|" + packet)
	if packet_type == SHAKE_DATA_ACC:
		shake_update_acc(dev, packet)
	elif packet_type == SHAKE_DATA_GYRO:
		shake_update_gyr(dev, packet)
	elif packet_type == SHAKE_DATA_MAG:
		shake_update_mag(dev, packet)
	elif packet_type == SHAKE_DATA_HEADING:
		shake_update_heading(dev, packet)
	elif packet_type == SHAKE_DATA_CAP0:
		shake_update_cap0(dev, packet)
	elif packet_type == SHAKE_DATA_CAP1:
		shake_update_cap1(dev, packet)
	elif packet_type == SHAKE_DATA_ANA0:
		shake_update_ana0(dev, packet)
	elif packet_type == SHAKE_DATA_ANA1:
		shake_update_ana1(dev, packet)
	elif packet_type >= SHAKE_DATA_NVU and packet_type <= SHAKE_DATA_NVN:
		if dev.eventcallback != None:
			type = -1
			if packet[3] == 'U':
				type = SHAKE_NAV_UP
			elif packet[3] == 'D':
				type = SHAKE_NAV_DOWN
			elif packet[3] == 'C':
				type = SHAKE_NAV_CENTRE
			elif packet[3] == 'N':
				type = SHAKE_NAV_NORMAL
			dev.eventcallback(type)
	elif packet_type == SHAKE_DATA_CU0:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS0_UPPER)
	elif packet_type == SHAKE_DATA_CL0:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS0_LOWER)
	elif packet_type == SHAKE_DATA_CU1:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS1_UPPER)
	elif packet_type == SHAKE_DATA_CL1:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_CS1_LOWER)
	elif packet_type == SHAKE_DATA_SHAKING:
		shake_update_shaking(dev, packet)
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_SHAKING_EVENT)
	elif packet_type == SHAKE_DATA_HEART_RATE:
		shake_update_heart_rate(dev, packet)
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_HEART_RATE_EVENT)
	elif packet_type == SHAKE_DATA_RFID_TID:
		dev.lasttid = packet[5:21]
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_RFID_TID_EVENT)
	else:
		return SHAKE_ERROR

	debug("shake_update_data_normal: done")
	return SHAKE_SUCCESS

# converts the 16-bit signed raw data values into Python ints
def convert_raw_data_value(bytes):
	lsb = ord(bytes[0])
	msb = ord(bytes[1])

	if msb & 0x80:
		return (lsb + (msb << 8)) - 65536
	else:
		return lsb + (msb << 8)

def shake_update_data_raw(dev, packet_type, packet, has_seq):
	# update data member of dev depending on value of <type>, reading info from <packet>
	# would be nice if python had a switch statement here ...

	#if len(packet) == 10 or len(packet) == 6 and ord(packet[-1:]) == 0x7F:
	#	dev.peek_flag = True
	#	dev.peek = 0x7F
	#else:
	#	dev.peek_flag = False

	debug("shake_update_data_raw: " + str(packet_type) + "|" + packet)

	#print packet + "\n"

	if packet_type == SHAKE_RAW_DATA_ACC:
		dev.data.accx = convert_raw_data_value(packet[3:5])
		dev.data.accy = convert_raw_data_value(packet[5:7])
		dev.data.accz = convert_raw_data_value(packet[7:9])
		if has_seq:
			dev.data.accseq = ord(packet[9:10])
	elif packet_type == SHAKE_RAW_DATA_GYRO:
		dev.data.gyrx = convert_raw_data_value(packet[3:5])
		dev.data.gyry = convert_raw_data_value(packet[5:7])
		dev.data.gyrz = convert_raw_data_value(packet[7:9])
		if has_seq:
			dev.data.gyrseq = ord(packet[9:10])
	elif packet_type == SHAKE_RAW_DATA_MAG:
		dev.data.magx = convert_raw_data_value(packet[3:5])
		dev.data.magy = convert_raw_data_value(packet[5:7])
		dev.data.magz = convert_raw_data_value(packet[7:9])
		if has_seq:
			dev.data.magseq = ord(packet[9:10])
	elif packet_type == SHAKE_RAW_DATA_HEADING:
		dev.data.heading = convert_raw_data_value(packet[3:5])
		if has_seq:
			dev.data.headingseq = ord(packet[5:6])
	elif packet_type == SHAKE_RAW_DATA_CAP0:
		dev.data.cap0 = convert_raw_data_value(packet[3:5])
		if has_seq:
			dev.data.cap0seq = ord(packet[5:6])
	elif packet_type == SHAKE_RAW_DATA_CAP1:
		dev.data.cap1 = convert_raw_data_value(packet[3:5])
		if has_seq:
			dev.data.cap1seq = ord(packet[5:6])
	elif packet_type == SHAKE_RAW_DATA_ANALOG0:
		dev.data.ana0 = convert_raw_data_value(packet[3:5])
		if has_seq:
			dev.data.ana0seq = ord(packet[5:6])
	elif packet_type == SHAKE_RAW_DATA_ANALOG1:
		dev.data.ana1 = convert_raw_data_value(packet[3:5])
		if has_seq:
			dev.data.ana1seq = ord(packet[5:6])
	elif packet_type == SHAKE_RAW_DATA_EVENT:
		if dev.eventcallback != None:
			type = -1
			val = convert_raw_data_value(packet[3:5])
			if val == 1:
				type = SHAKE_NAV_UP
			elif val == 2:
				type = SHAKE_NAV_DOWN
			elif val == 3:
				type = SHAKE_NAV_CENTRE
			elif val == 4:
				type = SHAKE_NAV_NORMAL
			dev.eventcallback(type)
	elif packet_type == SHAKE_RAW_DATA_SHAKING:
		if dev.eventcallback != None:
			dev.eventcallback(SHAKE_SHAKING_EVENT)
	else:
		return SHAKE_ERROR

	debug("shake_update_data_raw: done")

	return SHAKE_SUCCESS

# 	The following set of functions update the members of dev.data for each of the various packet types
def shake_update_acc(dev, packet):
	accpacket = shake_data_acc_packet(packet)
	(dev.data.accx, dev.data.accy, dev.data.accz) = (accpacket.accx, accpacket.accy, accpacket.accz)
	dev.data.accseq = accpacket.seq

def shake_update_gyr(dev, packet):
	gyrpacket = shake_data_gyr_packet(packet)
	(dev.data.gyrx, dev.data.gyry, dev.data.gyrz) = (gyrpacket.gyrx, gyrpacket.gyry, gyrpacket.gyrz)
	dev.data.gyrseq = gyrpacket.seq

def shake_update_mag(dev, packet):
	magpacket = shake_data_mag_packet(packet)
	(dev.data.magx, dev.data.magy, dev.data.magz) = (magpacket.magx, magpacket.magy, magpacket.magz)
	dev.data.magseq = magpacket.seq

def shake_update_heading(dev, packet):
	headingpacket = shake_data_heading_packet(packet)
	dev.data.heading = headingpacket.heading
	dev.data.headingseq = headingpacket.seq

def shake_update_cap0(dev, packet):
	cappacket = shake_data_cap_packet(packet)
	dev.data.cap0 = cappacket.prox
	dev.data.cap0seq = cappacket.seq
	
def shake_update_cap1(dev, packet):
	cappacket = shake_data_cap_packet(packet)
	dev.data.cap1 = cappacket.prox
	dev.data.cap1seq = cappacket.seq

def shake_update_ana0(dev, packet):
	anapacket = shake_data_analog_packet(packet)
	dev.data.ana0 = anapacket.voltage
	dev.data.ana0seq = anapacket.seq

def shake_update_ana1(dev, packet):
	anapacket = shake_data_analog_packet(packet)
	dev.data.ana1 = anapacket.voltage
	dev.data.ana1seq = anapacket.seq
	
def shake_update_shaking(dev, packet):
	shakingpacket = shake_data_shaking_packet(packet)
	dev.data.peakaccel = shakingpacket.peakaccel
	dev.data.direction = shakingpacket.direction
	dev.data.timestamp = shakingpacket.timestamp

def shake_update_heart_rate(dev, packet):
	hrpacket = shake_heart_rate_packet(packet)
	dev.data.hrbpm = hrpacket.bpm

def shake_update_rfid(dev, packet):
	rfid_packet = shake_rfid_packet(packet)
	dev.data.lasttid = rfid_packet.tid

def init_module():
	pass

init_module()
