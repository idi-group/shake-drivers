# SHAKE Drivers

This project contains drivers for the SHAKE SK6 and SK7 sensor packs. For more information on the devices and their capabilities, [see the wiki](https://github.com/andrewramsay/shake-drivers/wiki).

This repo contains multiple drivers. The most up to date and recommended implementation is in Python (under /python) and should run on any of the 3 major desktop operating systems, and on any other platform that supports serial port access via the [pyserial](http://pyserial.sourceforge.net/) module. It should also support both Python 2 and Python 3.

The other drivers are described below, but note that they have not been maintained for some time and may have various issues on newer systems.

**NOTE**: If you want to use the SHAKE over a USB cable connection instead of Bluetooth, you'll need to install the [FTDI VCP Driver](http://www.ftdichip.com/Drivers/VCP.htm) for your platform. See the [user manual](http://www.dcs.gla.ac.uk/~adr/SHAKE%20SK7%20User%20Manual%20Rev%2007.pdf) for details.

## Simple installation

If you use the recommended Python driver (only depends on pyserial and should work on all the major plaforms that suppprt it), then you should now be able to install via pip:
`pip install shake-drivers`
 
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

## Other drivers

An alternative to the Python implementation is a C++ driver that can be compiled for Windows, OSX and Linux (32/64-bit). This driver also comes with bindings for Python, C# and Java, although in some cases functionality supported by the C++ code has not yet been exposed through these bindings, and the code in this package is often outdated compared to the pure Python implementation/

In addition, there is a basic but functional implementation written in Java (probably only useful in that it supports Android). 

## A basic example

(for more examples see [the wiki page](https://github.com/andrewramsay/shake-drivers/wiki/Examples))

**Python / pyserial**

```python
import pyshake
sd = pyshake.ShakeDevice()

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

**Python / C++ bindings**
```python
import shake
sd = shake.shake_device()

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


