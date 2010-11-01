CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings"
LDFLAGS="-lm -lbluetooth -lpthread"
LIBSHAKE="libshake_driver.so"

rm -f $LIBSHAKE

/usr/bin/g++ $CFLAGS -Iinc -shared -o $LIBSHAKE src/shake_driver.cpp src/shake_thread.cpp src/shake_rfcomm.cpp src/shake_serial_usb.cpp src/shake_io.cpp src/shake_packets.cpp src/shake_parsing.cpp src/SK6.cpp src/SK7.cpp src/SHAKE.cpp $LDFLAGS
