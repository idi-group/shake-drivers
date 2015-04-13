# Copyright (c) 2006-2015, University of Glasgow
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are
# permitted provided that the following conditions are met:
#
#    * Redistributions of source code must retain the above copyright notice, this list of 
#               conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright notice, this list
#               of conditions and the following disclaimer in the documentation and/or other
#               materials provided with the distribution.
#    * Neither the name of the University of Glasgow nor the names of its contributors 
#               may be used to endorse or promote products derived from this software without
#               specific prior written permission.
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

# in case you wanna test the lastest module dev
# import sys
# sys.path.insert(1,'/Users/toine/Documents/glasgow/dev/shake-drivers/python')
import time
import pyshake as ps

dev = ps.ShakeDevice()
adr_usb = '/dev/tty.usbserial-SHAKESK7'


def setup_module(module):
    print 'setup'
    assert dev.connect(adr_usb)

def teardown_module(module):
    print 'teardown'
    assert dev.close()

def test_power_up():
    conf = ps.SHAKE_POWER_ACC | ps.SHAKE_POWER_MAG | ps.SHAKE_POWER_NAV
    assert ps.SHAKE_SUCCESS == dev.write_power_state(conf)
    assert (ps.SHAKE_SUCCESS, conf) == dev.read_power_state()

def test_write_sample_rate():
    assert ps.SHAKE_SUCCESS == dev.write_sample_rate(ps.SHAKE_SENSOR_ACC, 20)
    assert ps.SHAKE_SUCCESS == dev.write_sample_rate(ps.SHAKE_SENSOR_MAG, 20)

def test_read_data():
    for i in range(200):
        x, y, z = dev.acc()
        # assert (x, y, z) != (0, 0, 0)
        print "Accelerometer: %04d %04d %04d \r" % (x, y, z),
        time.sleep(0.01)
    for i in range(200):
        x, y, z = dev.mag()
        # assert (x, y, z) != (0, 0, 0)
        print "Magnetometer: %04d %04d %04d \r" % (x, y, z),
        time.sleep(0.01)

def test_print_info():
        # display information about the connected SHAKE
    print "Serial number:", dev.info_serial_number()
    print "Firmware version:", dev.info_firmware_revision()
    print "Hardware version:", dev.info_hardware_revision()
    for i in range(5):
        print "Expansion module %d: %s" % (i+1, dev.info_module_name(dev.info_module(i)))

## could be used too
# class Test:
#     """docstring for Test"""
#     @classmethod
#     def setup_class(cls):
#         print 'setup_class'
#         assert dev.connect(adr_usb)

#     @classmethod
#     def teardown_class(cls):
#         print 'teardown_class'
#         assert dev.close()

#     def test_power_up(self):
#         conf = ps.SHAKE_POWER_ACC | ps.SHAKE_POWER_MAG | ps.SHAKE_POWER_NAV
#         assert ps.SHAKE_SUCCESS == dev.write_power_state(conf)
#         assert (ps.SHAKE_SUCCESS, conf) == dev.read_power_state()
