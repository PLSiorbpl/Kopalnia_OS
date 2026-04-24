#pragma once
#include "../../libs/std/types.hpp"

namespace drivers::ata {
    constexpr u16 IO_REGISTERS_BASE = 0x1F0;


    // IO REGISTERS
    constexpr u16 DATA_REGISTER = IO_REGISTERS_BASE + 0;           // W/R
    constexpr u16 ERROR_REGISTER = IO_REGISTERS_BASE + 1;          // R
    constexpr u16 FEATURES_REGISTER = IO_REGISTERS_BASE + 1;       // W
    constexpr u16 SECTOR_COUNT_REGISTER = IO_REGISTERS_BASE + 2;   // W/R
    constexpr u16 SECTOR_NUMBER_REGISTER = IO_REGISTERS_BASE + 3;  // W/R
    constexpr u16 CYL_LOW_REGISTER = IO_REGISTERS_BASE + 4;        // W/R
    constexpr u16 CYL_HI_REGISTER = IO_REGISTERS_BASE + 5;         // W/R
    constexpr u16 DRIVE_HEAD_REGISTER = IO_REGISTERS_BASE + 6;     // W/R
    constexpr u16 STATUS_REGISTER = IO_REGISTERS_BASE + 7;         // R
    constexpr u16 COMMAND_REGISTER = IO_REGISTERS_BASE + 7;        // W

    // CONTROL REGISTERS
    constexpr u16 CONTROL_REGISTERS_BASE = 0x3F6;

    constexpr u16 ALTERNATE_STATUS_REGISTER = CONTROL_REGISTERS_BASE + 0; // R (status reg that doesnt affect interrupts)
    constexpr u16 DEVICE_CONTROL_REGISTER = CONTROL_REGISTERS_BASE + 0; // W
    constexpr u16 DRIVE_ADDR_REGISTER = CONTROL_REGISTERS_BASE + 1; // R

    // Commands
    constexpr u8 IDENTIFY_COMMAND = 0xEC;
    constexpr u8 IDENTIFY_PACKET_DEV_COMMAND = 0xA1;

    // Bit masks
    constexpr u8 SRST_BIT = (1 << 2);
    constexpr u8 BSY_BIT = (1 << 7);
    constexpr u8 ERR_BIT = (1 << 0);
    constexpr u8 DRQ_BIT = (1 << 3);
    constexpr u8 DF_BIT = (1 << 5);

    constexpr u8 ERR_ABRT_BIT = (1 << 2);
}