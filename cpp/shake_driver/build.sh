# Compiles the SHAKE C++ library and the Python/C++ extension module
# 	Requires: gcc, libbluetooth, python (including headers)

CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings"
LDFLAGS="-lm -lbluetooth -lpthread -lpython2.5"

rm -f shake_driver.so pyshake.so

/usr/bin/g++ $CFLAGS -Iinc -shared -o shake_driver.so src/shake_driver.cpp src/shake_thread.cpp src/shake_rfcomm.cpp src/shake_serial_osx.cpp src/shake_io.cpp src/shake_packets.cpp src/shake_parsing.cpp src/SK6.cpp src/SK7.cpp src/SHAKE.cpp $LDFLAGS

/usr/bin/g++ $CFLAGS -I../pyshake/inc -Iinc -DPYSHAKE_COMPILE -shared -o pyshake.so ../pyshake/src/pyshake.cpp shake_driver.so  $LDFLAGS
