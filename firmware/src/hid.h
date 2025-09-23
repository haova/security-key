#ifndef HID_H
#define HID_H

void hid_task(void);
bool hid_queue_push(uint8_t report_id, const uint8_t *buf, uint8_t len);
bool hid_queue_push_keyboard(uint8_t modifier, uint8_t keycodes[6]);
void hid_queue_push_keyboard_release(void);
void hid_type_string(const char *str);

#endif // HID_H