/**
 * Copyright (c) 2024 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdlib.h>
#include <bsp/board_api.h>
#include <tusb.h>
#include <pico/stdio.h>
#include <pico/bootrom.h>

#define BOOTSEL_MASK (1u << 23)

int main(void)
{
  // Initialize TinyUSB stack
  board_init();
  tusb_init();

  // TinyUSB board init callback after init
  if (board_init_after_tusb)
  {
    board_init_after_tusb();
  }

  // let pico sdk use the first cdc interface for std io
  stdio_init_all();

  // main run loop
  while (1)
  {
    // TinyUSB device task | must be called regurlarly
    tud_task();
  }

  // indicate no error
  return 0;
}

// callback when data is received on a CDC interface
void tud_cdc_rx_cb(uint8_t itf)
{
  // allocate buffer for the data in the stack
  uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

  printf("RX CDC %d\n", itf);

  // read the available data
  // | IMPORTANT: also do this for CDC0 because otherwise
  // | you won't be able to print anymore to CDC0
  // | next time this function is called
  uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

  buf[count] = 0; // null-terminate the string
  // now echo data back to the console on CDC 0
  printf("%s\n", buf);
}

// Support for default BOOTSEL reset by changing baud rate to 110
void tud_cdc_line_coding_cb(__unused uint8_t itf, cdc_line_coding_t const *p_line_coding)
{
  if (p_line_coding->bit_rate == 110)
  {
    // board_led_write(1);
    reset_usb_boot(BOOTSEL_MASK, 0);
  }
}