#ifndef i2c_flash_h
#define i2c_flash_h

#include "Stream.h"
#include "variant.h"

#include "Wire.h"
#include "SERCOM.h"
#include "flash.h"

#define ADDR_LEN 4
#define RX_BUFFER_PADDING_LEN 4
#define RX_BUFFER_LEN (ADDR_LEN + FLASH_ROW_SIZE + RX_BUFFER_PADDING_LEN)
#define TX_BUFFER_LEN 0x40
#define FLASH_START_ADDR 0x2000U

enum I2CFlashStatus
{
  FLASH_READY,
  FLASH_ERASE,
  FLASH_WRITE,
  FLASH_CRC_CALC,
  FLASH_JUMP_TO_APPL,
  FLASH_GET_VERSION,
};

class I2CFlash
{
  public:
    I2CFlash(SERCOM *s, uint8_t pinSDA, uint8_t pinSCL);
    void begin(uint8_t address, bool enableGeneralCall = false);
    void i2c_flash_it_handler(void);
    void write_flash(void);
    void prepare_crc(void);
    void write_version(const char* version);
    
    volatile uint16_t flash_write_len;
    volatile I2CFlashStatus status;
    volatile uint8_t* mem_pointer = (uint8_t*)FLASH_START_ADDR;
    volatile uint8_t* mem_end = (uint8_t*)FLASH_SIZE;

    
  private:
    bool is_ready(void);
    void write_stop(void);
    bool receive_byte(uint8_t byte);
    void send_byte(void);
    bool is_flash_mem(void);

    SERCOM* sercom;
    uint8_t _uc_pinSDA;
    uint8_t _uc_pinSCL;

    volatile uint16_t rx_buffer_index;
    alignas(4) volatile uint8_t rx_buffer[RX_BUFFER_LEN];
    alignas(4) volatile uint8_t tx_buffer[TX_BUFFER_LEN];
    
};
#endif // i2c_flash_h