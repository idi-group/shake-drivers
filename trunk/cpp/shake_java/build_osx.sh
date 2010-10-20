CFLAGS="-fPIC -fno-stack-protector -Wno-write-strings -arch x86_64 -mmacosx-version-min=10.5 -isysroot /Developer/SDKs/MacOSX10.5.sdk -Iinc -I../shake_driver/inc -I/Library/Java/Home/include"
LDFLAGS="-lm -lpthread"
CPP="/usr/bin/g++"

rm -f libshake_java.jnilib

$CPP -o libshake_java.jnilib -shared $CFLAGS src/shake_java_stubs.cpp ../shake_driver/shake_driver.so
