##  @package shake Python wrapper for the SHAKE C driver.
##  @file shake.py Python wrapper for the SHAKE C driver.

import sys, atexit, os

# set the directory used for loading the pyshake.pyd extension
ext_dir = os.path.join(sys.prefix, 'lib\\site-packages\\')
sys.path.append(ext_dir)

major, minor = sys.version_info[0], sys.version_info[1]
if major == 2 and minor == 4:
    import pyshake
elif major == 2 and minor >= 5:
    import pyshakecpp as pyshake
else:
    print("shake.py: Warning, unsupported Python version (%d.%d)"%(major, minor))
    # attempt to import the pyshake module anyway
    import pyshakecpp as pyshake

# SHAKE driver constants

## Global success code for SHAKE functions.
SHAKE_SUCCESS = 1
## Global error code for SHAKE functions.
SHAKE_ERROR = -1

## Device type SK6
SHAKE_SK6 = 0

## Device type SK7
SHAKE_SK7 = 1

## Event produced when nav switch moved up
SHAKE_NAV_UP = 1

## Event produced when nav switch moved down
SHAKE_NAV_DOWN = 2

## Event produced when nav switch pressed
SHAKE_NAV_CENTRE = 3

## Event produced when nav switch released
SHAKE_NAV_NORMAL = 4

## Event produced when cap switch 0 upper threshold reached
SHAKE_CS0_UPPER = 5

## Event produced when cap switch 0 lower threshold reached
SHAKE_CS0_LOWER = 6

## Event produced when cap switch 1 upper threshold reached
SHAKE_CS1_UPPER = 7

## Event produced when cap switch 1 lower threshold reached
SHAKE_CS1_LOWER = 8

## Event produced when data logging playback is completed
SHAKE_PLAYBACK_COMPLETE = 9

## Event produced when a shaking event detected
SHAKE_SHAKING_EVENT = 10

## Event produced when a heart rate event is triggered
SHAKE_HEART_RATE_EVENT = 11

## Event produced when an RFID tag is detected
SHAKE_RFID_TID_EVENT = 12

## Capacitive sensor 0 upper threshold reached (SK7 only) 
SK7_CS0_UPPER = 13
## Capacitive sensor 0 lower threshold reached (SK7 only)   
SK7_CS0_LOWER = 14
##  Capacitive sensor 1 upper threshold reached (SK7 only) 
SK7_CS1_UPPER = 15
## Capacitive sensor 1 lower threshold reached (SK7 only) 
SK7_CS1_LOWER = 16
##  Capacitive sensor 2 upper threshold reached (SK7 only) 
SK7_CS2_UPPER = 17
## Capacitive sensor 2 lower threshold reached (SK7 only) 
SK7_CS2_LOWER = 18
##  Capacitive sensor 3 upper threshold reached (SK7 only) 
SK7_CS3_UPPER = 19
## Capacitive sensor 3 lower threshold reached (SK7 only) 
SK7_CS3_LOWER = 20
##  Capacitive sensor 4 upper threshold reached (SK7 only) 
SK7_CS4_UPPER = 21
## Capacitive sensor 4 lower threshold reached (SK7 only) 
SK7_CS4_LOWER = 22
##  Capacitive sensor 5 upper threshold reached (SK7 only) 
SK7_CS5_UPPER = 23
## Capacitive sensor 5 lower threshold reached (SK7 only) 
SK7_CS5_LOWER = 24
##  Capacitive sensor 6 upper threshold reached (SK7 only)  
SK7_CS6_UPPER = 25
## Capacitive sensor 6 lower threshold reached (SK7 only)       
SK7_CS6_LOWER = 26
##  Capacitive sensor 7 upper threshold reached (SK7 only)  
SK7_CS7_UPPER = 27
## Capacitive sensor 7 lower threshold reached (SK7 only) 
SK7_CS7_LOWER = 28
##  Capacitive sensor 8 upper threshold reached (SK7 only) 
SK7_CS8_UPPER = 29
## Capacitive sensor 8 lower threshold reached (SK7 only) 
SK7_CS8_LOWER = 30
##  Capacitive sensor 9 upper threshold reached (SK7 only) 
SK7_CS9_UPPER = 31
## Capacitive sensor 9 lower threshold reached (SK7 only) 
SK7_CS9_LOWER = 32
##  Capacitive sensor 10 upper threshold reached (SK7 only) 
SK7_CSA_UPPER = 33
## Capacitive sensor 10 lower threshold reached (SK7 only) 
SK7_CSA_LOWER = 34
##  Capacitive sensor 11 upper threshold reached (SK7 only) 
SK7_CSB_UPPER = 35
## Capacitive sensor 11 lower threshold reached (SK7 only) 
SK7_CSB_LOWER = 36

## Return values for info_module_slot1 and info_module_slot2 functions

###############
## SK6 modules
###############
## No module installed
SK6_MODULE_NONE = 0
## slot 1 : G-01 gyro module
SK6_MODULE_GYRO1 = 1,       
## slot 1 : G-02 gyro module
SK6_MODULE_GYRO2 = 2            
## slot 1 : RFID module
SK6_MODULE_RFID = 3
## slot 2 : audio module
SK6_MODULE_AUDIO = 4
## slot 2 : external vibration module
SK6_MODULE_EXTVIB = 5
## slot 2 : USB module
SK6_MODULE_USB = 6
## slot 2 : STANE
SK6_MODULE_EXPANSION = 7
SK6_MODULE_LAST = 8

###############
## SK7 modules
###############
## No module installed
SK7_MODULE_NONE = SK6_MODULE_LAST + 1
# TODO


## Contents of SHAKE_NV_REG_POWER1 register will be a combination of these values
SHAKE_POWER_CAP = 0x01
SHAKE_POWER_ACC = 0x02
SHAKE_POWER_GYRO = 0x04
SHAKE_POWER_VIB = 0x08
SHAKE_POWER_MAG = 0x10
SHAKE_POWER_TEMP = 0x20
SHAKE_POWER_ANALOG = 0x40
SHAKE_POWER_NAV = 0x80

## Contents of SHAKE_NV_REG_POWER2 register will be a combination of these values
SHAKE_POWER_CS0_INC =   0x01
SHAKE_POWER_CS0_DEC =   0x02
SHAKE_POWER_CS1_INC =   0x04
SHAKE_POWER_CS1_DEC =   0x08
SHAKE_POWER_CS_EVENTS = 0x10

