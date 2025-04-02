#!/usr/bin/env python3
import argparse
import logging
import os
import struct
import sys

import time
import zlib
from periphery import I2C

class I2CFlasher:
    STRUCT_ADDR = struct.Struct(">I")
    STRUCT_CRC_REQ = struct.Struct(">II")
    STRUCT_CRC_RESP = struct.Struct(">I")
    CMD_CRC = b'\xca'
    CMD_ERASE_CHIP = b'\xc7'
    CMD_START_APPLICATION = b'\xce'
    CMD_GET_BLD_VERSION = b'\xcf'

    RETRY_MAX = 3

    def __init__(self, i2c_dev, device_addr = 0x30):
        self.i2c = I2C(i2c_dev)
        self.device_addr = device_addr

    @staticmethod
    def crc32_file(filename: str, chunk_size=4096) -> int:
        crc = 0
        with open(filename, "rb") as f:
            for chunk in iter(lambda: f.read(chunk_size), b""):
                crc = zlib.crc32(chunk, crc)
        return crc & 0xFFFFFFFF  # Ensure it's a 32-bit unsigned value

    def write_file(self, filename: str) -> None:
        with open(filename, "rb") as f:
            index = 0
            chunk = b'1'
            while chunk:
                chunk = f.read(128)
                addr = self.STRUCT_ADDR.pack(index)
                msg = I2C.Message(addr + chunk)
                self.i2c.transfer(self.device_addr, [msg])
                index += len(chunk)
                time.sleep(0.01)

    def read_crc(self, size: int, start_addr: int = 0x0) -> int:
        req = I2C.Message(self.CMD_CRC + self.STRUCT_CRC_REQ.pack(start_addr, size))
        self.i2c.transfer(self.device_addr, [req])
        time.sleep(1)
        resp = [I2C.Message(bytes(self.STRUCT_CRC_RESP.size), read=True)]
        self.i2c.transfer(self.device_addr, resp)
        return self.STRUCT_CRC_RESP.unpack(resp[0].data)[0]

    def erase_chip(self):
        self.i2c.transfer(self.device_addr, [I2C.Message(self.CMD_ERASE_CHIP)])

    def flash(self, filename: str) -> None:
        logging.debug('erasing chip')
        self.erase_chip()
        logging.debug('chip erased')
        time.sleep(1)
        self.write_file(filename)

    def start_application(self):
        req = I2C.Message(self.CMD_START_APPLICATION)
        self.i2c.transfer(self.device_addr, [req])

    def read_bld_version(self) -> str:
        req = I2C.Message(self.CMD_GET_BLD_VERSION)
        self.i2c.transfer(self.device_addr, [req])
        time.sleep(0.01)
        resp = I2C.Message(bytes(0x40), read=True)
        self.i2c.transfer(self.device_addr, [resp])
        return resp.data.split(b'\x00', 1)[0].decode('utf-8')

    def flash_and_start(self, filename: str) -> None:
        crc_calc = self.crc32_file(filename)
        file_size = os.path.getsize(filename)
        logging.info(self.read_bld_version())
        for i in range(0, self.RETRY_MAX):
            crc_read = self.read_crc(file_size)
            if  crc_calc == crc_read:
                self.start_application()
                logging.info('starting application')
                break
            logging.info(f'flashing file {filename}')
            time.sleep(1)
            self.flash(filename)
            time.sleep(1)
        else:
            raise ValueError(f'Could not flash file {filename}')

if __name__ == "__main__":
    parser = argparse.ArgumentParser(prog=__name__, description='Tool to upgrade I2C micro controllers')
    parser.add_argument('firmware_path', help='Path to firmware file or directory')
    parser.add_argument('--verbose', '-v', action='count', default=0)
    parser.add_argument('-d', '--device', type=str, default='/dev/i2c-' + os.environ.get('I2C_BUS_BATTERY', '1'),
                        help='I2C device to use (default: %(default)s)')
    args = parser.parse_args()

    if args.verbose == 1:
        log_level = logging.INFO
    elif args.verbose == 2:
        log_level = logging.DEBUG
    else:
        log_level = logging.WARNING
    logging.basicConfig(level=log_level, format='%(levelname)s: %(message)s')

    flasher = I2CFlasher(args.device)
    flasher.flash_and_start(args.firmware_path)
