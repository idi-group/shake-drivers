package com.ugla.shake;

/** Values that can appear as the parameter of the SHAKE event callback function.
*	The SHAKE event callback function is used to relay several different events that can
*	be produced by the device. This enumeration defines a value for each individual event
*	so that the callback function can determine the type of the event that has occurred.
*	See the shake_register_event_callback() function for information on setting up the callback. */
public class ShakeEvents {
	/** Nav switch moved up */
	public static final int SHAKE_NAV_UP = 1;
	/** Nav switch moved down */
	public static final int SHAKE_NAV_DOWN = 2;
	/** Nav switch pressed */
	public static final int SHAKE_NAV_CENTRE = 3;
	/** Nav switch released */
	public static final int SHAKE_NAV_NORMAL = 4;
	/** Cap switch 0 upper threshold reached (SK6 only) */
	public static final int SK6_CS0_UPPER = 5;
	/** Cap switch 0 lower threshold reached (SK6 only) */
	public static final int SK6_CS0_LOWER = 6;
	/** Cap switch 1 upper threshold reached (SK6 only) */
	public static final int SK6_CS1_UPPER = 7;
	/** Cap switch 1 lower threshold reached (SK6 only) */
	public static final int SK6_CS1_LOWER = 8;
	/** Logging data playback complete */
	public static final int SHAKE_PLAYBACK_COMPLETE = 9;
	/** Shaking event was triggered */
	public static final int SHAKE_SHAKING_EVENT = 10;
	/** Heart rate monitor event */
	public static final int SHAKE_HEART_RATE_EVENT = 11;
	/** RFID TID detection event */
	public static final int SHAKE_RFID_TID_EVENT = 12;
	/**	Capacitive sensor 0 upper threshold reached (SK7 only) */
	public static final int SK7_CS0_UPPER = 13;
	/** Capacitive sensor 0 lower threshold reached (SK7 only) */
	public static final int SK7_CS0_LOWER = 14;
	/**	Capacitive sensor 1 upper threshold reached (SK7 only) */
	public static final int SK7_CS1_UPPER = 15;
	/** Capacitive sensor 1 lower threshold reached (SK7 only) */
	public static final int SK7_CS1_LOWER = 16;
	/**	Capacitive sensor 2 upper threshold reached (SK7 only) */
	public static final int SK7_CS2_UPPER = 17;
	/** Capacitive sensor 2 lower threshold reached (SK7 only) */
	public static final int SK7_CS2_LOWER = 18;
	/**	Capacitive sensor 3 upper threshold reached (SK7 only) */
	public static final int SK7_CS3_UPPER = 19;
	/** Capacitive sensor 3 lower threshold reached (SK7 only) */
	public static final int SK7_CS3_LOWER = 20;
	/**	Capacitive sensor 4 upper threshold reached (SK7 only) */
	public static final int SK7_CS4_UPPER = 21;
	/** Capacitive sensor 4 lower threshold reached (SK7 only) */
	public static final int SK7_CS4_LOWER = 22;
	/**	Capacitive sensor 5 upper threshold reached (SK7 only) */
	public static final int SK7_CS5_UPPER = 23;
	/** Capacitive sensor 5 lower threshold reached (SK7 only) */
	public static final int SK7_CS5_LOWER = 24;
	/**	Capacitive sensor 6 upper threshold reached (SK7 only) */
	public static final int SK7_CS6_UPPER = 25;
	/** Capacitive sensor 6 lower threshold reached (SK7 only) */
	public static final int SK7_CS6_LOWER = 26;
	/**	Capacitive sensor 7 upper threshold reached (SK7 only) */
	public static final int SK7_CS7_UPPER = 27;
	/** Capacitive sensor 7 lower threshold reached (SK7 only) */
	public static final int SK7_CS7_LOWER = 28;
	/**	Capacitive sensor 8 upper threshold reached (SK7 only) */
	public static final int SK7_CS8_UPPER = 29;
	/** Capacitive sensor 8 lower threshold reached (SK7 only) */
	public static final int SK7_CS8_LOWER = 30;
	/**	Capacitive sensor 9 upper threshold reached (SK7 only) */
	public static final int SK7_CS9_UPPER = 31;
	/** Capacitive sensor 9 lower threshold reached (SK7 only) */
	public static final int SK7_CS9_LOWER = 32;
	/**	Capacitive sensor 10 upper threshold reached (SK7 only) */
	public static final int SK7_CSA_UPPER = 33;
	/** Capacitive sensor 10 lower threshold reached (SK7 only) */
	public static final int SK7_CSA_LOWER = 34;
	/**	Capacitive sensor 11 upper threshold reached (SK7 only) */
	public static final int SK7_CSB_UPPER = 35;
	/** Capacitive sensor 11 lower threshold reached (SK7 only) */
	public static final int SK7_CSB_LOWER = 36;
}
