# Copyright (c) 2006-2009, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of 
#		conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list
#		of conditions and the following disclaimer in the documentation and/or other
#	 	materials provided with the distribution.
#    * Neither the name of the University of Glasgow nor the names of its contributors 
#		may be used to endorse or promote products derived from this software without
# 		specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
# THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
# OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


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

