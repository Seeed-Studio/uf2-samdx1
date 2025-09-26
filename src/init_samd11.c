#include "uf2.h"

void system_init(void) {
    // Enable GCLK module
    PM->APBAMASK.reg |= PM_APBAMASK_GCLK;

    // Software reset GCLK
    GCLK->CTRL.reg = GCLK_CTRL_SWRST;
    while (GCLK->STATUS.bit.SYNCBUSY) {}

    // Enable DFLL48M (open loop for size). Optionally we could load calibration.
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)) {}

    // Route DFLL to GCLK0
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(0);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(0) | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_GENEN;
    while (GCLK->STATUS.bit.SYNCBUSY) {}

    // Basic SysTick 1ms@48MHz
    SysTick->LOAD = 48000000/1000 - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk;
}
