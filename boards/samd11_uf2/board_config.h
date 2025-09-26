#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#define USB_VID 0x2886
#define USB_PID 0x002F

#define USB_MANUFACTURER "SAMD11"
#define USB_PRODUCT "SAMD11 UF2"

#define VENDOR_NAME "SAMD11"
#define PRODUCT_NAME USB_PRODUCT

#define UF2_PRODUCT_NAME "SAMD11 UF2 Bootloader"
#define UF2_BOARD_ID "SAMD11D14-UF2"
#define UF2_VOLUME_LABEL "SAMD11BOOT"
#define UF2_INDEX_URL "https://microchip.com"

// USB pins
#define DM_PIN 24
#define DP_PIN 25
#define DM_MUX 6
#define DP_MUX 6

#define BOARD_ID UF2_BOARD_ID
#define VOLUME_LABEL UF2_VOLUME_LABEL

#endif
