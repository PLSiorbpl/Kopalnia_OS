#include "ata.h"

#include "ata_constants.h"
#include "arch/x86_64/Common/Common.hpp"
#include "kernel/Sleep.hpp"
#include "std/printf.hpp"

namespace drivers::ata {
    soft_reset_error device::soft_reset() {
        if (type == drive_type::PATA_PI)
            return soft_reset_error::pata_pi_dev;

        if (x64::inb(ALTERNATE_STATUS_REGISTER) == 0xFF) {
            return soft_reset_error::floating_bus;
        }

        x64::outb(DEVICE_CONTROL_REGISTER, 0x0 | SRST_BIT);

        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);

        x64::outb(DEVICE_CONTROL_REGISTER, 0x0);

        Time::Sleep(2);

        if (!check_command(false)) {
            return soft_reset_error::timeout;
        }

        return soft_reset_error::success;
    }

    device::device(const bool slave): type(drive_type::UNKNOWN) {
        switch (soft_reset()) {
            case soft_reset_error::floating_bus:
                std::kernel::printf("&4No ATA devices found!\n");
                return;
            case soft_reset_error::pata_pi_dev:
                std::kernel::printf("&4Unsupported PATAPI device!\n");
                return;
            case soft_reset_error::timeout:
                std::kernel::printf("&4Failed to reset ATA device! Command timed out.\n");
                return;
            case soft_reset_error::success:
                break;
        }

        x64::outb(DRIVE_HEAD_REGISTER, 0xA0 | (slave << 4));

        x64::inb(ALTERNATE_STATUS_REGISTER); // yes they are needed
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);
        x64::inb(ALTERNATE_STATUS_REGISTER);

        const auto cyl_lo = x64::inb(CYL_LOW_REGISTER);
        const auto cyl_hi = x64::inb(CYL_HI_REGISTER);

        if (cyl_lo == 0x14 && cyl_hi == 0xEB)
            type = drive_type::PATA_PI;
        if (cyl_lo == 0x69 && cyl_hi == 0x96)
            type = drive_type::SATA_PI;
        if (cyl_lo == 0 && cyl_hi == 0)
            type = drive_type::PATA;
        if (cyl_lo == 0x3c && cyl_hi == 0xc3)
            type = drive_type::SATA;

        x64::outb(SECTOR_COUNT_REGISTER, 0x0);
        x64::outb(SECTOR_NUMBER_REGISTER, 0x0);
        x64::outb(CYL_LOW_REGISTER, 0x0);
        x64::outb(CYL_HI_REGISTER, 0x0);

        if (type == drive_type::PATA_PI) {
            std::kernel::printf("&4Unsupported PATAPI device!\n");
            return;
        }

        x64::outb(COMMAND_REGISTER, IDENTIFY_COMMAND);

        if (!check_command(true)) {
            std::kernel::printf("&4Failed to fetch ATA device data!\n");
            return;
        }

        u16 data[256];
        for (unsigned short &i : data) {
            i = x64::inw(DATA_REGISTER);
        }

        char model[41];
        for (int i = 0; i < 20; i++) {
            model[i * 2] = static_cast<char>(data[27 + i] >> 8);
            model[i * 2 + 1] = static_cast<char>(data[27 + i] & 0xFF);
        }
        model[40] = '\0';

        char firmware[9];
        for (int i = 0; i < 4; i++) {
            firmware[i * 2] = static_cast<char>(data[23 + i] >> 8);
            firmware[i * 2 + 1] = static_cast<char>(data[23 + i] & 0xFF);
        }
        firmware[8] = '\0';

        std::kernel::printf("&aFirmware Version: %s, Model Number: %s\n", firmware, model);
    }

    bool device::check_command(bool drq) {
        constexpr int TIMEOUT = 40000000;
        for (int i = 0; i < TIMEOUT; ++i) {
            const auto status = x64::inb(ALTERNATE_STATUS_REGISTER);
            if (!(status & BSY_BIT)){
                if ((status & ERR_BIT) || (status & DF_BIT)) {
                    return false;
                }
                if ((!drq || (status & DRQ_BIT))) {
                    return true;
                }
            }

            x64::inb(ALTERNATE_STATUS_REGISTER); // delay
        }

        return false;
    }
}
