#!/bin/bash

export PICO_SDK_PATH=../../pico-sdk
cd picotool
mkdir -p build
cd build
cmake ..
make