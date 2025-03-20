#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#define FLASH_ROW_SIZE 256
#define FLASH_NUM_ROWS 1024
#define FLASH_DEVICE_SIZE 0x10000

void flash_erase_row(uint32_t *dst);
void flash_erase_nvm_user_config();
void flash_erase_to_end(uint32_t *start_address);
void flash_write_words(uint32_t *dst, uint32_t *src, uint32_t n_words);
void flash_write_nvm_user_config(uint32_t value);
#ifdef __cplusplus
}
#endif