## Sensor ID numbers for the read/write_sample_rate and read/write_digital_filter functions
SHAKE_SENSOR_ACC = 0
SHAKE_SENSOR_GYRO = 1
SHAKE_SENSOR_MAG = 2
SHAKE_SENSOR_HEADING = 3
SHAKE_SENSOR_CAP = 4
SHAKE_SENSOR_CAP0 = 4
SHAKE_SENSOR_CAP1 = 5
SHAKE_SENSOR_ANA0 = 6
SHAKE_SENSOR_ANA1 = 7

## SHAKE vibrotactile channels
SHAKE_VIB_MAIN = 0
SHAKE_VIB_LEFT = 1
SHAKE_VIB_RIGHT = 2
SHAKE_VIB_FORCEREACTOR = 3
SHAKE_VIB_EXT_ACTUATOR = 4

## Maximum number of speed/time pairs in a vibrotactile sample
SHAKE_VIB_SAMPLE_MAX_LENGTH = 32
## Highest address you can use to store a sample in memory
SHAKE_VIB_PROFILE_MAX = 0xFF

## The contents of the SHAKE_VO_REG_PWRSTA register will be a combination of these values
SHAKE_EXT_POWER = 0x01
SHAKE_BAT_CHARGING = 0x02
SHAKE_BAT_FULL = 0x04

## Maximum battery level
SHAKE_BATTERY_MAX = 0xFF

## Temperature range available from the temp sensor on the SHAKEs
SHAKE_TEMP_MIN = 0x00
SHAKE_TEMP_MAX = 0x40

## When writing to the SHAKE_VO_REG_LOGGING_CTRL register, use one of these values as the parameter
SHAKE_LOGGING_PLAY  =   0x01
SHAKE_LOGGING_PAUSE =   0x02
SHAKE_LOGGING_STOP  =   0x03
SHAKE_LOGGING_RECORD =  0x04
SHAKE_LOGGING_RESET =   0x05
SHAKE_LOGGING_BT_POWER_DOWN = 0x06

## The contents of the SHAKE_VO_REG_LOGGING_STATUS register should be a combination of these values 
SHAKE_LOGGING_RECORDING =   0x01
SHAKE_LOGGING_PLAYING   =   0x02
SHAKE_LOGGING_MEMORYFULL =  0x04

## The contents of the SHAKE_NV_REG_SHAKING_CONFIG register should be a combination of zero or more of these values
SHAKE_SHAKING_DETECTION = 0x01
SHAKE_SHAKING_DIRECTION_DETECTION = 0x02
SHAKE_SHAKING_VIBRATION = 0x04

## The contents of the SHAKE_NV_REG_AUDIO_CONFIG register should be a combination of zero or more of these values
SHAKE_AUDIO_SAMPLING = 0x01
SHAKE_AUDIO_PLAYBACK = 0x02
SHAKE_AUDIO_EXTERNAL_MIC = 0x04
SHAKE_AUDIO_WAVE_PLAYBACK = 0x08
SHAKE_AUDIO_SAMPLE_PLAYBACK = 0x10

## The contents of the SHAKE_NV_REG_HEART_RATE_CONFIG register should be a combination of zero or more of these values
SHAKE_HEART_RATE_MONITORING = 0x01,
SHAKE_HEART_RATE_VISUAL_PULSE = 0x02
SHAKE_HEART_RATE_VIB_PULSE = 0x04

## The contents of the SHAKE_NV_REG_EXPANSION_CONFIG register should be a combination of zero or more of these values
SHAKE_EXPANSION_CONTACT_MIC = 0x02
SHAKE_EXPANSION_GP_OUTPUTS = 0x04
SHAKE_EXPANSION_VIB_POWER = 0x08

## RFID TID tag length
SHAKE_RFID_TAG_LENGTH = 32

## The contents of the SHAKE_NV_REG_RFID_CONFIG register should be a combination of zero or more of these values
SHAKE_RFID_POWER_ON     = 0x01
SHAKE_RFID_VISUAL_FEEDBACK = 0x02
SHAKE_RFID_AUTO_SCAN    = 0x04

## Number of samples in a SHAKE audio data packet
SHAKE_AUDIO_DATA_LEN =  0x20

## SHAKE audio sampling rate in Hz
SHAKE_AUDIO_SAMPLE_RATE = 7200

## (decoded) SHAKE audio sample size in bits
SHAKE_AUDIO_SAMPLE_SIZE = 16

## Writing this value into SHAKE_VO_REG_MIDI_AMPLITUDE mutes the synthesiser
SHAKE_MIDI_AMPLITUDE_MUTE = 0x00

## Minimum and maximum values for the synthesiser volume
SHAKE_MIDI_AMPLITUDE_MIN = 0x01
SHAKE_MIDI_AMPLITUDE_MAX = 0xFF

## Minimum and maximum values for the SHAKE_VO_REG_MIDI_NOTE register
SHAKE_MIDI_NOTE_MIN = 0x21
SHAKE_MIDI_NOTE_MAX = 0x69

## If writing into the SHAKE_VO_REG_MIDI_WAVEFORM register, the value to write should be one of the following 
SHAKE_MIDI_WAVEFORM_SINE = 0x00
SHAKE_MIDI_WAVEFORM_SQUARE = 0x01
SHAKE_MIDI_WAVEFORM_TRIANGLE = 0x02
SHAKE_MIDI_WAVEFORM_SAWTOOTH = 0x03

## Audio sample upload page size (1 page = 1056 samples)
SHAKE_UPLOAD_PAGE_SIZE = 1056

## Audio sample minimum amplitude 
SHAKE_AUDIO_AMPLITUDE_MIN = 0

## Audio sample maximum amplitude
SHAKE_AUDIO_AMPLITUDE_MAX = 255

## Audio sample upload minimum address 
SHAKE_UPLOAD_MIN_PAGE = 0

## Audio sample upload maximum address
SHAKE_UPLOAD_MAX_PAGE = 7999

