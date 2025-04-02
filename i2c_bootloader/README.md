# Custom I2C MCU Bootloader

This custom I2C bootloader for SAMD21 devices allows the MCU to be programmed over I2C.
The bootloader protects itself from being overwritten using the fuse bits (`BOOT_PROT`).
The bootloader does only jump to the application when receiving the corresponding I2C command.
This reduces the risk of bricking a device.

The included python scripts compares the CRC of the provided firmware file with the CRC calculated on the MCU. If they are identical, the MCU is started. If they differ, the binary is flashed onto the MCU before starting the application.

## Protocol

The protocol uses 32bit address and all values are in big endian.

### Write

A write is performed by sending the 4 byte big endian destination address (starting from application flash space) followed by the bytes to write. The maximal number of data byte is 255.
After the write operation, the memory pointer is incremented by the number of data bytes.
```
ADDR_HH ADDR_H ADDR_L ADDR_LL DATA0 DATA1 ... DATAN
```

N.B.: The write command does not erase the memory. Use the erase command to erase the memory before writing.

### Read

Reading returns directly bytes from the application flash space at the current memory pointer address, if none of the commands below has been executed.
To read at a specific address, issue a write command with the 4 bytes big endian address (starting from application flash space) before reading.

### Jump to Application `0xCE`
Sending this command resets the MCU and it jumps to the application code. No tests are performed before jumping to application.

### Erase Chip `0xC7`
Sending this command erase the entire memory, except the bootloader section.

### CRC request `0xCA`
Sending this command including the start address and the number of bytes triggers the calculation of the CRC of the application firmware.
```
0xCA START_ADDR_HH START_ADDR_H START_ADDR_L START_ADDR_LL COUNT_HH COUNT_H COUNT_L COUNT_LL
```
A subsequent read returns the CRC32 checksum in big endian:
```
CRC_HH CRC_H CRC_L CRC_LL
```

### GET_BLD_VERSION `0xCF`
Sending this command loads the bootloader's version as C-style string into the tx buffer.
A Subsequent read returns the bootloader's version. The maximal length is 40 bytes.

## Write Protection
The bootloader automatically sets write protection for its own region.
Therefore, once flashed, the chip cannot be erased anymore.
To deactivate write protection, write 0xff to 0x804000.

With `JLinkExe`, use the following command:
```shell
write1 804000 ff
```
