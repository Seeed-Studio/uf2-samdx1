// Minimal crystal-less DFLL + USB clock setup for SAMD11
#include "uf2.h"
#include "sam.h"

static void gclk_sync(void) { while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) { } }
static void dfll_sync(void) { while ((SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY) == 0) { } }

#define NVM_SW_CALIB_DFLL48M_COARSE_VAL 58

void system_init(void) {
    NVMCTRL->CTRLB.bit.RWS = 1;

    PM->APBAMASK.reg |= PM_APBAMASK_GCLK;
    PM->AHBMASK.reg  |= PM_AHBMASK_USB;   // ensure USB AHB clock
    PM->APBBMASK.reg |= PM_APBBMASK_USB;  // peripheral bus clock

    // GCLK2 = OSC8M
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2);
    gclk_sync();
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_SRC_OSC8M | GCLK_GENCTRL_GENEN;
    gclk_sync();

    // DFLL open loop enable
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    dfll_sync();

    uint32_t coarse = (*((uint32_t *)(NVMCTRL_OTP4) + (NVM_SW_CALIB_DFLL48M_COARSE_VAL / 32)) >> (NVM_SW_CALIB_DFLL48M_COARSE_VAL % 32)) & 0x3F;
    if (coarse == 0x3F) coarse = 0x1F;
    SYSCTRL->DFLLVAL.bit.COARSE = coarse;
    SYSCTRL->DFLLVAL.bit.FINE   = 0x1FF;

    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_CSTEP(0x1F/4) | SYSCTRL_DFLLMUL_FSTEP(10) | 48000;
    dfll_sync();

    // Closed-loop with USB correction
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_BPLCKC | SYSCTRL_DFLLCTRL_ENABLE;
    dfll_sync();

    // Main clock = DFLL
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(0);
    gclk_sync();
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
    gclk_sync();

    // USB clock from GCLK0
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(6) | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_CLKEN;
    gclk_sync();

    // 1ms SysTick
    SysTick->LOAD = 48000000/1000 - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}
