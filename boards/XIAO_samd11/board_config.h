#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define USB_VID 0x2886
#if defined(NRF54L15)
#define USB_PID 0x8066
#elif defined(NRF54LM20A)
#define USB_PID 0x8068
#elif defined(MG24)
#define USB_PID 0x8062
#else
#error "Unsupported board variant"
#endif

#define VENDOR_NAME "Seeed Studio"
#define PRODUCT_NAME "XIAO SAMD11"
#define VOLUME_LABEL "SAMD11BOOT"
#define BOARD_ID "SAMD11D14A-XIAO-v0"

// USB pins
#define DM_PIN 24
#define DP_PIN 25
#define DM_MUX 6
#define DP_MUX 6

#endif
