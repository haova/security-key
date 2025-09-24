#include <bsp/board_api.h>
#include <hardware/gpio.h>
#include <pico/stdio.h>
#include <stdlib.h>
#include <tusb.h>

#include "hid.h"
#include "usb_descriptors.h"

#define MAX_GPIO 64

const uint BOOTSEL_PIN = MAX_GPIO - 1;
const uint BTN_PIN = 29;

static bool gpio_states[MAX_GPIO] = {false};

// return if button was pressed (just one)
bool btn_read(const uint pin) {
  bool state = false;

  if (pin == BOOTSEL_PIN) {
    state = board_button_read() != 0;
  } else {
    state = gpio_get(pin);
  }

  // relesed
  if (!state) {
    gpio_states[pin] = false;
    return false;
  }

  // holding button
  if (state && gpio_states[pin])
    return false;

  // pressed
  gpio_states[pin] = true;
  return true;
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

  // GPIO
  gpio_init(BTN_PIN);

  // main run loop
  while (1) {
    // TinyUSB device task | must be called regurlarly
    tud_task();

    // HID
    hid_task();

    // custom task
    if (btn_read(BTN_PIN)) {
      hid_type_string("Hello, World!");
    }
  }

  // indicate no error
  return 0;
}
