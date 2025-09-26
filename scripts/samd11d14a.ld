/* SAMD11 8KB UF2 bootloader linker script */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x2000 /* 8KB bootloader */
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 0x1000 /* 4KB SRAM */
}

_estack = ORIGIN(RAM) + LENGTH(RAM);

/* Sections */
SECTIONS
{
  .text :
  {
    KEEP(*(.vectors*))
    *(.text*)
    *(.rodata*)
    *(.glib*)
    KEEP(*(.init))
    KEEP(*(.fini))
    . = ALIGN(4);
    _etext = .;
  } > FLASH

  .data : AT ( _etext )
  {
    _srelocate = .;
    *(.data*)
    . = ALIGN(4);
    _erelocate = .;
  } > RAM

  .bss (NOLOAD) :
  {
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    _end = .;
  } > RAM
}
