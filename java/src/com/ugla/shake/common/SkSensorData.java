package com.ugla.shake.common;

/**	Contains all the sensor data for a SHAKE device, SK6 or SK7 */
public class SkSensorData {
	public int[] acc = new int[3];
	public int[] gyro = new int[3];
	public int[] mag = new int[3];
	public int heading;
	public int[] cap_sk6 = new int[2];	// SK6
	public int[][] cap_sk7 = new int[3][12];			// SK7
	public int[] ana = new int[2];
	public int shaking_peak_accel, shaking_direction, shaking_timestamp;
	public int hr_bpm;
	public int[] rph = new int[3]; // roll-pitch-heading
	public int[] rphq = new int[4];

	/* sequence numbers from the various types of data packets */
	public int[] internal_timestamps = new int[8]; 
	public int sk6seq, hrseq;
	public int sk7seq;

	/*	these sequence numbers are linked to the data access functions, so that an application 
	*	can tell which packet the data it read came from. */
	public int[] timestamps = new int[8];
}