## Non volatile register addresses
SHAKE_NV_REG_POWER1 = 0x0000
SHAKE_NV_REG_POWER2 = 0x0001
SHAKE_NV_REG_DATAFMT = 0x0002
SHAKE_NV_REG_ACC_CONFIG = 0x0004
SHAKE_NV_REG_CALIB_BYPASS = 0x0006
SHAKE_NV_REG_CX_CALIB_BYPASS = 0x0007
SHAKE_NV_REG_ACCOUT = 0x0008
SHAKE_NV_REG_GYROUT = 0x0009
SHAKE_NV_REG_MAGOUT = 0x000A
SHAKE_NV_REG_HEDOUT = 0x000B
SHAKE_NV_REG_CAP0OUT = 0x000C
SHAKE_NV_REG_CAP1OUT = 0x000D
SHAKE_NV_REG_ANA0OUT = 0x000E
SHAKE_NV_REG_ANA1OUT = 0x000F
SHAKE_NV_REG_TEMP_COMPENSATION = 0x0010
SHAKE_NV_REG_STREAM_DISABLE = 0x0012
SHAKE_NV_REG_DIGFIL_ACC = 0x0014
SHAKE_NV_REG_DIGFIL_GYR = 0x0015
SHAKE_NV_REG_DIGFIL_MAG = 0x0016
SHAKE_NV_REG_DIGFIL_HED = 0x0017
SHAKE_NV_REG_DIGFIL_CAP0 = 0x0018
SHAKE_NV_REG_DIGFIL_CAP1 = 0x0019
SHAKE_NV_REG_DIGFIL_ANA0 = 0x001A
SHAKE_NV_REG_DIGFIL_ANA1 = 0x001B
SHAKE_NV_REG_AUDIO_CONFIG = 0x001E
SHAKE_NV_REG_EXPANSION_CONFIG = 0x026
SK6_NV_REG_CS0_INC = 0x0028
SK6_NV_REG_CS0_DEC = 0x0029
SK6_NV_REG_CS0_INC_PROFILE = 0x002A
SK6_NV_REG_CS0_DEC_PROFILE = 0x002B
SK6_NV_REG_CS1_INC = 0x002C
SK6_NV_REG_CS1_DEC = 0x002D
SK6_NV_REG_CS1_INC_PROFILE = 0x002E
SK6_NV_REG_CS1_DEC_PROFILE = 0x002F
SK7_NV_REG_CAP_INC = 0x0028
SK7_NV_REG_CAP_DEC = 0x0029
SK7_NV_REG_CAP_INC_PROFILE = 0x002A
SK7_NV_REG_CAP_DEC_PROFILE = 0x002B
SHAKE_NV_REG_SHAKING_CONFIG = 0x0030
SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD = 0x0032
SHAKE_NV_REG_SHAKING_HOLDOFF_TIME = 0x0033
SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE = 0x0034
SHAKE_NV_REG_SHAKING_HPF_CONSTANT = 0x0035
SHAKE_NV_REG_SHAKING_LPF_CONSTANT = 0x0036
SHAKE_NV_REG_LOGGING_PKT_LSB = 0x0038
SHAKE_NV_REG_LOGGING_PKT_MSB = 0x0039
SHAKE_NV_REG_HEART_RATE_CONFIG = 0x003A
SHAKE_NV_REG_RFID_CONFIG = 0x003C
SHAKE_NV_REG_RFID_FREQUENCY = 0x003D
SK7_NV_REG_RPH_CONFIG = 0x0046
SK7_NV_REG_HEADING_FEEDBACK = 0x0048
SK7_NV_REG_HEADING_LOWER_LSB = 0x004A
SK7_NV_REG_HEADING_LOWER_MSB = 0x004B
SK7_NV_REG_HEADING_UPPER_LSB = 0x004C
SK7_NV_REG_HEADING_UPPER_MSB = 0x004D
SK7_NV_REG_HEADING_HYSTERESIS = 0x004E
SK7_NV_REG_HEADING_VIB_PROFILE = 0x004F


## Volatile register addresses
SHAKE_VO_REG_PKTREQ = 0x0100
SHAKE_VO_REG_DATAREQ = 0x0101
SHAKE_VO_REG_VIB_MAIN = 0x0102
SHAKE_VO_REG_VIB_LEFT = 0x0103
SHAKE_VO_REG_VIB_RIGHT = 0x0104
SHAKE_VO_REG_BATTERY = 0x0105
SHAKE_VO_REG_PWRSTA = 0x0106
SHAKE_VO_REG_TEMPERATURE = 0x0108
SHAKE_VO_REG_CALIBRATION = 0x010A
SHAKE_VO_REG_LOGGING_CTRL = 0x010D
SHAKE_VO_REG_LOGGING_STATUS = 0x010E
SHAKE_VO_REG_LOGGING_MEM = 0x0110
SHAKE_VO_REG_MIDI_AMPLITUDE = 0x0111
SHAKE_VO_REG_MIDI_NOTE = 0x0112
SHAKE_VO_REG_MIDI_WAVEFORM = 0x0113
SHAKE_VO_REG_RFID_SCAN = 0x011A
SHAKE_VO_REG_EXP_PWM1   = 0x0120
SHAKE_VO_REG_EXP_PWM2   = 0x0121
SHAKE_VO_REG_EXP_PWM3   = 0x0122
SHAKE_VO_REG_EXP_PWM4   = 0x0123
SHAKE_VO_REG_EXP_PWM5   = 0x0124
SHAKE_VO_REG_EXP_PWM6   = 0x0125
SHAKE_VO_REG_VIB_LEFT_CONTINUOUS = 0x0126
SHAKE_VO_REG_VIB_RIGHT_CONTINUOUS = 0x0127

# work out platform from os.name
PLAT_WIN32, PLAT_LINUX = range(2)

platform = -1

#   Linux
if os.name == 'posix':
    platform = PLAT_LINUX
#   Windows 2000/XP or CE
elif os.name == 'nt' or os.name == 'ce':
    platform = PLAT_WIN32
else:
    raise pyshake_serial_error("Unsupported or unknown platform: " + os.name)

##  Gets version of SHAKE driver dll
def driver_version():
    return pyshake.driver_version()

