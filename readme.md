# SHAKE Drivers

This project contains drivers for the SHAKE SK6 and SK7 sensor packs. For more information on the devices and their capabilities, [see the wiki](https://github.com/andrewramsay/shake-drivers/wiki/Overview).

This repo contains multiple drivers. The most feature complete implementation is a C++ driver that can be compiled for Windows, OSX and Linux (32/64-bit). This driver also has bindings for Python, C# and Java, although in some cases functionality supported by the C++ code has not yet been exposed through these bindings.

In addition, there are two separate implementations, one written in Python (with a dependency on [pyserial](http://pyserial.sourceforge.net/)) and one written in Java (probably most useful in that it supports Android). 

**NOTE 1**: If you install both the Python/C++ bindings and the pure Python implementation in the same Python environment you may encounter naming conflicts, as both include a module called "pyshake".

**NOTE 2**: If you want to use the SHAKE over a USB cable connection instead of Bluetooth, you'll need to install the [FTDI VCP Driver](http://www.ftdichip.com/Drivers/VCP.htm) for your platform.  

## Installation on Windows

The easiest way to get up and running quickly on Windows is to use the Python implementation. Install pyserial, then run `python setup.py install` from the `shake-drivers/python` directory. 

If you want to use the C++ driver, you'll need to compile it first. Inside the `shake-drivers/cpp` directory, you'll find:

 * shake\_driver (the C++ code)
 * pyshake (Python bindings for shake\_driver)
 * shake\_cs (C# bindings for shake\_driver)
 * shake\_java (Java bindings for shake\_driver)
 * shake\_mex (Basic Matlab bindings for shake\_driver)

Each of these should have a Visual Studio project file that can be used to compile them individually depending on which parts you need. They will probably also compile with gcc/g++ from MingW, but there are no build scripts for this at the moment. 

## Installation on OSX

Again the Python implementation is the easiest way to get started quickly:

 * Install pyserial
 * Run `python setup.py install` from `shake-drivers/python`

If you have the XCode/developer packages installed and want to compile the C++/Python bindings:

 * Run `sh build_osx_64.sh` from `shake-drivers/cpp/shake_driver`
 * Run `python setup_osx.py install` from `shake-drivers/cpp/pyshake`

## Installation on Linux

Similar to OSX. For the Python implementation:

 * Install pyserial
 * Run `python setup.py install` from `shake-drivers/python`

For the C++/Python bindings:

 * Run `sh build.sh` from `shake-drivers/cpp/shake_driver`
 * Run `python setup.py install` from `shake-drivers/cpp/pyshake`

## A basic example

(for more examples see [the wiki page](https://github.com/andrewramsay/shake-drivers/wiki/Examples)

**Python / pyserial**

```python
from pyshake import *
sd = shake_device()

# connection details depend on platform
# Windows (virtual COM port number)
device = 10
# OSX (Serial Port Profile dev node)
# device = '/dev/tty.SHAKESK7SN0077-SPPDev'

sd.connect(device) # returns False if connection failed
print sd.acc() # current accelerometer output
print sd.mag() # current magnetometer output
print sd.gyro() # current gyroscope output (if HW module installed)
sd.close()
```

**Python/C++ bindings**
```python
from shake import *
sd = shake_device()

# Windows: for non-Microsoft Bluetooth stacks, or if you're using USB connection, supply a COM port number
sd.connect(10) 

# Windows: if you're using the Microsoft Bluetooth stack, 
# you can instead connect by specifying the device address directly
sd.connect_rfcomm('aa:bb:cc:dd:ee:ff')

# OSX: always use this function
# (despite the name, it works with Bluetooth and USB connections). 
sd.connect_usb('/dev/tty.SHAKESK7SN0077-SPPDev')

# Linux: Bluetooth device address
sd.connect_rfcomm('aa:bb:cc:dd:ee:ff')

# access data...
print sd.acc(), sd.mag(), sd.gyro()

sd.close()
```


