// Minimal startup to reduce size; rely on our own vector table only for SP + Reset
#include "board_config.h"
#include "samd11.h"

extern int main(void);

extern uint32_t _etext, _srelocate, _erelocate, _sbss, _ebss, _estack;

void Reset_Handler(void) {
    uint32_t *src = &_etext;
    uint32_t *dst = &_srelocate;
    while (dst < &_erelocate) *dst++ = *src++;
    for (dst = &_sbss; dst < &_ebss; ) *dst++ = 0;
    main();
    for (;;) {}
}

__attribute__((section(".vectors")))
void (* const _vectors[])(void) = {
    (void (*)(void))(&_estack),
    Reset_Handler,
};
