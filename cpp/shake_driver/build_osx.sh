CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings -arch i386 -mmacosx-version-min=10.5 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk -Iinc"
LDFLAGS="-lm -lpthread"
CPP="/usr/bin/g++"
LIBSHAKE="libshake_driver.so"

rm -f $LIBSHAKE

$CPP -o $LIBSHAKE -shared $CFLAGS src/shake_driver.cpp src/shake_thread.cpp src/shake_serial_usb.cpp src/shake_io.cpp src/shake_packets.cpp src/shake_parsing.cpp src/SK6.cpp src/SK7.cpp src/SHAKE.cpp 

