CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings -arch x86_64 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk -Iinc"
LDFLAGS="-lm -lpthread"
CPP="/usr/bin/g++"
LIBSHAKE="libshake_driver.so"

rm -f $LIBSHAKE

$CPP -o $LIBSHAKE -shared $CFLAGS src/shake_driver.cpp src/shake_thread.cpp src/shake_serial_usb.cpp src/shake_io.cpp src/shake_packets.cpp src/shake_parsing.cpp src/SK6.cpp src/SK7.cpp src/SHAKE.cpp 

