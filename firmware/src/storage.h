#ifndef STORAGE_H
#define STORAGE_H

typedef enum {
  BOOT_BLOCK = 0,
  MKEY_BLOCK,
  SKEY_BLOCK,
  BLOCK_MAX
} flash_block_t;

bool flash_write_block(flash_block_t block, const uint8_t *data, size_t len);
bool flash_write_string(flash_block_t block, const char *str);
const uint8_t *flash_read_block(flash_block_t block);
const char *flash_read_string(flash_block_t block);

#endif // STORAGE_H