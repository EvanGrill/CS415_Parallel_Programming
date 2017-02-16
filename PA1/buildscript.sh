#!/bin/bash
BUILD_DIR="build/"

# building setup
if [ ! -d "$BUILD_DIR" ]; then
	mkdir build
fi
cd build
cp ../makefile .