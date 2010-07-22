import ugla.shake.*;
import ugla.shake.bluetooth.*;

public class shake_test implements SHAKEListener, SHAKEDebug {

	public void SHAKE_Event(int type) {
		switch(type) {
			case SHAKEConstants.SHAKE_NAV_CENTRE:
				System.out.println("Nav button centre");
				break;
			case SHAKEConstants.SHAKE_NAV_DOWN:
				System.out.println("Nav button down");
				break;
			case SHAKEConstants.SHAKE_NAV_UP:
				System.out.println("Nav button up");
				break;
			case SHAKEConstants.SHAKE_NAV_NORMAL:
				System.out.println("Nav button released");
				break;
		}
	}

	public void debug(String dbg) {
		System.out.println(dbg);
	}

	public static void main(String[] args) {
		// create an object to receive the events from pressing the button on the SHAKE
		shake_test st = new shake_test();
		
		// the SHAKE object
		SHAKEDevice s = new SHAKEDevice(new SHAKESerialPort());
		if(!s.connect(6)) {
		//SHAKEDevice s = new SHAKEDevice(new SHAKERXTX());
		//if(!s.connect("COM2")) {
			System.out.println("Failed to connect!");
			System.exit(1);
		}
		
		System.out.println("Connected");
		//s.setDebugOut(st);
		
		//System.out.println("Setting power state");
		// turn on button and accelerometer
		if(!s.writePowerState(SHAKEConstants.SHAKE_POWER_ACC | SHAKEConstants.SHAKE_POWER_ANALOG | SHAKEConstants.SHAKE_POWER_NAV)) 
			System.out.println("Failed to set power state");
		
		System.out.println("Setting data format");
		if(!s.writeDataFormat(0))
			System.out.println("Failed to set data format");

		System.out.println("Setting sample rate");
		if(!s.writeSampleRate(SHAKEConstants.SHAKE_SENSOR_ACC, 20))
			System.out.println("Failed to set sample rate");
		
		if(!s.writeSampleRate(SHAKEConstants.SHAKE_SENSOR_GYRO, 0))
			System.out.println("Failed to set sample rate");
		
		if(!s.writeSampleRate(SHAKEConstants.SHAKE_SENSOR_ANA0, 20))
			System.out.println("Failed to set sample rate");
		
		if(!s.writeSampleRate(SHAKEConstants.SHAKE_SENSOR_ANA1, 20))
			System.out.println("Failed to set sample rate");
		
		try {
			for(int i=0;i<100;i++) {
				Thread.sleep(10);
				int[] acc = s.acc();
				int[] ana = s.ana();
				System.out.println("Acc: " + acc[0] + ", " + acc[1] + ", " + acc[2] + " | " + s.data_timestamp(SHAKEConstants.SHAKE_SENSOR_ACC));
				System.out.println("Ana: " + ana[0] + ", " + ana[1]);
				
			}
		} catch (InterruptedException e) {}
		
		System.out.println("Hardware revision: " + s.hardwareRevision());
		System.out.println("Firmware revision: " + s.firmwareRevision());
		System.out.println("Serial number: " + s.serialNumber());
		System.out.println("Slot1: " + s.expansionSlotByName(s.expansionSlot1()));
		System.out.println("Slot2: " + s.expansionSlotByName(s.expansionSlot2()));
		
		s.writeSampleRate(SHAKEConstants.SHAKE_SENSOR_ACC, 0);
		s.writeSampleRate(SHAKEConstants.SHAKE_SENSOR_ANA0, 0);
		s.writeSampleRate(SHAKEConstants.SHAKE_SENSOR_ANA1, 0);
		
		// register the shake_test object as the listener for SHAKE events
		s.registerEventListener(st);
		//s.setDebugOut(st);
		
		try { Thread.sleep(5000); } catch (InterruptedException ie) {}
		
		// close the connection
		
		s.close();
		
	}

}
