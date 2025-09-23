#include <bsp/board_api.h>
#include <pico/stdio.h>
#include <stdlib.h>
#include <tusb.h>

#include "hid.h"
#include "usb_descriptors.h"

void custom_task() {
  static bool bootsel_pressed = false;
  uint32_t const btn = board_button_read();

  // relesed
  if (!btn) {
    bootsel_pressed = false;
    return;
  }

  // holding button
  if (btn && bootsel_pressed)
    return;

  // pressed
  bootsel_pressed = true;

  // do something
  hid_type_string("Hello, World!\n");
}

int main(void) {
  // Initialize TinyUSB stack
  board_init();
  tusb_init();

  // TinyUSB board init callback after init
  if (board_init_after_tusb) {
    board_init_after_tusb();
  }

  // let pico sdk use the first cdc interface for std io
  stdio_init_all();

  // main run loop
  while (1) {
    // TinyUSB device task | must be called regurlarly
    tud_task();

    // HID
    hid_task();

    // custom task
    custom_task();
  }

  // indicate no error
  return 0;
}
