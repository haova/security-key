#!/bin/bash

PORT=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -n 1)

if [ -z "$PORT" ]; then
  echo "No serial port found"
else
  echo "Using port: $PORT"

  # reset baud rate to 110 -> enter BOOTSEL mode
  stty -F "$PORT" 110
  sleep 1

  # get board info
  ./picotool/build/picotool info

  # upload in BOOTSEL mode
  ./picotool/build/picotool load -f build/firmware.uf2
  sleep 1

  # reboot
  ./picotool/build/picotool reboot -f
    
fi


