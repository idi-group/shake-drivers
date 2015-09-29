# Copyright (c) 2006-2015, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of 
#           conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list
#           of conditions and the following disclaimer in the documentation and/or other
#           materials provided with the distribution.
#    * Neither the name of the University of Glasgow nor the names of its contributors 
#           may be used to endorse or promote products derived from this software without
#           specific prior written permission.
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


# SHAKE serial port wrapper class for systems that support pyserial

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

    def open(self, baud=230400):
        if self.connected:
            return False
        # open port with parameters required for the SHAKE
        # SK7 requires rate 460800, SK6 requires 230400
        try:
            self.port = serial.Serial(self.target, baudrate=baud, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE)
        except Exception as e:
            # TODO save exception info somewhere (for a getlasterror() function)
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
        try:
            while bytes_read < bytes_to_read:
                newdata = self.port.read(bytes_to_read - bytes_read)
                bytes_read += len(newdata)
                data += newdata
        except:
            # TODO save exception info somewhere (for a getlasterror() function)
            pass
        return data

    def write(self, bytes):
        if not self.port:
            return 0

        sentbytes = self.port.write(bytes)
        return sentbytes


