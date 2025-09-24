
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <hardware/flash.h>
#include <hardware/sync.h>
#include <pico/flash.h>
#include <pico/stdlib.h>

#include "debug.h"
#include "storage.h"

// Pico, Pico W, Pico 2, RP2040, RP2350 have at least 4 MB QSPI Flash
#define FLASH_TARGET_OFFSET (256 * 1024) // safe offset after program
#define BLOCK_SIZE 64
#define MAX_BLOCKS 32

// ---------- Helpers for flash_safe_execute ----------

// Erase one flash sector
static void call_flash_range_erase(void *param) {
  uint32_t offset = (uint32_t)param;
  flash_range_erase(offset, FLASH_SECTOR_SIZE);
}

// Program one flash page
static void call_flash_range_program(void *param) {
  uintptr_t *p = (uintptr_t *)param;
  uint32_t offset = (uint32_t)p[0];
  const uint8_t *data = (const uint8_t *)p[1];
  flash_range_program(offset, data, FLASH_PAGE_SIZE);
}

// ---------- Core functions ----------

// Read raw block pointer
const uint8_t *flash_read_block(flash_block_t block) {
  if (block >= BLOCK_MAX)
    return NULL;
  return (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET + block * BLOCK_SIZE);
}

// Read string safely (return "" if empty)
const char *flash_read_string(flash_block_t block) {
  const uint8_t *ptr = flash_read_block(block);
  if (!ptr)
    return "";
  if (ptr[0] == 0xFF)
    return "";
  return (const char *)ptr;
}

// Write a 64-byte block safely
bool flash_write_block(flash_block_t block, const uint8_t *data, size_t len) {
  if (block >= BLOCK_MAX || len > BLOCK_SIZE)
    return false;

  uint32_t block_offset = FLASH_TARGET_OFFSET + block * BLOCK_SIZE;
  uint32_t page_offset = block_offset & ~(FLASH_PAGE_SIZE - 1);
  uint8_t page_buf[FLASH_PAGE_SIZE];

  // Read current flash page
  const uint8_t *flash_ptr = (const uint8_t *)(XIP_BASE + page_offset);
  memcpy(page_buf, flash_ptr, FLASH_PAGE_SIZE);

  // Copy new block into page buffer
  uint32_t offset_in_page = block_offset % FLASH_PAGE_SIZE;
  memcpy(page_buf + offset_in_page, data, len);

  // Erase sector first if necessary
  flash_safe_execute(call_flash_range_erase, (void *)(page_offset), UINT32_MAX);

  // Program page safely
  uintptr_t params[] = {page_offset, (uintptr_t)page_buf};
  flash_safe_execute(call_flash_range_program, params, UINT32_MAX);

  return true;
}

// Write null-terminated string to block
bool flash_write_string(flash_block_t block, const char *str) {
  uint8_t buf[BLOCK_SIZE] = {0};
  size_t len = strlen(str);
  if (len >= BLOCK_SIZE)
    len = BLOCK_SIZE - 1; // leave space for null terminator
  memcpy(buf, str, len);
  buf[len] = '\0';
  return flash_write_block(block, buf, BLOCK_SIZE);
}
