#include "flash.h"
#include "samd.h"

// this actually generates less code than a function
#define wait_ready()                                                                               \
    while (NVMCTRL->INTFLAG.bit.READY == 0)                                                        \
        ;

void flash_erase_row(uint32_t *dst) {
    wait_ready();
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

    // Execute "ER" Erase Row
    NVMCTRL->ADDR.reg = (uint32_t)dst / 2;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
    wait_ready();
}

void flash_erase_nvm_user_config() {
    wait_ready();
    NVMCTRL->STATUS.reg = NVMCTRL_STATUS_MASK;

    // Execute "EAR" Erase Row
    NVMCTRL->ADDR.reg = (uint32_t)NVMCTRL_USER / 2;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_EAR;
    wait_ready();
}

void flash_erase_to_end(uint32_t *start_address) {
    // Note: the flash memory is erased in ROWS, that is in
    // block of 4 pages.
    //       Even if the starting address is the last byte
    //       of a ROW the entire
    //       ROW is erased anyway.

    uint32_t dst_addr = (uint32_t) start_address; // starting address

    while (dst_addr < FLASH_SIZE) {
        flash_erase_row((void *)dst_addr);
        dst_addr += FLASH_ROW_SIZE;
    }
}

void copy_words(uint32_t *dst, uint32_t *src, uint32_t n_words) {
    while (n_words--)
        *dst++ = *src++;
}

void flash_write_words(uint32_t *dst, uint32_t *src, uint32_t n_words) {
    // Set automatic page write
    NVMCTRL->CTRLB.bit.MANW = 0;

    while (n_words > 0) {
        uint32_t len = (FLASH_PAGE_SIZE >> 2) < n_words ? (FLASH_PAGE_SIZE >> 2) : n_words;
        n_words -= len;

        // Execute "PBC" Page Buffer Clear
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
        wait_ready();

        // make sure there are no other memory writes here
        // otherwise we get lock-ups

        while (len--)
            *dst++ = *src++;

        // Execute "WP" Write Page
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
        wait_ready();
    }
}

void flash_write_nvm_user_config(uint32_t value) {
    // Set automatic page write
    NVMCTRL->CTRLB.bit.MANW = 0;


    // Execute "PBC" Page Buffer Clear
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
    wait_ready();

    // make sure there are no other memory writes here
    // otherwise we get lock-ups
    *(uint32_t*)NVMCTRL_USER = value;

    // Execute "WAP" Write Page
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WAP;
    wait_ready();
}
