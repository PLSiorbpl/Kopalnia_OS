#pragma once
#include "../../libs/std/types.hpp"

namespace drivers::ahci::fis {
    enum class type : u8
    {
        FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
        FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
        FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
        FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
        FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
        FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
        FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
        FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
    };

    struct reg_h2d {
        u8 fis_type;

        u8 port_mult_port : 4;
        u8 reserved : 3;
        u8 command_control : 1; // 1: command, 0: control

        u8 command;
        u8 feature_lo;

        u8 lba0; // LBA low register, 7:0
        u8 lba1; // LBA mid-register, 15:8
        u8 lba2; // LBA high register, 23:16
        u8 device;

        u8 lba3; // LBA register, 31:24
        u8 lba4; // LBA register, 39:32
        u8 lba5; // LBA register, 47:40
        u8 feature_hi;

        u8 count_lo;
        u8 count_hi;
        u8 icc; // Isochronous command completion
        u8 control;

        u8 reserved1[4];
    };

    struct reg_d2h {
        u8 fis_type;

        u8 port_mult_port : 4;
        u8 reserved : 2;
        u8 interrupt : 1; // Interrupt bit
        u8 reserved1 : 1;

        u8 status;
        u8 error;

        u8 lba0; // LBA low register, 7:0
        u8 lba1; // LBA mid-register, 15:8
        u8 lba2; // LBA high register, 23:16
        u8 device;

        u8 lba3; // LBA register, 31:24
        u8 lba4; // LBA register, 39:32
        u8 lba5; // LBA register, 47:40
        u8 reserved2;

        u8 count_lo;
        u8 count_hi;
        u8 reserved3[2];

        u8 reserved4[4];
    };

    struct fis_data {
        u8 fis_type;

        u8 port_mult_port : 4;
        u8 reserved : 4;

        u8 reserved1[2];

        u32 data[1];
    };

    struct pio_setup {
        u8 fis_type;

        u8 port_mult_port : 4;
        u8 reserved : 1;
        u8 data_direction : 1; // Data transfer direction, 1 - device to host
        u8 interrupt : 1; // Interrupt bit
        u8 reserved1 : 1;

        u8 status;
        u8 error;

        u8 lba0;
        u8 lba1;
        u8 lba2;
        u8 device;

        u8 lba3;
        u8 lba4;
        u8 lba5;
        u8 reserved2;

        u8 count_lo;
        u8 count_hi;
        u8 reserved3;
        u8 e_status; // new value of status reg

        u16 transfer_count;
        u8 reserved4[2];
    };

    struct dma_setup {
        u8 fis_type;

        u8 port_mult_port : 4;
        u8 reserved : 1;
        u8 data_direction : 1;
        u8 interrupt : 1;
        u8 a : 1; // Auto-activate. Specifies if DMA Activate FIS is needed

        u8 reserved1[2];

        u64 dma_buffer_id; // DMA Buffer Identifier. Used to Identify DMA buffer in host memory.
        // SATA Spec says host specific and not in Spec. Trying AHCI spec might work.

        u32 reserved2;

        u32 dma_buffer_offset; // Byte offset into buffer. First 2 bits must be 0

        u32 transfer_count; // num bytes to transfer. bit 0 must be 0

        u32 reserved3;
    };


}
