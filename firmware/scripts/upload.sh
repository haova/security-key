#!/bin/bash

# reset baud rate to 110 -> enter BOOTSEL mode
stty -F /dev/ttyACM0 110
sleep 1

# get board info
./picotool/build/picotool info

# upload in BOOTSEL mode
./picotool/build/picotool load -f build/firmware.uf2
sleep 1

# reboot
./picotool/build/picotool reboot -f