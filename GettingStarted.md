# Summary #
This page gives a brief overview of the different versions of the SHAKE driver available and describes the differences between each. It also describes the methods available to connect to a SHAKE device on each supported platform, as some platforms support multiple connection types.

## Driver versions ##
The basic SHAKE driver has 3 separate versions, each written in a different language:
  1. A **C++** implementation (with interfaces to other languages such as Python, Java and C#). This can be found in the **cpp** directory of the SVN repository.
  1. A pure **Python** implementation. This can be found in the **python** directory of the SVN repository.
  1. A pure **Java** implementation. This can be found in the **java** directory of the SVN repository.

Of the 3, the C++ driver will usually be the most up to date and support most features, while the Python and Java implementations may lag behind in some places or not support more advanced functionality. All the drivers have the same general style of interface with some obvious differences due to the differences in the languages themselves.

The C++ driver can be compiled on Windows, Linux (including Maemo) and OSX. The Python and Java interfaces for the driver can also be compiled on these operating systems. The C# interface has only been used successfully on Windows so far.

The pure Python driver can be used on Windows, Linux (including Maemo), OSX and Nokia Series60 devices (via PythonS60). The only external dependency it requires is a serial port interface. On most systems [pyserial](http://pyserial.sourceforge.net/) provides the required functionality, but on S60 devices a native Bluetooth module is provided by the PythonS60 platform.

The pure Java driver can only be used on mobile devices. Currently it supports J2ME compatible devices (tested on various S60 phones) and also recent Android devices (version 2.1 and later have been tested).

## Connecting to a SHAKE ##

Depending on the platform you are developing for, you may have multiple options for connecting to a SHAKE:
  * Using a Bluetooth 'virtual serial port' (requires that you find the COM port number corresponding to the SHAKE device through your Bluetooth software)
  * Using a Bluetooth RFCOMM connection (requires that you know the Bluetooth address of your SHAKE - this is typically available from your Bluetooth software and has the format **xx:xx:xx:xx:xx:xx**, eg 12:34:56:78:9a)
  * Using a USB serial port with a mini-USB cable to connect the device (requires that you know the port name/number - on Windows this will be a COM port number, as with a Bluetooth serial port. On Linux/OSX, it will typically be called **/dev/something**, eg /dev/ttyUSB3)

The following table shows the supported connection types on each operating system.

| **Connection type** | **Windows** | **Linux** | **Maemo** | **OSX** | Symbian/S60 |
|:--------------------|:------------|:----------|:----------|:--------|:------------|
| Bluetooth serial port | Yes(1) | No | No | No | No |
| Bluetooth RFCOMM | Yes(2) | Yes | Yes | Yes | Yes |
| USB serial port | Yes | Yes | unknown | Yes | No |

(1) The Microsoft Bluetooth stack appears to have a bug which may prevent you from using the virtual serial port to connect to the SHAKE. If this affects you, use the RFCOMM connection method instead.

(2) RFCOMM connections are not currently supported on non-Microsoft Bluetooth stacks (eg Broadcomm, BlueSoleil)