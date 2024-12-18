#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define VENDOR_NAME "MycoSense"
#define PRODUCT_NAME "DLP BOARD V3"
#define VOLUME_LABEL "DLP_BOARD"
#define INDEX_URL "https://github.com/Mycosense/SL-xiao-bootloader"

#define BOARD_ID "DLP-v3"

#define BOOT_USART_MODULE                 SERCOM2
#define BOOT_USART_BUS_CLOCK_INDEX        PM_APBCMASK_SERCOM2
#define BOOT_USART_PER_CLOCK_INDEX        GCLK_CLKCTRL_ID_SERCOM2_CORE_Val
#define BOOT_USART_PAD_SETTINGS           UART_RX_PAD3_TX_PAD2 
#define BOOT_USART_PAD3                   PINMUX_PA15C_SERCOM2_PAD3
#define BOOT_USART_PAD2                   PINMUX_PA14C_SERCOM2_PAD2
#define BOOT_USART_PAD1                   PINMUX_UNUSED 
#define BOOT_USART_PAD0                   PINMUX_UNUSED

/* Master clock frequency */
//#define CPU_FREQUENCY                     (48000000ul)
#define VARIANT_MCK                       CPU_FREQUENCY

/* Frequency of the board main oscillator */
#define VARIANT_MAINOSC                   (32768ul)
#define CRYSTALLESS                       (1)

/* Calibration values for DFLL48 pll */
//#define NVM_SW_CALIB_DFLL48M_COARSE_VAL   (58)
//#define NVM_SW_CALIB_DFLL48M_FINE_VAL     (64)

#endif
