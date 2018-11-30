#!/bin/bash -xe

export CFLAGS=-ggdb3
PKG_CONFIG_PATH=/opt/X11/lib/pkgconfig ./autogen.sh --enable-warnaserror
make -w
make check -k -w && result=0 || result=$?

if [ $result -ne 0 ]; then
    cat ./tests/test-suite.log
    exit $result
fi
