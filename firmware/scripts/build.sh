#!/bin/bash

export PICO_SDK_PATH=../pico-sdk
mkdir -p build
cd build
cmake -DPICO_PLATFORM=rp2350 -DPICO_BOARD=pico2 ..
make

sleep 1

echo "Check your build:"
arm-none-eabi-size firmware.elf