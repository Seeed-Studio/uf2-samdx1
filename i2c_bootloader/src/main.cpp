#include <Wire.h>
#include <Arduino.h>
#include <flash.h>
#include <i2c_flash.h>

#define I2C_SLAVE_ADDRESS 0x30
#define BLINK_HALF_PERIOD_MS 500

#define str(s) #s
#define xstr(s) str(s)

#ifndef BLD_VERSION
#define BLD_VERSION dev
#endif

#define START_APPLICATION_BYTE_PTR ((volatile uint32_t *)(HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4))
#define START_APPLICATION_MAGIC 0xf02669ef
#define BOOT_PROT 0x2


static I2CFlash g_i2c_flash(&PERIPH_WIRE, PIN_WIRE_SDA, PIN_WIRE_SCL);
static bool is_jump_application_set(void);
static void jump_to_application(void);
static void reboot_to_application(void);
static void lock_bootloader_section(void);


void SERCOM0_Handler()
{
  g_i2c_flash.i2c_flash_it_handler();
}

void setup() {
  if(is_jump_application_set())
  {
    *START_APPLICATION_BYTE_PTR = 0xff;
    jump_to_application();
  }
  pinMode(PIN_LED, OUTPUT);
  lock_bootloader_section();
  g_i2c_flash.begin(I2C_SLAVE_ADDRESS);
}

static void led_toggle()
{
  static bool led_state;
  led_state = !led_state;
  digitalWrite(PIN_LED, led_state);
}

static void blink_loop(){
  static uint32_t last_toggle_time;
  
  if(millis() - last_toggle_time > BLINK_HALF_PERIOD_MS)
  {
    led_toggle();
    last_toggle_time = millis();
  }
}

static bool is_jump_application_set(void)
{
  return (*START_APPLICATION_BYTE_PTR == START_APPLICATION_MAGIC);
}

static void jump_to_application(void)
{
    uint32_t app_reset_handler_address = *(uint32_t *)(FLASH_START_ADDR + 4);
    uint32_t main_stack_pointer_address = *(uint32_t *)FLASH_START_ADDR;
    uint32_t vector_table_address = ((uint32_t)FLASH_START_ADDR & SCB_VTOR_TBLOFF_Msk);

    /* Rebase the Stack Pointer */
    __set_MSP(main_stack_pointer_address);

    /* Rebase the vector table base address */
    SCB->VTOR = vector_table_address;

    /* Jump to application Reset Handler in the application */
    asm("bx %0" ::"r"(app_reset_handler_address));
}

static void reboot_to_application(void)
{
  *START_APPLICATION_BYTE_PTR = START_APPLICATION_MAGIC;
  NVIC_SystemReset();
}

void loop() {
  switch(g_i2c_flash.status)
  {
    case FLASH_ERASE:
      flash_erase_to_end((uint32_t*)FLASH_START_ADDR);
      g_i2c_flash.status = FLASH_READY;
      break;
    case FLASH_WRITE:
      g_i2c_flash.write_flash();
      led_toggle(); // blink during flash
      break;
    case FLASH_CRC_CALC:
      g_i2c_flash.prepare_crc();
      break;
    case FLASH_JUMP_TO_APPL:
      reboot_to_application();
      break;
    case FLASH_GET_VERSION:
      g_i2c_flash.write_version(xstr(BLD_VERSION));
      break;
    case FLASH_READY:
      break;
  }
  blink_loop();
}


#if FLASH_START_ADDR != 0x2000
#warning "BOOTPROT is not adapted for current bootloader size"
#endif
static void lock_bootloader_section(void)
{
  // persistently set bootloader section to 0x2000 to write protect
  uint32_t user_row = *(uint32_t*)NVMCTRL_USER;
  if((user_row & 0x00000007) != BOOT_PROT)
  {
    user_row = (user_row & 0xfffffff8) | BOOT_PROT;
    flash_erase_nvm_user_config();
    flash_write_nvm_user_config(user_row);
  }
}
