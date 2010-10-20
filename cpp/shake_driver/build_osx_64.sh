CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings -arch x86_64 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk -Iinc -I/Library/Frameworks/Python.framework/Versions/2.5/include/python2.5 -I/Library/Frameworks/Python.framework/Versions/2.5/include/python2.6 -F/Library/Frameworks -framework Python"
LDFLAGS="-lm -lpthread"
CPP="/usr/bin/g++"

rm -f shake_driver.so pyshake.so

$CPP -o shake_driver.so -shared $CFLAGS src/shake_driver.cpp src/shake_thread.cpp src/shake_serial_osx.cpp src/shake_io.cpp src/shake_packets.cpp src/shake_parsing.cpp src/SK6.cpp src/SK7.cpp src/SHAKE.cpp 

$CPP -o pyshake.so -shared $CFLAGS -I../pyshake/inc -DPYSHAKE_COMPILE ../pyshake/src/pyshake.cpp shake_driver.so  $LDFLAGS
