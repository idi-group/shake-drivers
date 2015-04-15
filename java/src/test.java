import com.ugla.shake.ShakeDevice;
import com.ugla.shake.ShakeConstants;
import com.ugla.shake.ShakeListener;
import com.ugla.shake.bluetooth.ShakeSerialPort;

public class test implements ShakeListener {

	public static void main(String[] args) {
		ShakeDevice sd = new ShakeDevice(ShakeConstants.ShakeDeviceType.SHAKE_SK7, new ShakeSerialPort());
		sd.connect(25);
		
		sd.register_event_listener(new test());

		sd.write_data_format(0x02);
		sd.write(0x0046, 0x07);
		sd.write_power_state(0xFF);
		sd.write_sample_rate(ShakeConstants.SHAKE_SENSOR_ACC, 10);
		sd.write_sample_rate(ShakeConstants.SHAKE_SENSOR_MAG, 10);
		sd.write_sample_rate(ShakeConstants.SHAKE_SENSOR_GYRO, 10);
		sd.write_sample_rate(ShakeConstants.SHAKE_SENSOR_SK7_CAP, 0);
		sd.write_sample_rate(ShakeConstants.SHAKE_SENSOR_ANA0, 0);
		sd.write_sample_rate(ShakeConstants.SHAKE_SENSOR_ANA1, 0);
		sd.write_sample_rate(ShakeConstants.SHAKE_SENSOR_HEADING, 10);
		for(int i=0;i<500;i++) {
			System.out.println("RPHQ: " + sd.sk7_roll_pitch_heading_quaternions()[0] + ", " + sd.sk7_roll_pitch_heading_quaternions()[1] + ", " + sd.sk7_roll_pitch_heading_quaternions()[2]+ ", " + sd.sk7_roll_pitch_heading_quaternions()[3]);
			
			try { Thread.sleep(10); } catch (InterruptedException ie) {}
		}
		
		try { Thread.sleep(500000); } catch (InterruptedException ie) {}
		sd.close();
	}

	@Override
	public void ShakeEvent(int type) {
		System.out.println(">>> ShakeEvent: " + type);
		
	}

}
