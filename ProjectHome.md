The SHAKE (Sensing Hardware Accessory for Kinaesthetic Expression) is a versatile Bluetooth inertial sensing device developed by SAMH Engineering Services. The first production units were used by a consortium composed of the [Dynamics & Interaction](http://www.dcs.gla.ac.uk/%7Erod) group (Glasgow University & Hamilton Institute, NUIM), Queen's University Belfast and the FIT Lab, Swansea University.

![http://www.dcs.gla.ac.uk/~adr/dual-SK7-with-2euro-web.png](http://www.dcs.gla.ac.uk/~adr/dual-SK7-with-2euro-web.png)

Drivers for the device are currently available in a variety of languages, including C/C++, Python, Java, Matlab and C#, and can be used on Windows XP/2000, Windows Mobile devices, the Nokia S60 and Maemo platforms, OSX and Linux.

The main features of the SHAKE hardware are summarised below, taken from the SK7 user manual. For a full description of the device and its capabilities, see the [SHAKE SK7 Product Guide](http://shake-drivers.googlecode.com/files/SHAKE%20SK7%20Product%20Guide%20January%202010%20r02.pdf) and the [SHAKE SK7 User manual](http://shake-drivers.googlecode.com/files/SHAKE%20SK7%20User%20Manual%20Rev%2006.pdf).

  * Triple axis linear accelerometer with configurable full scale range of either +/2 g or +/-6 g and an output resolution of 1mg
  * Triple axis Angular Rate Sensor (Gyro) with configurable full scale range of either +/- 300 deg / second or +/- 900 deg / second and a resolution of 0.1 deg / second
  * Triple axis magnetometer with full scale range of +/- 200 uT and resolution of 0.1 uT
  * Two-channel external analog input available over the Aux connector for connecting general-purpose sensors with a 3V 10mA regulated power source available on the same connector.
  * 12 Channel capacitive sensing on the top surface of the enclosure with the sensors arranged in a numeric style keypad layout
  * A three position side navigation switch for general use in the end user’s application
  * AHRS - Attitude (Pitch and Roll orientation) and Azimuth (Compass heading) sensor fusion algorithm that outputs Euler angle or quaternion information for any orientation of the SK7 using data from the accelerometer, magnetometer and gyros (if the SK7-G01 option is installed)
  * AHRS - Attitude (Pitch and Roll orientation) and Azimuth (Compass heading) algorithm that outputs Euler angle and Quaternion information for any orientation of the SK7 using data from the just the accelerometer and magnetometer (if the SK7-G01 option is not installed)
  * Integrated programmable vibrating motor(s) with speed / direction control and braking capability allowing the display of immediate haptic feedback to gestures or other stimuli
  * A vibration profile playback engine that allows complex vibration patterns to be designed offline, uploaded to the SK7 and subsequently triggered on demand using simple command sequences.
  * A trigger window in the pitch/heading Euler angles can be configured such that when the angles are within the window range a vibration profile and/or LED flash is triggered.
  * Trigger thresholds for the capacitive sensing data that can be configured to trigger vibration profiles (if the SK7-C01 option is installed).
  * Data logging facility allows any combination of sensor data to be recorded offline to the internal FLASH memory and uploaded to the host at any time later.
  * In-built 64Mbit of FLASH memory for data recording / logging and non-volatile storage of calibration constants and custom vibration feedback profiles
  * Accurate built in clock for precise time stamping of data samples or events
  * Inbuilt calibration routines for all sensors that allow re-calibration by the end-user with the aid of a simple terminal application should the need ever arise
  * All sensor channels can
    * have their output data rate adjusted between 1Hz and 256Hz
    * be processed with a choice of 18 different FIR filters (N = 256) with differing cutoff frequencies and either linear or minimum phase responses and better than -50dB stopband attenuation
    * have calibration constants selectively applied to the data
    * be streamed in real time to a host computer over USB or Bluetooth
    * be recorded to internal FLASH memory using the data logging feature
  * Data can be output in one of two formats
    * ASCII mode is bandwidth hungry but useful for debugging as it is human readable
    * Raw mode uses bandwidth efficiently and is preferred for normal use in order to maximize data throughput, minimize latency and maximize battery life
  * Four internal expansion slots for optional expansion modules such as the triple axes angular rate sensor SK7-G01, or external vibration driver module SK7-V01
  * The LED visual feedback can be overridden so that any LED colour or pattern can be displayed
  * A 4 pole 2.5mm socket (Auxiliary Socket) that is multiplexed between connectivity of external sensors, external option modules, external vibration actuators and UART communications.
  * A temperature sensor with 0.01 degree C resolution that is also used to compensate the accelerometer data over temperature (if the temperature calibration is ordered)
  * A powerful, efficient DSP microcontroller to handle filtering and other math intensive calculations.
  * Mini-USB connector for data connectivity (via a virtual COM port) and power supply/battery charging
  * A Bluetooth module with Serial Port Profile firmware. This module is called the SK7-BT01 and is based on a CSR Bluecore-4 IC.
  * Integrated device firmware upgrade facilities for both the DSP and Bluetooth microcontrollers over the USB connection (in conjunction with a WindowsTM PC.)
  * Dimensions just 43mm X 32mm X 18mm, Weight is 22 grams
  * Internal rechargeable Li-Polymer battery that can supply the SK7 untethered for up to 10 hours.


The development of these drivers was supported by several funded projects, including EPSRC projects _Audioclouds_, and _Negotiated Interaction_, the IST Programme of the European Commission, via the _Open Interface_ project, IRCSET BRG SC/2003/271, _Continuous Gestural Interaction with Mobile devices_; HEA project _Body Space_; SFI grant _A dynamic systems approach to Negotiated Interaction on mobile devices_, and SFI grant 00/PI.1/C067.

SAMH Engineering can sell the devices to other research groups interested in using the SHAKE. Please contact stephenahughes@gmail.com for more information.