import SHAKE.*;

public class shake_device_test {
	// SHAKE event callback function has to have this signature
	public void callback(int i) {
		switch(i) {
			case shake_device.SHAKE_NAV_UP:
				System.out.println("Nav switch moved up");
				break;
			case shake_device.SHAKE_NAV_DOWN:
				System.out.println("Nav switch moved down");
				break;
			case shake_device.SHAKE_NAV_CENTRE:
				System.out.println("Nav switch pressed straight down");
				break;
			case shake_device.SHAKE_NAV_NORMAL:
				System.out.println("Nav switch released");
				break;
			case shake_device.SHAKE_SHAKING_EVENT:
				System.out.println("SHAKE");
				break;
		}
	}

	public static void main(String[] args) {
		// an instance of this class is used to receive callbacks from the driver
		shake_device_test sdt = new shake_device_test();

		// create the SHAKE device object
		shake_device sd = new shake_device(shake_device.SHAKE_SK7);
	
		// attempt to connect. the -1 parameter means "try to autodetect the COM port number", but this will
		// only work if you have WIDCOMM-based Bluetooth drivers (the type with "My Bluetooth Places")
		// Otherwise the parameter should just be the port number to connect on.
		if (!sd.connect(6)) {
			System.out.println("Failed to connect!");
			System.exit(-1);
		}

		System.out.println("Mod1:" + sd.info_module_name(sd.info_module_slot1()));
		System.out.println("Mod2:" + sd.info_module_name(sd.info_module_slot2()));

		sd.write_power_state(sd.SHAKE_POWER_VIB);
		System.out.println("\nTrying to play a vibration...");
		sd.sk6_upload_vib_sample(5, new int[] { 0x55, 0x33 });
		sd.playvib(sd.SHAKE_VIB_MAIN, 5);

		System.out.println("Result: " + sd.write_sample_rate(shake_device.SHAKE_SENSOR_HEADING, 25));
		System.out.println("Firmware: " + sd.info_firmware_revision());

		// turn on accelerometer, and nav switch
		sd.write_power_state(shake_device.SHAKE_POWER_ACC | shake_device.SHAKE_POWER_NAV);

		// set accelerometer output rate to 50Hz
		sd.write_sample_rate(shake_device.SHAKE_SENSOR_ACC, 50);

		// register an event callback by passing the shake_device_test instance as a parameter
		sd.register_event_callback(sdt);

		// display some accelerometer readings
		for(int i=0;i<250;i++) {
			try {
				Thread.sleep(10);	
			} catch (InterruptedException ie) {}
			int[] acc = new int[3];
			sd.acc(acc);
			System.out.println(acc[0] + ", " + acc[1] + ", " + acc[2]);
		}

		// pause for 5s, try moving the nav switch at this point to see the 
		// callback function output
		System.out.println("\nTry moving the nav switch on the SHAKE...");
		try {
			Thread.sleep(5000);	
		} catch (InterruptedException ie) {}

		sd.reset_shaking_detection();
		System.out.println("\nNow try shaking it!");
		try {
			Thread.sleep(5000);	
		} catch (InterruptedException ie) {}
		sd.write_shaking_config(0);
		
		// unregistering the callback (not required before closing connection)
		sd.register_event_callback(null);

		// close device connection
		System.out.println("Closing dev");
		boolean ret = sd.close();
		System.out.println("ret = " + ret);
	}

}
