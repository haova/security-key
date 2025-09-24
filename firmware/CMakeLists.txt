cmake_minimum_required(VERSION 3.13)

include(pico-sdk/external/pico_sdk_import.cmake)

project(firmware C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(firmware src/main.c)
target_sources(firmware PUBLIC
  src/main.c
  src/hid.c
  src/cdc.c
  src/usb_descriptors.c
  src/storage.c
  src/debug.c
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(firmware PUBLIC src)

target_link_libraries(firmware PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board)

pico_enable_stdio_usb(firmware 1)

pico_add_extra_outputs(firmware)
