// Startup code for SAMD11 (similar style to SAMD21 but trimmed)
#include "board_config.h"
#include "samd11.h"

/* Linker script symbols */
extern uint32_t _etext, _srelocate, _erelocate, _sbss, _ebss, _estack;

int main(void);

/* Weak default handlers */
void Dummy_Handler(void) __attribute__((weak));
void Dummy_Handler(void) { while (1) { } }

void NMI_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void HardFault_Handler(void)    __attribute__((weak, alias("Dummy_Handler")));
void SVC_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void PendSV_Handler(void)       __attribute__((weak, alias("Dummy_Handler")));
// SysTick_Handler has a concrete implementation later (no weak alias needed)

/* Peripheral IRQs present on SAMD11 (see datasheet) */
void PM_Handler(void)           __attribute__((weak, alias("Dummy_Handler")));
void SYSCTRL_Handler(void)      __attribute__((weak, alias("Dummy_Handler")));
void WDT_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void RTC_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void EIC_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void NVMCTRL_Handler(void)      __attribute__((weak, alias("Dummy_Handler")));
void DMAC_Handler(void)         __attribute__((weak, alias("Dummy_Handler")));
void USB_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void EVSYS_Handler(void)        __attribute__((weak, alias("Dummy_Handler")));
void SERCOM0_Handler(void)      __attribute__((weak, alias("Dummy_Handler")));
void SERCOM1_Handler(void)      __attribute__((weak, alias("Dummy_Handler")));
void SERCOM2_Handler(void)      __attribute__((weak, alias("Dummy_Handler")));
void TCC0_Handler(void)         __attribute__((weak, alias("Dummy_Handler")));
void TC1_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void TC2_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void ADC_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void AC_Handler(void)           __attribute__((weak, alias("Dummy_Handler")));
void DAC_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));
void PTC_Handler(void)          __attribute__((weak, alias("Dummy_Handler")));

// Provided by main.c for LED timing
void LED_TICK(void) __attribute__((weak));

// Provide non-weak implementation (overrides weak alias) for LED tick support
void SysTick_Handler(void) { if (LED_TICK) LED_TICK(); }

void Reset_Handler(void) {
    uint32_t *src = &_etext;
    uint32_t *dst = &_srelocate;
    while (dst < &_erelocate) *dst++ = *src++;
    for (dst = &_sbss; dst < &_ebss; ) *dst++ = 0;
    // Set VTOR to start of flash (vector table at 0x0000)
    SCB->VTOR = (0x00000000 & SCB_VTOR_TBLOFF_Msk);
    // Minimal NVM workaround similar to SAMD21
    NVMCTRL->CTRLB.bit.MANW = 1;
    main();
    while (1) {}
}

// Vector table (DeviceVectors layout is different on SAMD11 vs SAMD21; build a flat array)
__attribute__((section(".vectors")))
void (* const _vectors[])(void) = {
    (void (*)(void))(&_estack), // Initial Stack Pointer
    Reset_Handler,              // Reset
    NMI_Handler,                // NMI
    HardFault_Handler,          // HardFault
    0, 0, 0, 0, 0, 0, 0,        // Reserved (up to SVCall)
    SVC_Handler,                // SVCall
    0, 0,                       // Reserved
    PendSV_Handler,             // PendSV
    SysTick_Handler,            // SysTick
    // Peripheral IRQs (follow order in samd11.h IRQn enum)
    PM_Handler,       // 0 Power Manager
    SYSCTRL_Handler,  // 1 System Control
    WDT_Handler,      // 2 Watchdog
    RTC_Handler,      // 3 RTC
    EIC_Handler,      // 4 External Interrupt
    NVMCTRL_Handler,  // 5 NVM
    DMAC_Handler,     // 6 DMAC
    USB_Handler,      // 7 USB
    EVSYS_Handler,    // 8 EVSYS
    SERCOM0_Handler,  // 9 SERCOM0
    SERCOM1_Handler,  // 10 SERCOM1
    SERCOM2_Handler,  // 11 SERCOM2
    TCC0_Handler,     // 12 TCC0
    TC1_Handler,      // 13 TC1
    TC2_Handler,      // 14 TC2
    ADC_Handler,      // 15 ADC
    AC_Handler,       // 16 AC
    DAC_Handler,      // 17 DAC
    PTC_Handler,      // 18 PTC
};
