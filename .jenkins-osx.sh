#!/bin/bash -xe

if [[ ${label} == *'osx-i386'* ]]; then
	EXTRA_CFLAGS="$EXTRA_CFLAGS -m32 -arch i386 -mmacosx-version-min=10.9"
	EXTRA_LDFLAGS="$EXTRA_LDFLAGS -m32 -arch i386"
	EXTRA_CONF_FLAGS="--host=i386-apple-darwin13.0.0 --build=i386-apple-darwin13.0.0"
else	
	EXTRA_CFLAGS="$EXTRA_CFLAGS -m64 -arch x86_64 -mmacosx-version-min=10.9"
	EXTRA_LDFLAGS="$EXTRA_LDFLAGS -m64 -arch x86_64" 
fi

export CFLAGS=-ggdb3
PKG_CONFIG_PATH=/opt/X11/lib/pkgconfig ./autogen.sh --enable-warnaserror CFLAGS="$CFLAGS $EXTRA_CFLAGS" LDFLAGS="$LDFLAGS $EXTRA_LDFLAGS" $EXTRA_CONF_FLAGS
make -w
make check -k -w && result=0 || result=$?

if [ $result -ne 0 ]; then
    cat ./tests/test-suite.log
    exit $result
fi
