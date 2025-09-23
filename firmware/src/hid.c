#include <bsp/board_api.h>
#include <pico/stdio.h>
#include <tusb.h>

#include "usb_descriptors.h"

// hid queue ~2.1 KB
#define HID_QUEUE_SIZE 32
#define HID_REPORT_MAX 64

typedef struct {
  uint8_t report_id;
  uint8_t len;
  uint8_t data[HID_REPORT_MAX];
} hid_report_t;

static hid_report_t hid_queue[HID_QUEUE_SIZE];
static uint8_t hid_head = 0;
static uint8_t hid_tail = 0;

// typing a string
const char *typing_ptr = NULL; // currently typing string
volatile bool typing_active = false;
volatile bool hid_callback = false;

// key mapping
uint8_t const hid_ascii_to_keycode[128][2] = {HID_ASCII_TO_KEYCODE};

// hid queue
bool hid_queue_push(uint8_t report_id, const uint8_t *buf, uint8_t len) {
  uint8_t next = (hid_head + 1) % HID_QUEUE_SIZE;
  if (next == hid_tail)
    return false; // queue full

  if (len > HID_REPORT_MAX)
    len = HID_REPORT_MAX;
  hid_queue[hid_head].report_id = report_id;
  hid_queue[hid_head].len = len;
  memcpy(hid_queue[hid_head].data, buf, len);
  hid_head = next;
  return true;
}

// push key
bool hid_queue_push_keyboard(uint8_t modifier, uint8_t keycodes[6]) {
  uint8_t report[8];
  report[0] = modifier; // modifier byte (Shift, Ctrl, Alt)
  report[1] = 0;        // reserved
  memcpy(&report[2], keycodes, 6);

  return hid_queue_push(REPORT_ID_KEYBOARD, report, sizeof(report));
}

// release key
void hid_queue_push_keyboard_release(void) {
  uint8_t report[8] = {0}; // all zeros: no modifier, no keys
  hid_queue_push(REPORT_ID_KEYBOARD, report, sizeof(report));
}

// typing
void hid_type_string(const char *str) {
  // Safe replacement: discard remaining characters
  typing_ptr = str;
  typing_active = true;
}

void hid_type_push_next_char(void) {
  if (typing_active && typing_ptr &&
      ((hid_head + 1) % HID_QUEUE_SIZE != hid_tail)) {

    uint8_t keycode[6] = {0};
    uint8_t modifier = 0;
    uint8_t c = (uint8_t)(*typing_ptr & 0x7F);

    keycode[0] = hid_ascii_to_keycode[c][1];
    if (hid_ascii_to_keycode[c][0])
      modifier = KEYBOARD_MODIFIER_LEFTSHIFT;

    // push to queue: press + release
    hid_queue_push_keyboard(modifier, keycode);
    hid_queue_push_keyboard_release();

    // advance pointer
    typing_ptr++;
    if (*typing_ptr == 0)
      typing_active = false;
  }
}

void hid_send_from_queue() {
  if (hid_tail != hid_head) {
    hid_report_t *rpt = &hid_queue[hid_tail];
    tud_hid_report(rpt->report_id, rpt->data, rpt->len);
    hid_callback = true;
  }

  if (hid_tail == hid_head && hid_callback) {
    hid_callback = false;
    uint8_t report[8] = {0}; // all zeros: no keys, no modifier
    tud_hid_report(REPORT_ID_KEYBOARD, report, sizeof(report));
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

  // Remote wakeup example (optional)
  uint32_t btn = board_button_read();
  if (tud_suspended() && btn) {
    tud_remote_wakeup();
    return;
  }

  // Prevent handle queue while typing
  if (hid_callback)
    return;

  // Prevent handling when not ready
  if (!tud_hid_ready())
    return;

  // Push first character of typing string if queue has space
  hid_type_push_next_char();

  // Send next report from queue
  hid_send_from_queue();
}

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len) {
  (void)instance;
  (void)len;

  // Prevent handling when not ready
  if (!tud_hid_ready()) {
    hid_callback = false;
  }

  // advance queue tail
  if (hid_tail != hid_head)
    hid_tail = (hid_tail + 1) % HID_QUEUE_SIZE;

  // Push first character of typing string if queue has space
  hid_type_push_next_char();

  // Send next report from queue
  hid_send_from_queue();
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