## An instance of this class represents a single SHAKE device.
class shake_device:

    ## Constructor
    def __init__(self, t = SHAKE_SK7):
        self.__shakedev = -1;
        self.__lasterror = ""
        self.__connected = False
        self.__devtype = t # default to SK6

    ##  Attempts to open a connection to a SHAKE device - Windows only?
    #
    #   @param p should be a COM port number on Windows (integer type) or a Bluetooth address in string format
    #       ("12:34:56:78:90") on Linux
    #   @param t SHAKE_SK6 or SHAKE_SK7
    # 
    #   @return True on success, False on failure. If False, use last_error() to obtain a string indicating the problem.
    def connect(self, p):
        ti = type(123)
        ts = type("123")

        if self.__connected:
            self.__lasterror = "Already connected to a SHAKE!"
            return False

        if p == None:
            self.__lasterror = "Must provide either COM port (int) or Bluetooth address (string)"
            return False

        if type(p) != ti and type(p) != ts:
            self.__lasterror = "Unknown parameter type (expected int or string)"
            return False
    
        if type(p) != ti and platform == PLAT_WIN32:
            self.__lasterror = "Error: must give COM port as parameter on Windows platforms!"
            return False
        elif type(p) != ts and (platform == PLAT_LINUX):
            self.__lasterror = "Error: must give Bluetooth address as parameter on Linux platforms!"
            return False

        # pass the parameter onto pyshake.init_device
        self.__shakedev = pyshake.init_device(p, self.__devtype)

        if self.__shakedev == -1:
            self.__lasterror = "Failed to connect"
            return False

        self.__connected = True
        return True

    ##  Attempts to open a connection to a SHAKE device
    #
    #   @param btaddr should be a Bluetooth address in integer or string format. Note that on Windows this will
    #       only work if you're using the Bluetooth stack provided by Microsoft. For other stacks, use the 
    #       connect() function and a COM port number.
    #       If the address is in integer format, it should correspond directly to the target device Bluetooth address
    #           eg 00:11:22:33:44:55 could be given as 0x001122334455
    #       If the address is in string format, any of the following formats should work (note that a-f characters must be lowercase)
    #           "aa:bb:cc:dd:ee:ff" OR "aabbccddeeff"
    #
    #   @return True on success, False on failure. If False, use last_error() to obtain a string indicating the problem.
    def connect_rfcomm(self, btaddr):
        ti = type(123L)
        ts = type("123")

        if type(btaddr) == ti:
            self.__shakedev = pyshake.init_device_rfcomm_i64(btaddr, self.__devtype)
        else:
            self.__shakedev = pyshake.init_device_rfcomm_str(btaddr, self.__devtype)

        if self.__shakedev == -1:
            self.__lasterror = "Failed to connect"
            return False

        self.__connected = True
        return True

    ##  Attempts to open a connection to a SHAKE device
    #
    #   @param usb_dev should be the character device file related to the Shake to connect to,
    #       for example '/dev/tty.SHAKESK7SN0075-SPPDev', passed as a string.
    #
    #   @return True on success, False on failure. If False, use last_error() to obtain a string indicating the problem.
    def connect_usb(self, usb_dev):
        self.__shakedev = pyshake.init_device_usb(usb_dev, self.__devtype)
        if self.__shakedev == -1:
            self.__lasterror = "Failed to connect"
            return False

        self.__connected = True
        return True

    ##  Attempts to open a "debugging connection" to a SHAKE device. This simply means that instead of making a Bluetooth
    #   connection, the driver will instead read data from the file given as the first parameter, and output any commands
    #   into the file given as the second parameter. 
    #
    #   @param inp a file that the driver should read data from. This should contain valid SHAKE data in ASCII or binary format. When
    #       the end of the file is reached the reading position will be reset back to the start. 
    #   @param outp a file that the driver should output data into (eg commands that would normally be sent to the SHAKE). This
    #       file will be overwritten each time the function is called. 
    #
    #   @return True on success, False on failure. If False, use last_error() to obtain a string indicating the problem.
    def connect_debug(self, inp, outp):
        self.__shakedev = pyshake.init_device_debug(inp, outp, self.__devtype)
        if self.__shakedev == -1:
            self.__lasterror = "Failed to connect"
            return False

        self.__connected = True
        return True
    
    ##  Retrieves the last error message produced by the connect() function.
    #
    #   @return a string containing the message
    def last_error(self):
        return self.__lasterror

    ##  Closes the connection associated with this instance of the class
    #
    #   @return True on success, False on failure
    def close(self):
        if not self.__connected:
            self.__lasterror = 'Not connected'
            return False

        pyshake.register_audio_callback(self.__shakedev, None, self)
        pyshake.register_event_callback(self.__shakedev, None, self)
        self.__connected = False
        pyshake.free_device(self.__shakedev)
        self.__shakedev = -1
        return True

    ##  Indicates if connection is currently active.
    #   @return True if connection active, False if not.
    def isconnected(self):
        return self.__connected

    ##  Gets firmware revision of connected device.
    #   @return 0.0 on error, else firmware version (x.yz format)
    def info_firmware_revision(self):
        if not self.__connected:
            return 0.0
        return pyshake.info_firmware_revision(self.__shakedev)

    ##  Gets hardware revision of connected device.
    #   @return 0.0 on error, else hardware version (x.yz format)
    def info_hardware_revision(self):
        if not self.__connected:
            return 0.0
        return pyshake.info_hardware_revision(self.__shakedev)

    ##  Get serial number of connected device.
    #   @return empty string on error, else serial number in format: "SK6-xxxx" (eg "SK6-0003")
    def info_serial_number(self):
        if not self.__connected:
            return ""
        return pyshake.info_serial_number(self.__shakedev)

    ##  Get the type of expansion module (if any) installed in slot 1 of the connected SHAKE
    #
    #   @return SHAKE_ERROR if no SHAKE connected, otherwise one of the SHAKE_MODULE_ constants
    def info_module_slot1(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.info_module_slot1(self.__shakedev)

    ##  Get the type of expansion module (if any) installed in slot 2 of the connected SHAKE
    #
    #   @return SHAKE_ERROR if no SHAKE connected, otherwise one of the SHAKE_MODULE_ constants
    def info_module_slot2(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.info_module_slot2(self.__shakedev)

    ##  Causes the SHAKE to output information on its installed expansion modules, plus firmware and hardware versions
    #   After calling this function, it should be possible to retrieve that data through the other info_ functions
    #   @return SHAKE_ERROR or SHAKE_SUCCESS
    def info_retrieve(self):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.info_retrieve(self.__shakedev)

    ##  Attempt to reset SHAKE registers to factory default
    def factory_reset(self, repeats=1):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.factory_reset(self.__shakedev, repeats)

    ##  Get x-axis accelerometer reading
    #   @return current x-axis accelerometer reading
    def accx(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.accx(self.__shakedev)

    ##  Get y-axis accelerometer reading
    #   @return current y-axis accelerometer reading
    def accy(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.accy(self.__shakedev)

    ##  Get z-axis accelerometer reading
    #   @return current z-axis accelerometer reading
    def accz(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.accz(self.__shakedev)

    ##  Get current readings from all 3 accelerometer axes
    #   @return empty list on error, else a 3 element list with [x, y, z] axis readings
    def acc(self):
        if not self.__connected:
            return []
        return pyshake.acc(self.__shakedev)

    ##  Get x-axis accelerometer reading
    #   @return current x-axis accelerometer reading
    def gyrx(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.gyrx(self.__shakedev)
    
    ##  Get y-axis gyro reading
    #   @return current y-axis gyro reading
    def gyry(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.gyry(self.__shakedev)

    ##  Get z-axis magnetometer reading
    #   @return current z-axis magnetometer reading
    def gyrz(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.gyrz(self.__shakedev)

    ##  Get current readings from all 3 gyroscope axes
    #   @return empty list on error, else a 3 element list with [x, y, z] axis readings
    def gyro(self):
        if not self.__connected:
            return []
        return pyshake.gyr(self.__shakedev)
            
    ##  Get x-axis magnetometer reading
    #   @return current x-axis magnetometer reading
    def magx(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.magx(self.__shakedev)

    ##  Get y-axis gyro reading
    #   @return current y-axis gyro reading
    def magy(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.magy(self.__shakedev)

    ##  Get z-axis magnetometer reading
    #   @return current z-axis magnetometer reading
    def magz(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.magz(self.__shakedev)

    ##  Get current readings from all 3 magnetometer axes
    #   @return empty list on error, else a 3 element list with [x, y, z] axis readings
    def mag(self):
        if not self.__connected:
            return []
        return pyshake.mag(self.__shakedev)

    ##  Get current compass heading
    #   @return current compass heading (0-3599, tenths of a degree)
    def heading(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.heading(self.__shakedev)

    ##  Get current roll,pitch,heading
    def sk7_roll_pitch_heading(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.sk7_roll_pitch_heading(self.__shakedev)

    ##  Get current roll, pitch, heading data in quaternion format 
    #   (4 element list)
    def sk7_roll_pitch_heading_quaternions(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.sk7_roll_pitch_heading_quaternions(self.__shakedev)

    ##  Get current proxmity value for cap switch 0
    #   @return current proximity value for cap switch 0 (0-255)
    def cap0(self):
        if not self.__connected or self.__devtype == SHAKE_SK7:
            return SHAKE_ERROR

        return pyshake.sk6_cap0(self.__shakedev)

    ##  Get current proxmity value for cap switch 1
    #   @return current proximity value for cap switch 1 (0-255) 
    def cap1(self):
        if not self.__connected or self.__devtype == SHAKE_SK7:
            return SHAKE_ERROR

        return pyshake.sk6_cap1(self.__shakedev)

    ##  Get current proxmity values for both capacitive switches
    #   @return (SK6) empty list on error, else a 2 element list containing [cap 0 proximity, cap 1 proximity]
    #   @return (SK7) empty list on error, else a 12 element list containing [cap0 prox, ..., cap11 prox]
    def cap(self): 
        if not self.__connected:
            return []

        if self.__devtype == SHAKE_SK6:
            return pyshake.sk6_cap(self.__shakedev)

        return pyshake.sk7_cap(self.__shakedev)
    
    ##  Get current proximity values from an external capacitive board (SK7 only!)
    #   @param blocks 0 = 1st block, 1 = 2nd block, 2 = both blocks 
    #   @return a 12 element list (blocks = 0  or 1) or a 24 element list (blocks = 2)
    def cap_ext(self, blocks):
        if not self.__connected:
            return []

        if self.__devtype == SHAKE_SK6:
            return []
        if blocks < 0 or blocks > 2:
            return []

        return pyshake.sk7_cap_ext(self.__shakedev, blocks)

    ##  Get current reading from analog input 0
    #   @return current reading from analog input 0
    def analog0(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.analog0(self.__shakedev)

    ##  Get current reading from analog input 1
    #   @return current reading from analog input 1
    def analog1(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.analog1(self.__shakedev)

    ##  Get current readings from both analog inputs
    #   @return empty list on error, else a 2 element list containing [analog 0 value, analog 1 value]
    def analog(self):
        if not self.__connected:
            return []
        return pyshake.analog(self.__shakedev)

    ##  Get peak acceleration from last shaking event
    #   @return SHAKE_ERROR or peak acceleration in 0.1g units
    def shaking_peakaccel(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.shaking_peakaccel(self.__shakedev)
            
    ##  Get direction of last shaking event
    #   @return SHAKE_ERROR or one of the following: 0 (direction detection is disabled), 1 (x-axis), 2 (y-axis) and 3 (z-axis).
    def shaking_direction(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.shaking_direction(self.__shakedev)
    
    ##  Gets timestamp of last shaking event
    #   @return SHAKE_ERROR or the time modulo 1 second in units of 1/1024 second that the event was detected 
    def shaking_timestamp(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.shaking_timestamp(self.__shakedev)

    ##  Used to obtain the sequence number for the last set of data retrieved for a particular sensor
    #   For example, if you call acc() to retrieve a set of acc data, then call data_timestamp(SHAKE_SENSOR_ACC)
    #   the return value will be the sequence number that was attached to the packet that provided the readings
    #   returned by the acc() function. 
    #
    #   @param sensor one of the SHAKE_SENSOR_ constants
    #
    #   @return SHAKE_ERROR or the sequence number for the selected sensor
    def data_timestamp(self, sensor):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.data_timestamp(self.__shakedev, sensor)

    ##  Gets the latest heart rate reading
    #   
    #   @return SHAKE_ERROR or the heart rate reading in beats per minute
    def heart_rate(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.heart_rate(self.__shakedev)

    ##  Gets the last RFID tag ID detected
    #   
    #   @return an empty string on error, otherwise a string containing the ID
    def rfid_tid(self):
        if not self.__connected:
            return ""
        return pyshake.rfid_tid(self.__shakedev)

    ##  Triggers an RFID scan
    #
    #   @return SHAKE_ERROR or SHAKE_SUCCESS
    def rfid_scan(self):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.rfid_scan(self.__shakedev)

    ##  Reads register <address> and returns a 2-tuple: (success/error, value), where
    #   the first item in the tuple is SHAKE_SUCCESS or SHAKE_ERROR depending on the
    #   result of the operation. value will be 0 on error, otherwise it will be the
    #   current contents of the specified register.
    #
    #   @param address the address of the register to read
    #
    #   @return a 2-tuple containing a success/error value (SHAKE_SUCCESS/SHAKE_ERROR) and the contents
    #       of the selected register. If the first value is SHAKE_ERROR, the second value is always 0. 
    def read(self, address):
        if not self.__connected:
            return (SHAKE_ERROR, 0)
        return pyshake.read(self.__shakedev, address)

    ##  Writes <value> into register <address> and returns either SHAKE_ERROR or
    #   SHAKE_SUCCESS to indicate the result.
    #
    #   @param address the address of the register to write into
    #   @param value the value to write into the register
    #
    #   @return SHAKE_SUCCESS or SHAKE_ERROR
    def write(self, address, value):
        if not self.__connected:
            return SHAKE_ERROR
        return pyshake.write(self.__shakedev, address, value)

    ##  Registers a callback function used to relay events from a SHAKE device
    #   Events include: button movements, capacitive sensor thresholds, shaking detection, heart rate readings, RFID detection
    # 
    #   @param callbackfunc a Python function that takes two parameters. When called, the first parameter will be the shake_device 
    #       object that the callback was generated by, and the second parameter will be an integer indicating the type of event. 
    #
    #   @return SHAKE_SUCCESS or SHAKE_ERROR
    def register_event_callback(self, callbackfunc):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.register_event_callback(self.__shakedev, callbackfunc, self)

    ##  Registers a callback function used to relay audio data to/from a SHAKE device with the audio
    #   expansion module installed.
    #   
    #   @param callbackfunc a Python function that takes 3 parameters. The first parameter is always the shake_device object
    #   associated with the callback. The other two parameters are (in order) the latest set of audio samples from the SHAKE
    #   stored in a Python list (16-bit PCM format) and a second list that can be used to pass audio data TO the SHAKE (again
    #   in 16-bit PCM format). Note that only recording OR playback is supported, you cannot enable both recording and playback
    #   at the same time. The parameter that is not used in the current mode will be set to None, eg in recording mode the 3rd
    #   parameter will always be None. The length of both the recording and playback lists is always SHAKE_AUDIO_DATA_LEN when
    #   they are not set to None. 
    #
    #   @return SHAKE_SUCCESS or SHAKE_ERROR
    def register_audio_callback(self, callbackfunc):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.register_audio_callback(self.__shakedev, callbackfunc, self)

    ##  Starts playback of logged data from the internal memory on the SHAKE to a local file.
    #   IMPORTANT: this function will return immediately - this does NOT mean that logging
    #   playback has been completed. To receive notification of the end of the logging process
    #   use the register_event_callback function and watch for the event SHAKE_PLAYBACK_COMPLETE
    #   
    #   @param filename the local file to write the data into
    #
    #   @ return SHAKE_SUCCESS or SHAKE_ERROR
    def logging_play(self, filename):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.logging_play(self.__shakedev, u'' + filename);

    ##  Pauses any current logging recording/playback. To resume call logging_record or logging_play
    #   as appropriate
    #
    #   @return SHAKE_SUCCESS or SHAKE_ERROR
    def logging_pause(self):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_PAUSE)

    ##  Stops recording or playback AND resets the read/write position to the start of internal memory. 
    #   If an output logfile has been opened by the logging_play function, it will be closed. 
    # 
    #   @return SHAKE_SUCCESS or SHAKE_ERROR
    def logging_stop(self):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_STOP)

    ##  Starts logging data into internal memory from the current writing position.
    #   Call logging_stop first if you want to reset the writing position to the start of internal memory
    # 
    #   Note that while recording is in progress, changes to the SHAKE configuration registers (using the register access functions)
    #   will be disallowed. If you need to change sample rates etc mid-recording, call logging_pause, change the registers
    #   then resume logging by calling logging_record again (if you do not call logging_stop first, it will resume
    #   recording at the memory position where it left off)
    #
    #   @return SHAKE_SUCCESS or SHAKE_ERROR
    def logging_record(self):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RECORD)

    ##  Resets the data logging timestamp clock to zero
    #   Should usually only be sent prior to a logging_record command
    def logging_reset(self):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_RESET)

    ##  Powers down the Bluetooth module to increase logging battery life
    def logging_bt_power_down(self):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_LOGGING_CTRL, SHAKE_LOGGING_BT_POWER_DOWN)

    ##  Indicates the current status of data logging on the SHAKE
    #   The value returned will be a bitwise combination of these three values:
    #       - SHAKE_LOGGING_RECORDING   (recording is in progress)
    #       - SHAKE_LOGGING_PLAYING     (playback is in progress)
    #       - SHAKE_LOGGING_MEMORYFULL  (memory is almost full, <1% available
    #
    #   Obviously only one of the RECORDING and PLAYING values should be set at any time
    #
    #   @return SHAKE_SUCCESS or SHAKE_ERROR
    def logging_status(self):
        if not self.__connected:
            return SHAKE_ERROR

        return self.read(SHAKE_VO_REG_LOGGING_STATUS)

    ##  Indicates the proportion of internal memory remaining free for data logging
    #   
    #   @return SHAKE_ERROR or a value between 0 and 255, indicating the proportion of free space remaining.
    #   A value of 0 means no free space, 255 means all free space. 
    def logging_memory_status(self):
        if not self.__connected:
            return SHAKE_ERROR

        return self.read(SHAKE_VO_REG_LOGGING_MEM)

    ##  Returns the approximate number of packets logged to internal memory
    #
    #   @return SHAKE_ERROR or the number of packets logged (accurate to the nearest 100)
    def logging_packet_count(self):
        if not self.__connected:
            return SHAKE_ERROR

        ret = self.read(SHAKE_NV_REG_LOGGING_PKT_LSB)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR
        lsb = ret[1]

        ret = self.read(SHAKE_NV_REG_LOGGING_PKT_MSB)
        if ret[0] == SHAKE_ERROR:
            return SHAKE_ERROR
        msb = ret[1]

        return 100 * ((msb << 8) + lsb)

    ##  During playback of logged data, this function can be used to count incoming packets.
    #   Unlike the logging_packet_count function, which indicates the total number of packets stored
    #   in the SHAKE itself, this function queries a local value which gives the number of packets
    #   received from the SHAKE. This value can then be used to estimate the overall progress of the
    #   playback operation. 
    #
    #   @return SHAKE_ERROR or the number of packets read
    def logging_packets_read(self):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.logging_packets_read(self.__shakedev)

    def read_power_state(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_POWER1)

    def write_power_state(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        if value < 0 or value > 255:
            return SHAKE_ERROR
        return self.write(SHAKE_NV_REG_POWER1, value)

    def read_power_state_2(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_POWER2)

    def write_power_state_2(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        if value < 0 or value > 255:
            return SHAKE_ERROR
        return self.write(SHAKE_NV_REG_POWER2, value)

    def read_data_format(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_DATAFMT)

    def write_data_format(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_DATAFMT, value)

    def read_acc_config(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)
    
        return self.read(SHAKE_NV_REG_ACC_CONFIG)

    def write_acc_config(self, value):
        if not self.__connected:
            return SHAKE_ERROR
    
        return self.write(SHAKE_NV_REG_ACC_CONFIG, value)

    def read_sample_rate(self, sensor):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_ACCOUT + sensor)

    def write_sample_rate(self, sensor, value):
        if not self.__connected:
            return SHAKE_ERROR

        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return SHAKE_ERROR
    
        return self.write(SHAKE_NV_REG_ACCOUT + sensor, value)

    def read_digital_filter(self, sensor):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_DIGFIL_ACC + sensor)
    
    def write_digital_filter(self, sensor, value):
        if not self.__connected:
            return SHAKE_ERROR

        if sensor < SHAKE_SENSOR_ACC or sensor > SHAKE_SENSOR_ANA1:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_DIGFIL_ACC + sensor, value)

    def read_calib_bypass(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_CALIB_BYPASS)

    def write_calib_bypass(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_CALIB_BYPASS, value)

    def read_cx_bypass(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_CX_CALIB_BYPASS)

    def write_cx_bypass(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_CX_CALIB_BYPASS, value)

    def read_packet_streaming(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_STREAM_DISABLE)

    def write_packet_streaming(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_STREAM_DISABLE, value)

    def read_temp_compensation(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_TEMP_COMPENSATION)
    
    def write_temp_compensation(self, value):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.write(SHAKE_NV_REG_TEMP_COMPENSATION, value)

    def sk6_read_cs0_inc(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS0_INC)

    def sk6_write_cs0_inc(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS0_INC, value)

    def sk6_read_cs0_dec(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS0_DEC)

    def sk6_write_cs0_dec(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS0_DEC, value)

    def sk6_read_cs0_inc_profile(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS0_INC_PROFILE)

    def sk6_write_cs0_inc_profile(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS0_INC_PROFILE, value)

    def sk6_read_cs0_dec_profile(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS0_DEC_PROFILE)

    def sk6_write_cs0_dec_profile(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS0_DEC_PROFILE, value)

    def sk6_read_cs1_inc(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS1_INC)

    def sk6_write_cs1_inc(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS1_INC, value)

    def sk6_read_cs1_dec(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS1_DEC)

    def sk6_write_cs1_dec(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS1_DEC, value)

    def sk6_read_cs1_inc_profile(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS1_INC_PROFILE)

    def sk6_write_cs1_inc_profile(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS1_INC_PROFILE, value)

    def sk6_read_cs1_dec_profile(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SK6_NV_REG_CS1_DEC_PROFILE)

    def sk6_write_cs1_dec_profile(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SK6_NV_REG_CS1_DEC_PROFILE, value)

    def sk6_read_cap_thresholds(self):
        if not self.__connected:
            return (SHAKE_ERROR, [])

        values = []
        for i in range(SK6_NV_REG_CS0_INC, SK6_NV_REG_CS1_DEC_PROFILE+1):
            ret = self.read(i)
            if ret[0] == SHAKE_ERROR:
                return (SHAKE_ERROR, [])
            values.append(ret[1])

        return (SHAKE_SUCCESS, values)
        
    def sk6_write_cap_thresholds(self, values):
        if not self.__connected or len(values) != 8:
            return SHAKE_ERROR

        offset = 0
        for i in range(SK6_NV_REG_CS0_INC, SK6_NV_REG_CS1_DEC_PROFILE+1):
            ret = self.write(i, values[offset])
            if ret == SHAKE_ERROR:
                return SHAKE_ERROR
            offset += 1

        return SHAKE_ERROR

    def sk7_read_cap_thresholds(self):
        if not self.__connected:
            return (SHAKE_ERROR, [])

        values = []
        for i in range(SK7_NV_REG_CAP_INC, SK7_NV_REG_CS1_DEC_PROFILE+1):
            ret = self.read(i)
            if ret[0] == SHAKE_ERROR:
                return (SHAKE_ERROR, [])
            values.append(ret[1])

        return (SHAKE_SUCCESS, values)
        
    def sk7_write_cap_thresholds(self, values):
        if not self.__connected or len(values) != 8:
            return SHAKE_ERROR

        offset = 0
        for i in range(SK7_NV_REG_CAP_INC, SK7_NV_REG_CS1_DEC_PROFILE+1):
            ret = self.write(i, values[offset])
            if ret == SHAKE_ERROR:
                return SHAKE_ERROR
            offset += 1

        return SHAKE_ERROR

    def read_audio_config(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_AUDIO_CONFIG)

    def write_audio_config(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_AUDIO_CONFIG, value)

    def write_midi_amplitude(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_MIDI_AMPLITUDE, value)

    def write_midi_note(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_MIDI_NOTE, value)

    def write_midi_waveform(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_MIDI_WAVEFORM, value)

    def read_shaking_config(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_SHAKING_CONFIG)

    def write_shaking_config(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_SHAKING_CONFIG, value)

    def read_shaking_accel_threshold(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD)

    def write_shaking_accel_threshold(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_SHAKING_ACCEL_THRESHOLD, value)

    def read_shaking_holdoff_time(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME)

    def write_shaking_holdoff_time(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, value)

    def read_shaking_vibration_profile(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE)

    def write_shaking_vibration_profile(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, value)

    def read_shaking_hpf_constant(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_SHAKING_HPF_CONSTANT)

    def write_shaking_hpf_constant(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_SHAKING_HPF_CONSTANT, value)

    def read_shaking_lpf_constant(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_SHAKING_LPF_CONSTANT)

    def write_shaking_lpf_constant(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_SHAKING_LPF_CONSTANT, value)

    def reset_shaking_detection(self):
        if not self.__connected:
            return SHAKE_ERROR

        if self.write(SHAKE_NV_REG_SHAKING_CONFIG, 0xFF) == SHAKE_ERROR:
            return SHAKE_ERROR
        if self.write(SHAKE_NV_REG_SHAKING_HOLDOFF_TIME, 0x0A) == SHAKE_ERROR:
            return SHAKE_ERROR
        if self.write(SHAKE_NV_REG_SHAKING_VIBRATION_PROFILE, 0x00) == SHAKE_ERROR:
            return SHAKE_ERROR
        if self.write(SHAKE_NV_REG_SHAKING_HPF_CONSTANT, 0x20) == SHAKE_ERROR:
            return SHAKE_ERROR
        if self.write(SHAKE_NV_REG_SHAKING_LPF_CONSTANT, 0x06) == SHAKE_ERROR:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def read_heart_rate_config(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_HEART_RATE_CONFIG)
    
    def write_heart_rate_config(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_HEART_RATE_CONFIG, value)

    def read_expansion_config(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_NV_REG_EXPANSION_CONFIG)
    
    def write_expansion_config(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_NV_REG_EXPANSION_CONFIG, value)

    def read_rfid_config(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)
        return self.read(SHAKE_NV_REG_RFID_CONFIG)

    def write_rfid_config(self, value):
        if not self.__connected:
            return SHAKE_ERROR
        return self.write(SHAKE_NV_REG_RFID_CONFIG, value)

    def read_rfid_scan_freq(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)
        return self.read(SHAKE_NV_REG_RFID_FREQUENCY)

    def write_rfid_scan_freq(self, value):
        if not self.__connected:
            return SHAKE_ERROR
        return self.write(SHAKE_NV_REG_RFID_FREQUENCY)

    def write_packet_request(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_PKTREQ, value)

    def write_data_request(self, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_DATAREQ, value)

    def playvib(self, channel, value):
        if not self.__connected:
            return SHAKE_ERROR

        if value < 0 or value > 255:
            return SHAKE_ERROR

        return pyshake.playvib(self.__shakedev, channel, value)

    # SK6 + V01 module only
    def sk6_playvib_continuous(self, channel, amplitude, time):
        if not self.__connected:
            return SHAKE_ERROR

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

        vibaddr = SHAKE_VO_REG_VIB_LEFT_CONTINUOUS;
        if(channel == SHAKE_VIB_RIGHT):
            vibaddr = SHAKE_VO_REG_VIB_RIGHT_CONTINUOUS;
        return self.write(vibaddr, vibbyte)

    # Simple upload format for SK6 (can also be used on SK7 if you don't care about mode/duty/freq parameters)
    def sk6_upload_vib_sample(self, profile_number, sample_list):
        if not self.__connected:
            return SHAKE_ERROR
    
        if len(sample_list) > 64 or len(sample_list) < 2:
            return SHAKE_ERROR

        return pyshake.sk6_upload_vib_sample(self.__shakedev, profile_number, sample_list)

    def upload_vib_sample_extended(self, profile_number, sample_list, mode, freq, duty):
        if not self.__connected:
            return SHAKE_ERROR

        if len(sample_list) > 64 or len(sample_list) < 2:
            return SHAKE_ERROR

        return pyshake.upload_vib_sample_extended(self.__shakedev, profile_number, sample_list, mode, freq, duty)

    def read_temperature(self):
        if not self.__connected:
            return -1.0

        return pyshake.read_temperature(self.__shakedev)

    def read_battery_level(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_VO_REG_BATTERY)

    def read_power_status(self):
        if not self.__connected:
            return (SHAKE_ERROR, 0)

        return self.read(SHAKE_VO_REG_PWRSTA)

    def exp_write_gp_register(self, reg, value):
        if not self.__connected:
            return SHAKE_ERROR

        return self.write(SHAKE_VO_REG_EXP_PWM1 + reg, value)

    def upload_audio_sample(self, address, samples):
        if not self.__connected:
            return SHAKE_ERROR

        if len(samples) == 0:
            return SHAKE_ERROR

        return pyshake.upload_audio_sample(self.__shakedev, address, samples)

    def play_audio_sample(self, start_addr, end_addr, amplitude):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.play_audio_sample(self.__shakedev, start_addr, end_addr, amplitude)

    def register_audio_callback(self, callbackfunc):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.register_audio_callback(self.__shakedev, callbackfunc, self)

    def exp_upload_vib_sample(self, address, samples):
        if not self.__connected:
            return SHAKE_ERROR

        if len(samples) == 0:
            return SHAKE_ERROR

        return pyshake.exp_upload_vib_sample(self.__shakedev, address, samples)

    def exp_play_vib_sample(self, start_addr, end_addr, amplitude):
        if not self.__connected:
            return SHAKE_ERROR

        return pyshake.exp_play_vib_sample(self.__shakedev, start_addr, end_addr, amplitude)

    def wait_for_acks(self, wait_for_ack):
        if not self.__connected:
            return SHAKE_ERROR

        pyshake.wait_for_acks(self.__shakedev, wait_for_ack)
        return SHAKE_SUCCESS

    def sk7_override_led(self, r, g, b):
        if not self.__connected:
            return SHAKE_ERROR

        ret1 = self.write(SHAKE_VO_REG_LED_RED, r)
        ret2 = self.write(SHAKE_VO_REG_LED_GREEN, g)
        ret3 = self.write(SHAKE_VO_REG_LED_BLUE, b)

        if ret1 == SHAKE_ERROR or ret2 == SHAKE_ERROR or ret3 == SHAKE_ERROR:
            return SHAKE_ERROR

        return SHAKE_SUCCESS
        
    def sk7_configure_heading_feedback(self, lower_threshold, upper_threshold, hysteresis, vib_profile):
        if self.write(SK7_NV_REG_HEADING_LOWER_LSB, lower_threshold & 0x00FF) != SHAKE_SUCCESS:
            return SHAKE_ERROR

        if self.write(SK7_NV_REG_HEADING_LOWER_MSB, (lower_threshold & 0xFF00) >> 8) != SHAKE_SUCCESS:
            return SHAKE_ERROR

        if self.write(SK7_NV_REG_HEADING_UPPER_LSB, upper_threshold & 0x00FF) != SHAKE_SUCCESS:
            return SHAKE_ERROR

        if self.write(SK7_NV_REG_HEADING_UPPER_MSB, (upper_threshold & 0xFF00) >> 8) != SHAKE_SUCCESS:
            return SHAKE_ERROR

        if self.write(SK7_NV_REG_HEADING_HYSTERESIS, hysteresis) != SHAKE_SUCCESS:
            return SHAKE_ERROR

        if self.write(SK7_NV_REG_HEADING_VIB_PROFILE, vib_profile) != SHAKE_SUCCESS:
            return SHAKE_ERROR

        return SHAKE_SUCCESS

    def sk7_control_heading_feedback(self, enabled, vib_looping, led_feedback):
        val = 0
        if enabled:
            val |= 1
        if vib_looping:
            val |= 2
        if led_feedback:
            val |= 4
        return self.write(SK7_NV_REG_HEADING_FEEDBACK, val)

    def sk7_configure_roll_pitch_heading(self, val):
        return self.write(SK7_NV_REG_RPH_CONFIG, val)

