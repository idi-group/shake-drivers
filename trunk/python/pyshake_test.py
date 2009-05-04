from pyshake import *
import time

# define an callback function for events from the SHAKE
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

# create a shake_device object
sd = shake_device(SHAKE_SK7)

# connect using a COM port number (pyserial uses port numbers starting at 0, so
# this is actually saying "use COM3")
# you can also use the other forms of port identifier that pyserial supports, eg
# sd.connect('COM3:')
sd.connect(40)

import time
time.sleep(5)

sd.write_data_format(2)

# turn on the accelerometer, capacitive sensors and button
if sd.write_power_state(SHAKE_POWER_ACC | SHAKE_POWER_CAP | SHAKE_POWER_NAV) != SHAKE_SUCCESS:
	print "Failed to set power state"

# set accelerometer sample rate to 20Hz
if sd.write_sample_rate(SHAKE_SENSOR_ACC, 20) != SHAKE_SUCCESS:
	print "Failed to set sample rate (ACC)"

# set capacitive sensor sample rate to 20Hz
if sd.write_sample_rate(SHAKE_SENSOR_CAP, 20) != SHAKE_SUCCESS:
	print "Failed to set sample rate (CAP)"

# display some accelerometer readings
for i in range(200):
	print "Accelerometer:", sd.acc(), "\r",
	time.sleep(0.01)
print "\n"

# display some capacitive sensor reading
for i in range(200):
	print "Cap sensors: ", sd.cap(), "\r",
	time.sleep(0.01)
print "\n"

# display information about the connected SHAKE
print "Serial number:", sd.info_serial_number()
print "Firmware version:", sd.info_firmware_revision()
print "Hardware version:", sd.info_hardware_revision()
print "Expansion module 1:", sd.info_module_name(sd.info_module(0))
print "Expansion module 2:", sd.info_module_name(sd.info_module(1))

# register event callback
sd.register_event_callback(eventcallback)

print "Try moving the button on the SHAKE..."
time.sleep(5)

# close the connection
sd.close()

