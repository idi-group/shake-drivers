# SHAKE serial port wrapper class for Symbian
import os

class pyshake_serial_error(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

import socket, e32
import appuifw
from e32 import ao_yield
from e32 import ao_sleep
# this should allow the phone to redraw the screen after importing the modules, which
# can take some time depending on the phone
ao_yield()

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
		self.target = None
		self.connected = False
		if bt_addr.find(":") == -1:
			self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			t = bt_addr.split("|")
			self.target = (t[0], int(t[1]))
		else:
			self.sock = socket.socket(socket.AF_BT, socket.SOCK_STREAM)
			self.target = (bt_addr, 1)

	def open(self):
		if self.connected:
			appuifw.note(u'Already connected!')
			return False
			
		self.sock.connect(self.target)

		self.connected = True
		return True

	def close(self):
		if not self.connected:
			appuifw.note(u'Already disconnected!')
			return False

		
		self.sock.shutdown(2)
		self.sock = None

		self.target = None
		return True

	def read(self, bytes_to_read):
		if not self.sock:
			return ""
		bytes_read = 0
		data = ""
		#while bytes_read < bytes_to_read:
		newdata = self.sock.recv(bytes_to_read - bytes_read)
		bytes_read += len(newdata)
		data += newdata
		return data

	def write(self, bytes):
		if not self.sock:
			return 0
		sentbytes = 0
		bytes_to_send = len(bytes)

		sentbytes = self.sock.send(bytes)
		return sentbytes

