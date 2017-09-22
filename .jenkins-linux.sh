#!/bin/bash -xe

export CFLAGS=-ggdb3
./autogen.sh --enable-warnaserror
make -w
make check -k -w
