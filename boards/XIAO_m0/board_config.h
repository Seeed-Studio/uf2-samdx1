#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

//#define CRYSTALLESS    0

#define VENDOR_NAME "MycoSense"
#define PRODUCT_NAME "DLP BOARD XIAO"
#define VOLUME_LABEL "DLP_BOARD"
#define INDEX_URL "https://github.com/Mycosense/SL-xiao-bootloader"

#define BOARD_ID "SAMD21G18A-XIAO-v0"

#define USB_VID 0x2886
#define USB_PID 0x002F

#define LED_PIN PIN_PA17
#define LED_PIN_PULLUP 1
#define LED_TX_PIN PIN_PA19
#define LED_TX_PIN_PULLUP 1
#define LED_RX_PIN PIN_PA18
#define LED_RX_PIN_PULLUP 1

#define BOOT_USART_MODULE                 SERCOM4
#define BOOT_USART_BUS_CLOCK_INDEX        PM_APBCMASK_SERCOM4
#define BOOT_USART_PER_CLOCK_INDEX        GCLK_CLKCTRL_ID_SERCOM4_CORE_Val
#define BOOT_USART_PAD_SETTINGS           UART_RX_PAD1_TX_PAD0
#define BOOT_USART_PAD3                   PINMUX_UNUSED
#define BOOT_USART_PAD2                   PINMUX_UNUSED
#define BOOT_USART_PAD1                   PINMUX_PB09D_SERCOM4_PAD1
#define BOOT_USART_PAD0                   PINMUX_PB08D_SERCOM4_PAD0

#endif
