#!/bin/bash

sudo dnf install cmake -y
sudo dnf install arm-none-eabi-gcc-cs -y
sudo dnf install arm-none-eabi-gcc-cs-c++ -y
sudo dnf install arm-none-eabi-newlib -y

git clone https://github.com/raspberrypi/pico-sdk.git --branch master
cd pico-sdk
git submodule update --init
cd ..
git clone https://github.com/raspberrypi/pico-examples.git --branch master
git clone https://github.com/raspberrypi/picotool.git --branch master