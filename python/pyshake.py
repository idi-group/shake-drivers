# Main pyshake module. To use, run "from pyshake import *"

import pyshake_packets, atexit
from pyshake_constants import *

# custom exception class for SHAKE code
class shake_error(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

# this function is registered to be called on exit from a Python 
# shell, and just calls the close method of any active shake_device
# instances
def cleanup():
	for i in shake_device.instances:
		if i != None:
			i.close()

atexit.register(cleanup)

# an instance of this class represents a single SHAKE device
class shake_device:
	shake_handle_count 	= 0
	instances = []

	def __init__(self):
			
		self.handle = shake_device.shake_handle_count
		shake_device.shake_handle_count += 1

		shake_device.instances.append(self)
		self.priv = None

	def connect(self, addr):
		if self.priv != None:
			self.priv.close()
			self.priv = None

		self.priv = pyshake_packets.shake_device_private(addr)

		elapsed = 0
		while not self.priv.synced and not self.priv.thread_done and elapsed < 10000:
			pyshake_packets.sleep(0.05)
			elapsed += 50

		if not self.priv.synced or elapsed >= 10000:
			raise shake_error("Failed to connect!")

	def dbgmode(self):
		return self.priv.dbgmode
		
	# 	Closes the connection associated with the instance
	def close(self):
		if self.priv:
			self.priv.close()
		try:
			shake_device.instances.remove(self)
		except:
			pass

	def data_timestamp(self, sensor):
		if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
			return -1

		return self.priv.data.timestamps[sensor]

	# 	Returns x-axis accelerometer reading
	def accx(self):
		x = self.priv.data.accx
		self.priv.data.timestamps[SHAKE_SENSOR_ACC] = self.priv.data.accseq
		return x

	# 	Returns y-axis accelerometer reading
	def accy(self):
		y = self.priv.data.accy
		self.priv.data.timestamps[SHAKE_SENSOR_ACC] = self.priv.data.accseq
		return y

	# 	Returns z-axis accelerometer reading
	def accz(self):
		z = self.priv.data.accz
		self.priv.data.timestamps[SHAKE_SENSOR_ACC] = self.priv.data.accseq
		return z

	# 	Returns accelerometer readings in a list: [x, y, z]
	def acc(self):
		xyz = [self.priv.data.accx, self.priv.data.accy, self.priv.data.accz]
		self.priv.data.timestamps[SHAKE_SENSOR_ACC] = self.priv.data.accseq
		return xyz

	# 	Returns x-axis gyro reading
	def gyrx(self):
		x = self.priv.data.gyrx
		self.priv.data.timestamps[SHAKE_SENSOR_GYRO] = self.priv.data.gyrseq
		return x
	
	# 	Returns y-axis gyro reading
	def gyry(self):
		y = self.priv.data.gyry
		self.priv.data.timestamps[SHAKE_SENSOR_GYRO] = self.priv.data.gyrseq
		return y

	# 	Returns z-axis gyro reading
	def gyrz(self):
		z = self.priv.data.gyrz
		self.priv.data.timestamps[SHAKE_SENSOR_GYRO] = self.priv.data.gyrseq
		return z

	# 	Returns gyro readings in a list: [x, y, z]
	def gyro(self):
		xyz = [self.priv.data.gyrx, self.priv.data.gyry, self.priv.data.gyrz]
		self.priv.data.timestamps[SHAKE_SENSOR_GYRO] = self.priv.data.gyrseq
		return xyz
			
	# 	Returns x-axis mag reading
	def magx(self):
		x = self.priv.data.magx
		self.priv.data.timestamps[SHAKE_SENSOR_MAG] = self.priv.data.magseq
		return x

	# 	Returns y-axis mag reading
	def magy(self):
		y = self.priv.data.magy
		self.priv.data.timestamps[SHAKE_SENSOR_MAG] = self.priv.data.magseq
		return y

	# 	Returns z-axis mag reading
	def magz(self):
		z = self.priv.data.magz
		self.priv.data.timestamps[SHAKE_SENSOR_MAG] = self.priv.data.magseq
		return z

	# 	Returns mag readings in a list: [x, y, z]
	def mag(self):
		xyz = [self.priv.data.magx, self.priv.data.magy, self.priv.data.magz]
		self.priv.data.timestamps[SHAKE_SENSOR_MAG] = self.priv.data.magseq
		return xyz

	# 	Returns heading
	def heading(self):
		heading = self.priv.data.heading
		self.priv.data.timestamps[SHAKE_SENSOR_HEADING] = self.priv.data.headingseq
		return heading

	# 	Returns proximity value for cap switch 0
	def cap0(self):
		c = self.priv.data.cap0
		self.priv.data.timestamps[SHAKE_SENSOR_CAP0] = self.priv.data.cap0seq
		return c

	# 	Returns proximity value for cap switch 1
	def cap1(self):
		c = self.priv.data.cap1
		self.priv.data.timestamps[SHAKE_SENSOR_CAP1] = self.priv.data.cap1seq
		return c

	# 	Returns proximity values for cap switch [0, 1]
	def cap(self):
		cap = [self.priv.data.cap0, self.priv.data.cap1]
		self.priv.data.timestamps[SHAKE_SENSOR_CAP0] = self.priv.data.cap0seq
		self.priv.data.timestamps[SHAKE_SENSOR_CAP1] = self.priv.data.cap1seq
		return cap

	# 	Returns value of analog input 0
	def analog0(self):
		a = self.priv.data.ana0
		self.priv.data.timestamps[SHAKE_SENSOR_ANA0] = self.priv.data.ana0seq
		return a

	# 	Returns value of analog input 1
	def analog1(self):
		a = self.priv.data.ana1
		self.priv.data.timestamps[SHAKE_SENSOR_ANA1] = self.priv.data.ana1seq
		return a

	# 	Returns value of both analog inputs as [analog0, analog1]
	def analog(self):
		ana = [self.priv.data.ana0, self.priv.data.ana1]
		self.priv.data.timestamps[SHAKE_SENSOR_ANA0] = self.priv.data.ana0seq
		self.priv.data.timestamps[SHAKE_SENSOR_ANA1] = self.priv.data.ana1seq
		return ana

	def shaking_peakaccel(self):
		return self.priv.data.peakaccel

	def shaking_direction(self):
		return self.priv.data.direction

	def shaking_timestamp(self):
		return self.priv.data.timestamp
	
	def info_retrieve(self):
		if self.priv.fwrev == None:
			if self.write_data_request(1) == SHAKE_ERROR:
				return SHAKE_ERROR
			count = 0
			while count < 1000 and not self.priv.read_startup:
				count+=1
				pyshake_packets.ssleep(0.001)
		return SHAKE_SUCCESS

	def info_firmware_revision(self):
		self.info_retrieve()
		return self.priv.fwrev

	def info_hardware_revision(self):
		self.info_retrieve()
		return self.priv.hwrev

	def info_serial_number(self):
		self.info_retrieve()
		return self.priv.serial
	
	def info_module_slot1(self):
		self.info_retrieve()
		return self.priv.slot1 
	
	def info_module_slot2(self):
		self.info_retrieve()
		return self.priv.slot2
	
	def info_module_name(self, type):
		if type < 0 or type >= len(self.priv.modules):
			return "unknown module type"
		return self.priv.modules[type]

	# 	Reads register <address> and returns a 2-tuple: (success/error, value), where
	# 	the first item in the tuple is SHAKE_SUCCESS or SHAKE_ERROR depending on the
	# 	result of the operation. value will be 0 on error, otherwise it will be the
	# 	current contents of the specified register.
	def read(self, address):
		scp = '$REA,%04X,00' % (address)

		if self.priv.waiting_for_ack:
			pyshake_packets("read() already waiting for ack")
			return SHAKE_ERROR

		self.priv.write_to_port(scp)
		pyshake_packets.debug("SENT: " + scp)
		
		self.priv.waiting_for_ack_signal = True
		self.priv.waiting_for_ack = True
		
		timeout = 250
		while self.priv.waiting_for_ack_signal:
			pyshake_packets.ssleep(0.001)
			timeout -= 1
			if timeout == 0:
				break
			
		self.priv.waiting_for_ack = False
		
		if not self.priv.lastack:
			return (SHAKE_ERROR, 0)
		
		self.lastack = False

		return (SHAKE_SUCCESS, self.priv.lastval)

	# 	Writes <value> into register <address> and returns either SHAKE_ERROR or
	# 	SHAKE_SUCCESS to indicate the result.
	def write(self, address, value):
		scp = '$WRI,%04X,%02X' % (address, value)
		
		if self.priv.waiting_for_ack:
			pyshake_packets("write() already waiting for ack")
			return SHAKE_ERROR
		
		self.priv.write_to_port(scp)
		pyshake_packets.debug("+++ SENT: " + scp)
		
		self.priv.waiting_for_ack_signal = True
		self.priv.waiting_for_ack = True
		
		timeout = 250
		while timeout != 0 and self.priv.waiting_for_ack_signal:
			pyshake_packets.ssleep(0.001)
			timeout -= 1
			
		pyshake_packets.debug("+++ ACK WAIT OVER timeout = " + str(timeout))
			
		self.priv.waiting_for_ack = False
		if not self.priv.lastack:
			pyshake_packets.debug("write() failed to get ACK")
			return SHAKE_ERROR
		
		self.lastack = False

		return SHAKE_SUCCESS

	def register_event_callback(self, callback):
		self.priv.eventcallback = callback

	# 	All functions below here are used to read/write the configuration
	# 	registers on the SHAKE. The constants referred to in the comments
	# 	can all be found in the pyshake_constants.py file.

	# 	Read or write the NV_REG_POWER1 register. This controls power to the
	# 	various sensors on the device. When writing a value, use a logical
	# 	OR of one or more of the SHAKE_POWER_* constants. Eg to turn on the
	# 	accelerometers and gyros and disable everything else, use:
	# 	SHAKE_POWER_ACC | SHAKE_POWER_GYR
	# 
	# 	Reading from the register will return a 2-tuple consisting of a 
	# 	return code (either SHAKE_SUCCESS or SHAKE_ERROR) followed by
	# 	the current value of the register.
	def read_power_state(self):
		return self.read(NV_REG_POWER1)

	def write_power_state(self, value):
		if value < 0 or value > 255:
			return SHAKE_ERROR
		return self.write(NV_REG_POWER1, value)

	def read_power_state_2(self):
		return self.read(NV_REG_POWER1)

	def write_power_state_2(self, value):
		if value < 0 or value > 255:
			return SHAKE_ERROR
		return self.write(NV_REG_POWER2, value)

	# 	Read or write the NV_REG_DATAFMT register. This controls the output
	# 	format of the sensor data (either ASCII (default) or raw mode
	#
	# 	When writing to the register, the value should either be 
	# 	SHAKE_DATAFMT_ASCII or SHAKE_DATAFMT_RAW, other values will be
	# 	rejected. 
	# 	
	# 	Reading from the register will return a 2-tuple consisting of a 
	# 	return code (either SHAKE_SUCCESS or SHAKE_ERROR) followed by
	# 	the current value of the register, which will be one of the two
	# 	values detailed above. 
	def read_data_format(self):
		return self.read(NV_REG_DATAFMT)

	def write_data_format(self, value):
		return self.write(NV_REG_DATAFMT, value)

	def read_acc_config(self):
		return self.read(NV_REG_ACCCONFIG)

	def write_acc_config(self, value):
		return self.write(NV_REG_ACCCONFIG, value)

	# 	Read or write the registers which control output sample rates for the
	# 	different sensors on the SHAKE. In both cases, the <sensor> parameter
	# 	should be set to one of the SHAKE_SENSOR_* constants (eg SHAKE_SENSOR_ACC) 
	#
	# 	When writing to the register for a particular sensor, <value> must be
	# 	between 0 and the corresponding SHAKE_<sensor>_MAX_RATE limit. For example
	#  	the accelerometer sampling range is between 0 and SHAKE_ACC_MAX_RATE. Values
	# 	outside this range will be rejected.
	# 	
	# 	Reading from the register will return a 2-tuple consisting of a 
	# 	return code (either SHAKE_SUCCESS or SHAKE_ERROR) followed by
	# 	the current sample rate for the selected sensor.
	def read_sample_rate(self, sensor):
		if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
			return SHAKE_ERROR

		return self.read(NV_REG_ACCOUT + sensor)

	def write_sample_rate(self, sensor, value):
		if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
			return SHAKE_ERROR
	
		return self.write(NV_REG_ACCOUT + sensor, value)

	def read_digital_filter(self, sensor):
		if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
			return (SHAKE_ERROR, 0)

		return self.read(NV_REG_DIGFIL_ACC + sensor)

	def write_digital_filter(self, sensor, value):
		if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
			return SHAKE_ERROR
		
		return self.write(NV_REG_DIGFIL_ACC + sensor, value)

	def read_calib_bypass(self):
		return self.read(NV_REG_CALIB_BYPASS)

	def write_calib_bypass(self, value):
		return self.write(NV_REG_CALIB_BYPASS, value)

	def read_cx_bypass(self):
		return self.read(NV_REG_CX_CALIB_BYPASS)

	def write_cx_bypass(self, value):
		return self.write(NV_REG_CX_CALIB_BYPASS, value)

	def read_temp_compensation(self):
		return self.read(NV_REG_TEMP_COMPENSATION)

	def write_temp_compensation(self, value):
		return self.write(NV_REG_TEMP_COMPENSATION, value)

	def read_packet_streaming(self):
		return self.read(NV_REG_STREAM_DISABLE)

	def write_packet_streaming(self, value):
		return self.write(NV_REG_STREAM_DISABLE, value)

	def read_cs0_inc(self):
		return self.read(NV_REG_CS0_INC)

	def write_cs0_inc(self, value):
		return self.write(NV_REG_CS0_INC, value)

	def read_cs0_dec(self):
		return self.read(NV_REG_CS0_DEC)

	def write_cs0_dec(self, value):
		return self.write(NV_REG_CS0_DEC, value)

	def read_cs0_inc_profile(self):
		return self.read(NV_REG_CS0_INC_PROFILE)

	def write_cs0_inc_profile(self, value):
		return self.write(NV_REG_CS0_INC_PROFILE, value)

	def read_cs0_dec_profile(self):
		return self.read(NV_REG_CS0_DEC_PROFILE)
	
	def write_cs0_dec_profile(self, value):
		return self.write(NV_REG_CS0_DEC_PROFILE, value)

	def read_cs1_inc(self):
		return self.read(NV_REG_CS1_INC)

	def write_cs1_inc(self, value):
		return self.write(NV_REG_CS1_INC, value)

	def read_cs1_dec(self):
		return self.read(NV_REG_CS1_DEC)

	def write_cs1_dec(self, value):
		return self.write(NV_REG_CS1_DEC, value)

	def read_cs1_inc_profile(self):
		return self.read(NV_REG_CS1_INC_PROFILE)
	
	def write_cs1_inc_profile(self, value):
		return self.write(NV_REG_CS1_INC_PROFILE, value)

	def read_cs1_dec_profile(self):
		return self.read(NV_REG_CS1_DEC_PROFILE)

	def write_cs1_dec_profile(self, value):
		return self.write(NV_REG_CS1_DEC_PROFILE, value)
	
	def read_audio_config(self):
		return self.read(NV_REG_AUDIO_CONFIG)

	def write_audio_config(self, value):
		return self.write(NV_REG_AUDIO_CONFIG, value)

	def write_midi_amplitude(self, value):
		return self.write(VO_REG_MIDI_AMPLITUDE, value)

	def write_midi_note(self, value):
		return self.write(VO_REG_MIDI_NOTE, value)

	def write_midi_waveform(self, value):
		return self.write(VO_REG_MIDI_WAVEFORM, value)

	def read_cap_thresholds(self):
		vals = []
		vals.append(self.read(NV_REG_CS0_INC)[1])
		vals.append(self.read(NV_REG_CS0_DEC)[1])
		vals.append(self.read(NV_REG_CS0_INC_PROFILE)[1])
		vals.append(self.read(NV_REG_CS0_DEC_PROFILE)[1])
		vals.append(self.read(NV_REG_CS1_INC)[1])
		vals.append(self.read(NV_REG_CS1_DEC)[1])
		vals.append(self.read(NV_REG_CS1_INC_PROFILE)[1])
		vals.append(self.read(NV_REG_CS1_DEC_PROFILE)[1])
		return vals

	def write_cap_thresholds(self, values):
		self.write(NV_REG_CS0_INC, values[0])
		self.write(NV_REG_CS0_DEC, values[1])
		self.write(NV_REG_CS0_INC_PROFILE, values[2])
		self.write(NV_REG_CS0_DEC_PROFILE, values[3])
		self.write(NV_REG_CS1_INC, values[4])
		self.write(NV_REG_CS1_DEC, values[5])
		self.write(NV_REG_CS1_INC_PROFILE, values[6])
		self.write(NV_REG_CS1_DEC_PROFILE, values[7])
		return SHAKE_SUCCESS

	# Logging: note no playback capability on s60 for now, or logging_packets_read() function
	def logging_pause(self):
		return self.write(VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PAUSE)

	def logging_stop(self):
		return self.write(VO_REG_LOGGING_CTRL, SHAKE_LOGGING_STOP)

	def logging_record(self):
		return self.write(VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RECORD)

	def logging_reset(self):
		return self.write(VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RESET)

	def logging_status(self):
		ret = self.read(VO_REG_LOGGING_STATUS)
		if ret[0] == SHAKE_ERROR:
			return SHAKE_ERROR

		return ret[1]

	def logging_packet_count(self):
		ret = self.read(NV_REG_LOGGING_PKT_LSB)
		if ret[0] == SHAKE_ERROR:
			return SHAKE_ERROR
		lsb = ret[1]

		ret = self.read(NV_REG_LOGGING_PKT_MSB)
		if ret[0] == SHAKE_ERROR:
			return SHAKE_ERROR
		msb = ret[1]

		return 100 * ((msb << 8) + lsb)
	
	# Shaking configuration
	def read_shaking_config(self):
		return self.read(NV_REG_SHAKING_CONFIG)

	def write_shaking_config(self, value):
		return self.write(NV_REG_SHAKING_CONFIG, value)

	def read_shaking_accel_threshold(self):
		return self.read(NV_REG_SHAKING_ACCEL_THRESHOLD)

	def write_shaking_accel_threshold(self, value):
		return self.write(NV_REG_SHAKING_ACCEL_THRESHOLD, value)

	def read_shaking_holdoff_time(self):
		return self.read(NV_REG_SHAKING_HOLDOFF_TIME)

	def write_shaking_holdoff_time(self, value):
		return self.write(NV_REG_SHAKING_HOLDOFF_TIME, value)

	def read_shaking_vibration_profile(self):
		return self.read(NV_REG_SHAKING_VIBRATION_PROFILE)

	def write_shaking_vibration_profile(self, value):
		return self.write(NV_REG_SHAKING_VIBRATION_PROFILE, value)

	def read_shaking_hpf_constant(self):
		return self.read(NV_REG_SHAKING_HPF_CONSTANT)

	def write_shaking_hpf_constant(self, value):
		return self.write(NV_REG_SHAKING_HPF_CONSTANT, value)

	def read_shaking_lpf_constant(self):
		return self.read(NV_REG_SHAKING_LPF_CONSTANT)

	def write_shaking_lpf_constant(self, value):
		return self.write(NV_REG_SHAKING_LPF_CONSTANT, value)

	def reset_shaking_detection(self):
		return self.write(NV_REG_SHAKING_CONFIG, 0xFF)
		return self.write(NV_REG_SHAKING_ACCEL_THRESHOLD, 0x0A)
		return self.write(NV_REG_SHAKING_HOLDOFF_TIME, 0x06)
		return self.write(NV_REG_SHAKING_VIBRATION_PROFILE, 0x00)
		return self.write(NV_REG_SHAKING_HPF_CONSTANT, 0x20)
		return self.write(NV_REG_SHAKING_LPF_CONSTANT, 0x06)

	def read_heart_rate_config(self):
		return self.read(NV_REG_HEART_RATE_CONFIG)
	
	def write_heart_rate_config(self, value):
		return self.write(NV_REG_HEART_RATE_CONFIG, value)

	def read_expansion_config(self):
		return self.read(NV_REG_EXPANSION_CONFIG)
	
	def write_expansion_config(self, value):
		return self.write(NV_REG_EXPANSION_CONFIG, value)

	def read_rfid_config(self):
		return self.read(NV_REG_RFID_CONFIG)

	def write_rfid_config(self, value):
		return self.write(NV_REG_RFID_CONFIG, value)

	def read_rfid_scan_freq(self):
		return self.read(NV_REG_RFID_FREQUENCY)

	def write_rfid_scan_freq(self, value):
		return self.write(NV_REG_RFID_FREQUENCY)

	def heart_rate(self):
		return self.priv.data.hrbpm
	
	def rfid_tid(self):
		return self.priv.lasttid

	def read_temperature(self):
		ret = self.read(VO_REG_TEMPERATURE)
		if ret[0] == SHAKE_ERROR:
			return -1.0

		return ret[1] / 4.0

	# 	Writes a value to the VO_REG_PKTREQ register (write-only). This register 
	# 	allows you to request a set of data from one or more sensors (in addition
	# 	to or instead of the usual data stream from the device). 
	#
	# 	When writing to the register, the value you use should be a logical OR of
	# 	one or more SHAKE_REQ_<sensor> constants, eg SHAKE_REQ_ACC | SHAKE_REQ_GYR
	# 	to request both accelerometer and gyro packets.
	#
	# 	The data will be sent and processed in the usual way, so you should use the
	# 	data access functions above to read it.
	def write_packet_request(self, value):
		return self.write(VO_REG_PKTREQ, value)

	def write_data_request(self, value):
		return self.write(VO_REG_DATAREQ, value)

	def playvib(self, channel, profile):
		if channel < SHAKE_VIB_MAIN or channel > SHAKE_VIB_RIGHT:
			return SHAKE_ERROR

		if profile < 0 or profile > SHAKE_VIB_PROFILE_MAX:
			return SHAKE_ERROR

		return self.write(VO_REG_VIB_MAIN + channel, profile)
	
	def playvib_continuous(self, channel, amplitude, time):
		if channel != SHAKE_VIB_LEFT and channel != SHAKE_VIB_RIGHT:
			return SHAKE_ERROR

		if amplitude != 0 and amplitude != 33 and amplitude != 66 and amplitude != 100:
			return SHAKE_ERROR

		if time < 0 or time > 64:
			return SHAKE_ERROR

		vibbyte = 0
		if amplitude == 0:
			vibbyte = 0x00
		elif amplitude == 33:
			vibbyte = 0x40;
		elif amplitude == 66:
			vibbyte = 0x80;
		elif amplitude == 100:
			vibbyte = 0xc0;
		
		vibbyte += time;

		vibaddr = VO_REG_VIB_LEFT_CONTINUOUS;
		if(channel == SHAKE_VIB_RIGHT):
			vibaddr = VO_REG_VIB_RIGHT_CONTINUOUS;
		return self.write(vibaddr, vibbyte)

	def upload_vib_sample(self, profile, samples):
		return self.upload_vib_sample_extended(profile, samples, 0, 0, 0)

	def upload_vib_sample_extended(self, profile, samples, mode, freq, duty):
		if profile < 0 or profile > SHAKE_VIB_PROFILE_MAX:
			return SHAKE_ERROR

		if len(samples) < 2 or (len(samples) % 2 != 0):
			return SHAKE_ERROR

		pyshake_packets.debug("uploading to location " + str(profile))

		svp = '$VIB,%02X,%02X,%02X,%02X' % (profile, mode, freq, duty)

		i = 0
		while i < len(samples):
			s = ',%02X,%02X' % (samples[i], samples[i+1])
			svp += s
			i += 2

		svp += '~'
		pyshake_packets.debug("sending upload: " + svp + "\n")

		self.priv.waiting_for_ack = True
		self.priv.lastaddr = -1

		self.priv.write_to_port(svp)
		
		elapsed = 0
		while self.priv.waiting_for_ack and elapsed < 2000:
			pyshake_packets.sleep(0.01)
			elapsed += 10 
							
		self.priv.waiting_for_ack = False
	
		if not self.priv.lastack or self.priv.lastaddr == -1:
			return SHAKE_ERROR

		return SHAKE_SUCCESS


	def read_battery_level(self):
		return self.read(VO_REG_BATTERY)

	# 	Reads from the VO_REG_PWRSTA register (read-only). 
	# 	This register contains the power status of the SHAKE device, made up from 
	# 	three different values:
	# 		1. External power on/off (AC adapter)
	# 		2. Battery charging/not charging
	# 		3. Battery charged/not fully charged
	# 	The value returned from the function will be a logical OR of one or more
	# 	of the following three constants, corresponding to the list above:
	# 		1. SHAKE_EXT_POWER
	# 		2. SHAKE_BAT_CHARGING
	# 		3. SHAKE_BAT_FULL
	# 	To test for each condition, simply use a logical AND with the appropriate
	# 	constant. For example, to test for external power:
	# 		val = shake.read_power_status()
	# 		if val & SHAKE_EXT_POWER:
	# 			print "external power on"
	def read_power_status(self):
		return self.read(VO_REG_PWRSTA)
