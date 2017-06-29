#!/bin/bash -xe

export CFLAGS=-ggdb3
./autogen.sh
make -w
make check -k -w
