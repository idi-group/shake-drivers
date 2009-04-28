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
pyshake_packets.debugging = True
sd = shake_device()

# connect using a COM port number (pyserial uses port numbers starting at 0, so
# this is actually saying "use COM3")
# you can also use the other forms of port identifier that pyserial supports, eg
# sd.connect('COM3:')
sd.connect(39)

sd.write_data_format(2)

# turn on the accelerometer and button
if sd.write_power_state(SHAKE_POWER_ACC | SHAKE_POWER_NAV) != SHAKE_SUCCESS:
	print "Failed to set power state"

# set accelerometer sample rate to 50Hz
if sd.write_sample_rate(SHAKE_SENSOR_ACC, 3) != SHAKE_SUCCESS:
	print "Failed to set sample rate"

# display some accelerometer readings
for i in range(250):
	print "Accelerometer:", sd.acc(), "\r",
	time.sleep(0.01)
print "\n"

# display information about the connected SHAKE
print "Serial number:", sd.info_serial_number()
print "Firmware version:", sd.info_firmware_revision()
print "Hardware version:", sd.info_hardware_revision()
print "Expansion module 1:", sd.info_module_name(sd.info_module_slot1())
print "Expansion module 2:", sd.info_module_name(sd.info_module_slot2())

# register event callback
sd.register_event_callback(eventcallback)

print "Try moving the button on the SHAKE..."
time.sleep(5)

# close the connection
sd.close()

