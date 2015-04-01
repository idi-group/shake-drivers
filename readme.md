Shake Drivers
=============
C++ driver with bindings to Python, C# and Java; and drivers fully written in Python or Java.

Installation with OsX
---------------------
for C++:
- compile shake_driver with `sh build_osx_64.sh`
- install pyshake with `python pyshake_distutils_osx.py install`

for Python:
- use python setup.py install
- make sure pyserial is installed.

Quick test with OsX and Bluetooth connection:
---------------------------------------------
Start by pairing the device to your computer, the Shake will appear with its ID in the connection panel, for example `SHAKE SK7 SN0075`. After pairing you will find a device file under `/dev/` with the name of the Shake that just connected, for example `tty.SHAKESK7SN0075-SPPDev`.

Using the C++ drivers in a Python environment:
```
from shake import shake_device
dev = shake_device()
dev.connect_usb('/dev/tty.SHAKESK7SN0075-SPPDev')
dev.gyro()
```

N.B: The C++ based Python bindings also install a module called `pyshake` that could create a naming conflict with Python based module `PyShake`.
