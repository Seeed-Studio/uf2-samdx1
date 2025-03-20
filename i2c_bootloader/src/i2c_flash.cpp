// inspired by Arduino's TwoWire (Wire.h)

#include "i2c_flash.h"
#include <Arduino.h>
#include <wiring_private.h>
#include "Wire.h"
#include "crc.h"

#define ERASE_COMMAND 0xc7
#define CRC_COMMAND 0xca
#define JUMP_TO_APPL_COMMAND 0xce
#define GET_BLD_VERSION_COMMAND 0xcf

I2CFlash::I2CFlash(SERCOM *s, uint8_t pinSDA, uint8_t pinSCL) : sercom(s), _uc_pinSDA(pinSDA), _uc_pinSCL(pinSCL) {}

void I2CFlash::begin(uint8_t address, bool enableGeneralCall) {
  //Slave mode
  sercom->initSlaveWIRE(address, enableGeneralCall);
  sercom->enableWIRE();

  pinPeripheral(_uc_pinSDA, g_APinDescription[_uc_pinSDA].ulPinType);
  pinPeripheral(_uc_pinSCL, g_APinDescription[_uc_pinSCL].ulPinType);
}

bool I2CFlash::is_ready(void)
{
    return status == FLASH_READY;
}

void I2CFlash::write_stop(void)
{
    if(rx_buffer_index == 0) // received 0 bytes
    {
        return;
    }
    else if(rx_buffer[0] == ERASE_COMMAND)
    {
        status = FLASH_ERASE;
    }
    else if(rx_buffer[0] == CRC_COMMAND)
    {
        status = FLASH_CRC_CALC;
    }
    else if(rx_buffer[0] == JUMP_TO_APPL_COMMAND)
    {
        status = FLASH_JUMP_TO_APPL;
    }
    else if(rx_buffer[0] == GET_BLD_VERSION_COMMAND)
    {
        status = FLASH_GET_VERSION;
    }
    else if(is_flash_mem())
    {
        size_t addr = rx_buffer[0] << 24 | rx_buffer[1] << 16 | rx_buffer[2] << 8 | rx_buffer[3];
        if(addr+FLASH_START_ADDR < FLASH_SIZE)
        {
            mem_pointer = (uint8_t*)(FLASH_START_ADDR + addr);
            mem_end = (uint8_t*)FLASH_SIZE;
            flash_write_len = rx_buffer_index - ADDR_LEN;
            status = FLASH_WRITE;
        }
    }
}

bool I2CFlash::receive_byte(uint8_t byte)
{
    if(rx_buffer_index >= RX_BUFFER_LEN) // rx buffer overflow
    {
        return false;
    }
    
    if(rx_buffer_index == ADDR_LEN && is_flash_mem())
    {
        // finished receiving address, receiving first byte. Pad rx_buffer for 4 byte alignment
        size_t addr = rx_buffer[0] << 24 | rx_buffer[1] << 16 | rx_buffer[2] << 8 | rx_buffer[3];
        for(uint8_t i=0; i < addr % 4; i++) // pad buffer to align by 4 bytes
        {
            rx_buffer[rx_buffer_index++] = 0xff;
        }
    }
    rx_buffer[rx_buffer_index++] = sercom->readDataWIRE();
    
    return true;
}

void I2CFlash::send_byte(void)
{
    uint8_t c = 0xff;
    if(mem_pointer < mem_end)
    {
        c = *(uint8_t*)mem_pointer;
        mem_pointer++;
    }
    sercom->sendDataSlaveWIRE(c);
}

bool I2CFlash::is_flash_mem(void)
{
    // first addr bit == 0 => write to flash;
    return rx_buffer_index >= ADDR_LEN && (rx_buffer[0] & 0x80) == 0;
}

// inspired by TwoWire::onService()
void I2CFlash::i2c_flash_it_handler(void)
{
    if(sercom->isStopDetectedWIRE() || 
        (sercom->isAddressMatch() && sercom->isRestartDetectedWIRE() && !sercom->isMasterReadOperationWIRE())) //Stop or Restart detected
    {
        sercom->prepareAckBitWIRE();
        sercom->prepareCommandBitsWire(0x03);
        if(sercom->isMasterReadOperationWIRE())
        {
            mem_pointer--; // for n bytes read, send_byte() is called n+1 times, so we set flash_pointer back
        }
        else
        {
            write_stop();
            rx_buffer_index = 0;
        }
    }
    else if(sercom->isAddressMatch())  //Address Match
    {
        if(is_ready())
        {
            sercom->prepareAckBitWIRE();
        }
        else
        {
            sercom->prepareNackBitWIRE();
        }
        sercom->prepareCommandBitsWire(0x03);
    }
    else if(sercom->isDataReadyWIRE())
    {
      if (sercom->isMasterReadOperationWIRE()) // master is reading
      {
            send_byte();
      } else { //Received data
        uint8_t c = sercom->readDataWIRE();
        if(receive_byte(c))
        {
            sercom->prepareAckBitWIRE();
        }
        else
        {
            sercom->prepareNackBitWIRE();
        }
        sercom->prepareCommandBitsWire(0x03);
      }
    }
}

void I2CFlash::write_flash(void)
{
    uint32_t* dst = (uint32_t*)((uint32_t)mem_pointer & 0xfffffffc); // align 4 bytes
    uint32_t* src = (uint32_t*)(rx_buffer + ADDR_LEN);
    // apply padding to align with 4 bytes
    while (flash_write_len % 4 != 0 && flash_write_len < RX_BUFFER_LEN)
    {
        rx_buffer[ADDR_LEN+flash_write_len++] = 0xff;
    }
    
    uint32_t n_words = flash_write_len / 4;
    flash_write_words(dst, src, n_words);
    mem_pointer += flash_write_len;
    flash_write_len = 0;
    status = FLASH_READY;
}

void I2CFlash::prepare_crc(void)
{
    size_t addr = rx_buffer[1] << 24 | rx_buffer[2] << 16 | rx_buffer[3] << 8 | rx_buffer[4];
    size_t start_addr = addr + FLASH_START_ADDR;
    size_t size = rx_buffer[5] << 24 | rx_buffer[6] << 16 | rx_buffer[7] << 8 | rx_buffer[8];
    mem_pointer = tx_buffer;
    if(start_addr + size < FLASH_SIZE)
    {
        uint32_t crc = crc32((uint8_t*)start_addr, size);
        tx_buffer[0] = (uint8_t) ((crc >> 24) & 0x000000ff);
        tx_buffer[1] = (uint8_t) ((crc >> 16) & 0x000000ff);
        tx_buffer[2] = (uint8_t) ((crc >> 8) & 0x000000ff);
        tx_buffer[3] = (uint8_t) (crc & 0x000000ff);
        mem_end = tx_buffer+sizeof(crc);
    }
    else
    {
        mem_end = 0;
    }
    status = FLASH_READY;
}

void I2CFlash::write_version(const char* version)
{
    strncpy((char*)tx_buffer, version, TX_BUFFER_LEN);
    mem_pointer = tx_buffer;
    mem_end = tx_buffer + TX_BUFFER_LEN;
    status = FLASH_READY;
}

