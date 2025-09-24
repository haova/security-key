#!/bin/bash

PORT=$(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null | head -n 1)

if [ -z "$PORT" ]; then
  echo "No serial port found"
else
  echo "Using port: $PORT (exit by type: Ctrl+a, / (not Ctrl+/))"
  sleep 1
  ./picotool/build/picotool info
  screen "$PORT" # exit by type: Ctrl+a, / (not Ctrl+/)
fi