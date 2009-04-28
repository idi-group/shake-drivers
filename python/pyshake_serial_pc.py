# SHAKE serial port wrapper class for systems that support pyserial
import os

class pyshake_serial_error(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

# pyserial
import serial

class base_serial_port:
	def __init__(self):
		pass

	def open(self):
		pass

	def close(self):
		pass

	def read(self, bytes_to_read):
		pass

	def write(self, bytes):
		pass

class serial_port(base_serial_port):

	def __init__(self, bt_addr):
		self.target = bt_addr
		self.connected = False
        
		self.port = None 

	def open(self):
		if self.connected:
			return False
			
		# open port with parameters required for the SHAKE
		try:
			self.port = serial.Serial(self.target, baudrate=230400, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
		except:
			print "exception opening port"
			return False

		self.connected = True
		return True

	def close(self):
		if not self.connected:
			return False

		self.port.close()
		self.port = None

		self.target = None
		return True

	def read(self, bytes_to_read):
		if not self.port:
			return ""
		bytes_read = 0
		data = ""
		while bytes_read < bytes_to_read:
			newdata = self.port.read(bytes_to_read - bytes_read)
			bytes_read += len(newdata)
			data += newdata
		return data

	def write(self, bytes):
		if not self.port:
			return 0
		sentbytes = 0
		bytes_to_send = len(bytes)

		sentbytes = self.port.write(bytes)
		return sentbytes


