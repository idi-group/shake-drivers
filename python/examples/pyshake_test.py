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

# Demonstrates various features of the SHAKE SK7

import time, sys, platform
from pyshake import *

# a callback function for events from the SHAKE
def eventcallback(event_type):
    if event_type == SHAKE_NAV_UP:
        print "Button UP"
    elif event_type == SHAKE_NAV_DOWN:
        print "Button DOWN"
    elif event_type == SHAKE_NAV_CENTRE:
        print "Button CENTRE"
    elif event_type == SHAKE_NAV_NORMAL:
        print "Button RELEASE"
    else:
        print "Event type =", event_type

# a callback function for data being streamed from the SHAKE
def datacallback(sensor_id, sensor_data, sensor_sequence_number):
    print 'Sensor ID:', sensor_id,
    if sensor_id == SHAKE_SENSOR_ACC:
        print '(Acc.)'
    elif sensor_id == SHAKE_SENSOR_MAG:
        print '(Mag.)'
    else:
        print '(Other)'
    print '\tData:', sensor_data
    print '\tSequence:', sensor_sequence_number

def configure_device(sd):
    # Usually the first thing you'll want to do is pick the data packet format.
    # Here we're using SK7_DATA_RAW_SEQ, which means "send data in a binary 
    # format, with sequence numbers in each packet". 
    # The other common choices here would be:
    # - SK7_DATA_ASCII (ASCII format, useful for debugging)
    # - SK7_DATA_RAW (same as SK7_DATA_RAW_SEQ but without sequence numbers)
    sd.write_data_format(SK7_DATA_RAW_SEQ)

    # Next, you'll need to make sure the sensors you want to use are powered 
    # on. The main sensors (accelerometer, gyro, magnetometer, capacitive, 
    # temperature, physical button and analog inputs, plus vibration feedback)
    # are all controlled by register 0x0000, which can be accessed either
    # using the basic write() function or the shortcut function called 
    # write_power_state(). In this case we're powering on the accelerometer,
    # magnetometer and physical button. You can use the value 0xFF to enable
    # everything, but this may have an impact on battery life!
    sd.write_power_state(SHAKE_POWER_ACC | SHAKE_POWER_MAG | SHAKE_POWER_NAV)

    # Once you've powered on the sensors, you should set a sample rate for them.
    # This does NOT affect the rate at which the sensors are sampled on the 
    # device itself as detailed in the user manual, it simply determines how
    # rapidly the data from each sensor is streamed to the host. The values
    # are given in Hz and can range from 0-255. The code below sets both the
    # accelerometer and magnetometer to 25Hz output (the physical button 
    # produces discrete events rather than a stream of data, see below).
    sd.write_sample_rate(SHAKE_SENSOR_ACC, 25)
    sd.write_sample_rate(SHAKE_SENSOR_MAG, 25)

    # To get events from the button (and some other sources), you need to 
    # register a callback with the driver. The function you supply will be 
    # called each time one of the supported events occurred, and should expect
    # a single parameter containing an integer event ID. 
    sd.register_event_callback(eventcallback)

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print "pyshake_test.py <device address>"
        print ""
        print "Examples:"
        print "pyshake_test.py 4 (connect to COM port 5 on Windows (pyserial port numbers are 0-based))"
        print "pyshake_test.py /dev/tty.SHAKESK7SN0077-SPPDev (connect to a specific device on OSX"
        sys.exit(-1)

    # create a shake_device object and attempt to create the connection
    sd = shake_device(SHAKE_SK7)

    try:
        param = int(sys.argv[1])
    except ValueError:
        param = sys.argv[1]

    if not sd.connect(param):
        print "Failed to connect!"
        sys.exit(-1)

    
    # Set up the device (power state, sensor parameters, ...)
    configure_device(sd)

    # You can retreive information about the connected SHAKE like this:
    print "Device info"
    print "==========="
    print "Serial number:", sd.info_serial_number()
    print "Firmware version:", sd.info_firmware_revision()
    print "Hardware version:", sd.info_hardware_revision()
    for i in range(sd.info_num_slots()):
       print "Expansion module %d: %s" % (i+1, sd.info_module_name(sd.info_module(i)))

    print "\nDisplaying some data..."
    # display some sensor readings...
    for i in range(500):
        ax, ay, az = sd.acc()
        mx, my, mz = sd.mag()
        print "Acc: %d %d %d | Mag: %d %d %d" % (ax, ay, az, mx, my, mz)
        time.sleep(0.01)
    print "\n"

    # Alternatively, register a callback for new data packets and display
    # some data that way
    print "Registering data callback..."
    time.sleep(1)
    sd.register_data_callback(datacallback)
    time.sleep(2)
    # remove the callback
    sd.register_data_callback(None)

    # Test that button movements trigger the event callback
    print "Try moving the button on the SHAKE..."
    time.sleep(5)

    # A basic example of defining, uploading and playing a vibration sample
    # using the standard builtin actuator on the SK7
    print "Uploading and playing a vibration sample..."

    # turn off all other sensors and turn on the vibrator motor (this isn't
    # required, you can enable sensors and the actuator at the same time if 
    # you want to!)
    sd.write_power_state(SHAKE_POWER_VIB)

    # upload the samples to profile location 20. The sample list is made up of 
    # pairs of (amplitude, time) values which are passed to the actuator one
    # after the other. For details of the values see the documentation. This 
    # set of samples simply uses the maximum amplitude and time for each pair.
    samples = [SHAKE_VIB_SPEED_MAX, SHAKE_VIB_TIME_MAX] * 10
    sd.upload_vib_sample_extended(20, samples, 0, 0, 0)

    # playback the samples through the main onboard actuator
    sd.playvib(SHAKE_VIB_MAIN, 20)

    print "Closing the connection..."
    sd.close()

