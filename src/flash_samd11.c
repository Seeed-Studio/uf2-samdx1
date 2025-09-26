#include "uf2.h"

// Simplified flash API for SAMD11 (similar to SAMD21 but smaller flash).

static void nvm_wait_ready(void) {
    while (!NVMCTRL->INTFLAG.bit.READY) {}
}

void flash_write(uint32_t dst, const void *src, uint32_t len) {
    const uint32_t *s = (const uint32_t*)src;
    while (len) {
        uint32_t addr = dst & ~(FLASH_PAGE_SIZE - 1);
        // Erase row if at row boundary
        if ((dst & (FLASH_ROW_SIZE - 1)) == 0) {
            nvm_wait_ready();
            NVMCTRL->ADDR.reg = addr / 2; // address in 16-bit words
            NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
        }
        // write one page
        uint32_t off_in_page = dst & (FLASH_PAGE_SIZE - 1);
        uint32_t write_words = (FLASH_PAGE_SIZE - off_in_page) / 4;
        if (write_words * 4 > len) write_words = (len + 3)/4;
        uint32_t *dstw = (uint32_t*)(dst);
        for (uint32_t i=0;i<write_words;i++) {
            dstw[i] = s[i];
        }
        nvm_wait_ready();
        NVMCTRL->ADDR.reg = dst / 2;
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
        s += write_words;
        uint32_t wrote = write_words * 4;
        dst += wrote;
        len -= wrote;
    }
}

void flash_erase(uint32_t addr) {
    nvm_wait_ready();
    NVMCTRL->ADDR.reg = (addr & ~(FLASH_ROW_SIZE - 1)) / 2;
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
}

// Simple row write: erase + write row (256 bytes)
void flash_write_row(uint32_t *dst, uint32_t *src) {
    // Compare to skip if identical
    bool diff = false;
    for (int i=0;i<FLASH_ROW_SIZE/4;i++) {
        if (dst[i] != src[i]) { diff = true; break; }
    }
    if (!diff) return;

    // Erase row
    flash_erase((uint32_t)dst);
    // Program row page by page
    for (int page=0; page < FLASH_ROW_SIZE/FLASH_PAGE_SIZE; page++) {
        nvm_wait_ready();
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC; // clear page buffer
        nvm_wait_ready();
        uint32_t *d = dst + page*(FLASH_PAGE_SIZE/4);
        uint32_t *s = src + page*(FLASH_PAGE_SIZE/4);
        for (int i=0;i<FLASH_PAGE_SIZE/4;i++) d[i] = s[i];
        nvm_wait_ready();
        NVMCTRL->ADDR.reg = ((uint32_t)d) / 2;
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    }
}

void copy_words(uint32_t *dst, uint32_t *src, uint32_t n_words) {
    while (n_words--) *dst++ = *src++;
}

void flash_write_words(uint32_t *dst, uint32_t *src, uint32_t n_words) {
    while (n_words) {
        uint32_t chunk = (FLASH_PAGE_SIZE >> 2);
        if (chunk > n_words) chunk = n_words;
        // Clear page buffer
        nvm_wait_ready();
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
        nvm_wait_ready();
        for (uint32_t i=0;i<chunk;i++) dst[i] = src[i];
        nvm_wait_ready();
        NVMCTRL->ADDR.reg = ((uint32_t)dst) / 2;
        NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
        dst += chunk;
        src += chunk;
        n_words -= chunk;
    }
}

void flash_erase_to_end(uint32_t *start_address) {
    uint32_t addr = (uint32_t)start_address & ~(FLASH_ROW_SIZE - 1);
    while (addr < FLASH_SIZE) {
        flash_erase(addr);
        addr += FLASH_ROW_SIZE;
    }
}
