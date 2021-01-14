#!/bin/bash

make check -j12
lcov --capture --directory src --output-file coverage.info
genhtml coverage.info --output-directory coverage
