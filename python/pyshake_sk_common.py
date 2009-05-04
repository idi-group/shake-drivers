from pyshake_constants import *

def convert_raw_data_value(bytes):
	lsb = ord(bytes[0])
	msb = ord(bytes[1])
	
	if msb & 0x80:
		return (lsb + (msb << 8)) - 65536
	else:
		return lsb + (msb << 8)

class sk_sensor_data:
	def __init__(self):
		self.accx, self.accy, self.accz = 0,0,0
		self.gyrx, self.gyry, self.gyrz = 0,0,0
		self.magx, self.magy, self.magz = 0,0,0
		self.heading = 0
		self.cap_sk6 = [0 for x in range(2)]
		self.cap_sk7 = [0 for x in range(12)]
		self.ana0, self.ana1 = 0,0
		self.shaking_peak_accel, self.shaking_direction, self.shaking_timestamp = 0,0,0
		self.hr_bpm = 0.0
		self.timestamps = [0 for x in range(8)]
		self.internal_timestamps = [0 for x in range(8)]
		self.sk6seq = 0
		self.sk7seq = 0
		self.hrseq = 0

class SHAKE:
	def __init__(self, shakedev, devtype):
		self.__shake = shakedev
		self.device_type = devtype
		self.data = sk_sensor_data()

	def parse_ascii_packet(self, packet_type, packetbuf, playback, timestamp_packet):
		pass

	def read_ascii_packet(self, packet_type, packetbuf):
		pass

	def parse_raw_packet(self, packet_type, packetbuf, has_seq):
		pass

	def read_raw_packet(self, packet_type, packetbuf):
		pass

	def get_next_packet(self):
		pass

	def parse_packet(self, packetbuf, packet_type):
		pass

	def is_ascii_packet(self, packet_type):
		pass

	def parse_ack_packet(self, packetbuf, addr, val):
		pass

	def extract_ascii_packet(self, packet_type, packetbuf, playback, timestamp_packet):
		pass

	def extract_raw_packet(self, packet_type, packetbuf, has_seq):
		pass

	def classify_packet_header(self, packetbuf, ascii_packet):
		pass

	def read_device_info(self):
		pass
