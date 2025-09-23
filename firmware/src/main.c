#include <bsp/board_api.h>
#include <pico/bootrom.h>
#include <pico/stdio.h>
#include <stdlib.h>
#include <tusb.h>

#include "usb_descriptors.h"

#define BOOTSEL_MASK (1u << 23)

void hid_task(void);

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
  }

  // indicate no error
  return 0;
}

// callback when data is received on a CDC interface
void tud_cdc_rx_cb(uint8_t itf) {
  // allocate buffer for the data in the stack
  uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];

  printf("RX CDC %d\n", itf);

  uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

  buf[count] = 0; // null-terminate the string

  // now echo data back to the console on CDC 0
  printf("%s\n", buf);
}

// Support for default BOOTSEL reset by changing baud rate to 110
void tud_cdc_line_coding_cb(__unused uint8_t itf,
                            cdc_line_coding_t const *p_line_coding) {
  if (p_line_coding->bit_rate == 110) {
    // board_led_write(1);
    reset_usb_boot(BOOTSEL_MASK, 0);
  }
}

// HID
static void send_hid_report(uint8_t report_id, uint32_t btn) {
  // skip if hid is not ready yet
  if (!tud_hid_ready())
    return;

  switch (report_id) {
  case REPORT_ID_KEYBOARD:
    // do nothing
    break;

  default:
    break;
  }
}

// Every 10ms, we will sent 1 report for each HID profile
void hid_task(void) {
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if (board_millis() - start_ms < interval_ms)
    return; // not enough time

  start_ms += interval_ms;

  uint32_t const btn = board_button_read();

  // Remote wakeup
  if (tud_suspended() && btn) {
    tud_remote_wakeup();
  } else {
    send_hid_report(REPORT_ID_KEYBOARD, btn);
  }
}

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len) {
  (void)instance;
  (void)len;

  uint8_t next_report_id = report[0] + 1u;
}

// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;

  printf("Receive HID Report");

  // echo back anything we received from host
  tud_hid_report(0, buffer, bufsize);
}