package SHAKE;

/* This class defines the public SHAKE API */

public class SHAKEDevice {
	
	private internalSHAKE priv = null;

	
	public SHAKEDevice(SHAKESerialInterface _port) {
		priv = new internalSHAKE(_port);
	}

	public boolean connect(String btaddress) {
		if(!priv.openConnection(btaddress))
			return false;
		
		return true;
	}

	public boolean connect(int com_port) {
		if(!priv.openConnection(com_port))
			return false;
		
		return true;
	}
	
	public boolean close() {
		registerEventListener(null);
		return priv.closeConnection();
	}
	
	public void waitForACKs(boolean wait_on) {
		priv.wait_for_acks = wait_on;
	}
	
	public void registerEventListener(SHAKEListener sl) {
		priv.setEventListener(sl);
	}

	public int accx() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ACC] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ACC]; 
		return priv.acc[0];
	}
	
	public int accy() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ACC] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ACC]; 
		return priv.acc[1];
	}
	
	public int accz() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ACC] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ACC]; 
		return priv.acc[2];
	}
	
	public int[] acc() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ACC] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ACC]; 
		return priv.acc;
	}
	
	public int gyrox() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO]; 
		return priv.gyro[0];
	}
	
	public int gyroy() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO];
		return priv.gyro[1];
	}
	
	public int gyroz() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO];
		return priv.gyro[2];
	}
	
	public int[] gyro() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_GYRO];
		return priv.gyro;
	}
	
	public int magx() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_MAG] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_MAG];
		return priv.mag[0];
	}
	
	public int magy() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_MAG] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_MAG];
		return priv.mag[1];
	}
	
	public int magz() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_MAG] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_MAG];
		return priv.mag[2];
	}
	
	public int[] mag() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_MAG] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_MAG];
		return priv.mag;
	}
	
	public int heading() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_HEADING] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_HEADING];
		return priv.heading;
	}
	
	public int cap0() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_CAP0] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP0];
		return priv.cap[0];
	}
	
	public int cap1() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_CAP1] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP1];
		return priv.cap[1];
	}
	
	public int[] cap() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_CAP0] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP0];
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_CAP1] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_CAP1];
		return priv.cap;
	}
	
	public int ana0() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ANA0] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA0];
		return priv.ana[0];
	}
	
	public int ana1() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ANA1] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA1];
		return priv.ana[1];
	}
	
	public int[] ana() {
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ANA0] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA0];
		priv.timestamps[SHAKEConstants.SHAKE_SENSOR_ANA1] = priv.current_timestamps[SHAKEConstants.SHAKE_SENSOR_ANA1];
		return priv.ana;
	}
	
	public int shaking_peakaccel() {
		return priv.shaking_peak_accel;
	}
	
	public int shaking_direction() {
		return priv.shaking_direction;
	}
	
	public int shaking_timestamp() {
		return priv.shaking_timestamp;
	}
	
	public int heart_rate() {
		return priv.hr_bpm;
	}
	
	public int data_timestamp(int sensor) {
		return priv.timestamps[sensor];
	}
	
	public void setDebugOut(SHAKEDebug dbg) {
		priv.dbg = dbg;
	}

	public float firmwareRevision() {
		return priv.fwrev;
	}
	
	public float hardwareRevision() {
		return priv.hwrev;
	}
	
	public String serialNumber() {
		return priv.serial;
	}
	
	public int expansionSlot1() {
		return priv.slot1;
	}
	
	public int expansionSlot2() {
		return priv.slot2;
	}
	
	public String expansionSlotByName(int type) {
		if(type < 0 || type >= priv.modules.length)
			return "";
		return priv.modules[type];
	}
	
	public int read(int reg_address) {
		return priv.readRegister(reg_address);
	}
	
	public boolean write(int reg, int val) {
		return priv.writeRegister(reg, val);
	}

	public int readPowerState() {
		return priv.readRegister(0);
	}

	public boolean writePowerState(int value) {
		return priv.writeRegister(0, value);
	}

	public int readPowerState2() {
		return priv.readRegister(1);
	}

	public boolean writePowerState2(int value) {
		return priv.writeRegister(1, value);
	}

	public int readDataFormat() {
		return priv.readRegister(2);
	}

	public boolean writeDataFormat(int value) {
		return priv.writeRegister(2, value);
	}

	public int readSampleRate(int sensor) {
		return priv.readRegister(8 + sensor);
	}

	public boolean writeSampleRate(int sensor, int  value) {
		return priv.writeRegister(8 + sensor, value);
	}
	
	public boolean uploadVibrationSample(int address, int[] samples) {
		if(samples.length % 2 != 0)
			return false;
		return priv.uploadVibrationSample(address, samples);
	}
	
	public boolean playvib(int address) {
		return priv.playvib(SHAKEConstants.SHAKE_VIB_MAIN, address);
	}
	
	public boolean playvib(int channel, int address) {
		return priv.playvib(channel, address);
	}


}
