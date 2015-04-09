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

# Simple demo of logging data to the SHAKE memory and playing it back 
# to a local file

import time, sys, platform
from pyshake import *

done = False

# define an callback function for events from the SHAKE
def eventcallback(event_type):
    global done

    if event_type == SHAKE_NAV_UP:
        print "Button UP"
    elif event_type == SHAKE_NAV_DOWN:
        print "Button DOWN"
    elif event_type == SHAKE_NAV_CENTRE:
        print "Button CENTRE"
    elif event_type == SHAKE_NAV_NORMAL:
        print "Button RELEASE"
    elif event_type == SHAKE_PLAYBACK_COMPLETE:
        # this event signals that the log data playback
        # has been completed and the output file is closed
        print 'streaming completed'
        done = True
    else:
        print "Event type =", event_type

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
        pass

    if not sd.connect(param):
        print "Failed to connect!"
        sys.exit(-1)

    sd.write_data_format(2)

    # configure the sensors you want to log and turn off any you don't need
    # to save memory ...
    sd.write_power_state(0xFF)
    sd.write_sample_rate(SHAKE_SENSOR_ACC, 20)
    sd.write_sample_rate(SHAKE_SENSOR_MAG, 20)

    # register a callback so we can get the "Logging completed" event
    sd.register_event_callback(eventcallback)

    sd.logging_stop()
    print "stop"
    sd.logging_reset()
    print "reset"
    sd.logging_record()
    print "record"
    time.sleep(3) # record for a few seconds
    sd.logging_stop()
    print "stop"
    sd.logging_play('log.txt')
    print "play"

    while not done:
        time.sleep(0.1)
    
    print "done"
    sd.close()

