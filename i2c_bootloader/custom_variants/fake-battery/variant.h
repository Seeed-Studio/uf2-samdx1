/*
 * modified from .platformio/packages/framework-arduino-samd-seeed/variants/XIAO_m0/variant.h
*/

#pragma once

// The definitions here needs a SAMD core >=1.6.10
#define ARDUINO_SAMD_VARIANT_COMPLIANCE 10610


#include <WVariant.h>

// General definitions
// -------------------

// Frequency of the board main oscillator
#define VARIANT_MAINOSC (32768ul)

// Master clock frequency
#define VARIANT_MCK     (F_CPU)

// Pins
// ----

// Number of pins defined in PinDescription array
#define PINS_COUNT           (17u)
#define NUM_DIGITAL_PINS     (17u)
#define NUM_ANALOG_INPUTS    (11u)
#define NUM_ANALOG_OUTPUTS   (1u)

// Low-level pin register query macros
// -----------------------------------
#define digitalPinToPort(P)      (&(PORT->Group[g_APinDescription[P].ulPort]))
#define digitalPinToBitMask(P)   (1 << g_APinDescription[P].ulPin)
//#define analogInPinToBit(P)    ()
#define portOutputRegister(port) (&(port->OUT.reg))
#define portInputRegister(port)  (&(port->IN.reg))
#define portModeRegister(port)   (&(port->DIR.reg))
#define digitalPinHasPWM(P)      (g_APinDescription[P].ulPWMChannel != NOT_ON_PWM || g_APinDescription[P].ulTCChannel != NOT_ON_TIMER)

/*
 * digitalPinToTimer(..) is AVR-specific and is not defined for SAMD
 * architecture. If you need to check if a pin supports PWM you must
 * use digitalPinHasPWM(..).
 *
 * https://github.com/arduino/Arduino/issues/1833
 */
// #define digitalPinToTimer(P)


// LEDs
// ----
#define PIN_LED_13  (13u)
#define PIN_LED     PIN_LED_13
#define LED_BUILTIN PIN_LED


#define PIN_LED_RXL          (12u)
#define PIN_LED_TXL          (11u)
#define PIN_LED2             PIN_LED_RXL
#define PIN_LED3             PIN_LED_TXL

/*
 * Analog pins
 */
#define PIN_A0               (0ul)
#define PIN_A1               (PIN_A0 + 1)
#define PIN_A2               (PIN_A0 + 2)
#define PIN_A3               (PIN_A0 + 3)
#define PIN_A4               (PIN_A0 + 4)
#define PIN_A5               (PIN_A0 + 5)
#define PIN_A6               (PIN_A0 + 6)
#define PIN_A7               (PIN_A0 + 7)
#define PIN_A8               (PIN_A0 + 8)
#define PIN_A9               (PIN_A0 + 9)
#define PIN_A10              (PIN_A0 + 10)

#define PIN_DAC0             (PIN_A0)


static const uint8_t A0  = PIN_A0;
static const uint8_t A1  = PIN_A1;
static const uint8_t A2  = PIN_A2;
static const uint8_t A3  = PIN_A3;
static const uint8_t A4  = PIN_A4;
static const uint8_t A5  = PIN_A5;
static const uint8_t A6  = PIN_A6 ;
static const uint8_t A7  = PIN_A7 ;
static const uint8_t A8  = PIN_A8 ;
static const uint8_t A9  = PIN_A9 ;
static const uint8_t A10  = PIN_A10 ;
static const uint8_t DAC0 = PIN_DAC0;

#define ADC_RESOLUTION		12

#define D0  (0u)
#define D1  (1u)
#define D2  (2u)
#define D3  (3u)
#define D4  (4u)
#define D5  (5u)
#define D6  (6u)
#define D7  (7u)
#define D8  (8u)
#define D9  (9u)
#define D10 (10u)

/*
 * SPI Interfaces
 */
#define SPI_INTERFACES_COUNT 0

/*
 * RTL8720D Interfaces
*/
#define RTL8720D_CHIP_PU        (1ul)

/*
 * Wire Interfaces
 */
#define WIRE_INTERFACES_COUNT 1

  // "external" public i2c interface
#define PIN_WIRE_SDA         (4u)
#define PIN_WIRE_SCL         (5u)
#define PERIPH_WIRE          sercom0
// #define WIRE_IT_HANDLER      SERCOM0_Handler
#define WIRE_IT_HANDLER      DummyIT_Handler
static const uint8_t SDA = PIN_WIRE_SDA;
static const uint8_t SCL = PIN_WIRE_SCL;

// USB
// No USB, but here for compatibility
#define USB_VID 0x0000
#define USB_PID 0x0000
#define PIN_USB_HOST_ENABLE (14ul)
#define PIN_USB_DM          (15ul)
#define PIN_USB_DP          (16ul)


// I2S Interfaces
// --------------
#define I2S_INTERFACES_COUNT 0

// Serial ports
// ------------
#ifdef __cplusplus
#include "SERCOM.h"

// Instances of SERCOM
extern SERCOM sercom0;
extern SERCOM sercom1;
extern SERCOM sercom2;
extern SERCOM sercom3;

#endif // __cplusplus

