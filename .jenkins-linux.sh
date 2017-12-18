#!/bin/bash -xe

export CFLAGS=-ggdb3
./autogen.sh --enable-warnaserror
make -w
/bin/sh -c 'if make check -k -w; then true; else cat ./tests/test-suite.log; exit 1; fi'
