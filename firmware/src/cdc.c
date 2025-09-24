#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <bsp/board_api.h>
#include <pico/bootrom.h>
#include <pico/stdio.h>
#include <tusb.h>

#include "debug.h"
#include "storage.h"
#include "usb_descriptors.h"

#define BOOTSEL_MASK (1u << 23)

typedef enum { CMD_UNKNOWN, CMD_MKEY, CMD_SKEY } CommandType;

CommandType parse_command(const char *cmd) {
  if (strcmp(cmd, "SKEY") == 0)
    return CMD_SKEY;

  return CMD_UNKNOWN;
}

// callback when data is received on a CDC interface
void tud_cdc_rx_cb(uint8_t itf) {
  // allocate buffer for the data in the stack
  uint8_t buf[CFG_TUD_CDC_RX_BUFSIZE];
  uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

  buf[count] = 0; // null-terminate the string

  // command
  char command[5] = {0}; // 4 chars + null
  uint8_t *arg = NULL;

  // ensure buf have at lease command
  if (count < 4)
    return;

  // Copy first 4 chars as command
  strncpy(command, (char *)buf, 4);
  command[4] = '\0'; // ensure null-terminated

  // Arguments start from index 5
  if (count > 5)
    arg = buf + 5;

  // handle command
  CommandType cmd = parse_command(command);

  switch (cmd) {
  case CMD_SKEY:
    printf("%d\n", PICO_FLASH_SIZE_BYTES);
    if (flash_write_string(SKEY_BLOCK, (char *)arg)) {
      printf("Set SKEY\n");
    };
    break;
  }
}

// Support for default BOOTSEL reset by changing baud rate to 110
void tud_cdc_line_coding_cb(__unused uint8_t itf,
                            cdc_line_coding_t const *p_line_coding) {
  if (p_line_coding->bit_rate == 110) {
    // board_led_write(1);
    reset_usb_boot(BOOTSEL_MASK, 0);
  }
}