#include "ahci_port.h"

#include "ahci.h"
#include "ahci_helper.h"
#include "kernel/Paging.hpp"
#include "kernel/Memory/heap.hpp"
#include "std/mem_common.hpp"
#include "std/printf.hpp"

namespace drivers::ahci {
    i8 ahci_port::get_command_slot() const {
        const u32 slots = port->sact | port->command_issue;
        for (u8 i = 0; i < command_slots; ++i) {
            if ((slots & (1 << i)) == 0)
                return i;
        }

        std::kernel::printf("&4Failed to find free command slot.\n");
        return -1;
    }

    bool ahci_port::wait_for_port() const {
        for (int i = 0; i < 1000000; ++i) {
            if ((port->tfd & (ATA_DEV_BUSY_BIT | ATA_DEV_DRQ_BIT)) == 0) {
                return true;
            }
        }

        std::kernel::printf("&4Port is hung.\n");
        return false;
    }

    bool ahci_port::wait_for_port_completion(const u8 slot) {
        for (int i = 0; i < 1000000; ++i) {
            if (has_errored) {
                std::kernel::printf("&4Command failed.\n");
                has_errored = false;
                return false;
            }
            if ((port->command_issue & (1 << slot)) == 0)
                return true;
        }
        std::kernel::printf("&4Command timed out.\n");
        has_errored = false;
        return false;
    }

    void ahci_port::configure(const port_type type, volatile hba_port* port, u8 port_num, const volatile hba_memory* hba) {
        this->type = type;
        this->port_num = port_num;
        this->port = port;
        this->command_slots = hba->capabilities.num_command_slots;
        active = true;

        stop();

        buffer = allocate_virtual_memory(PORT_BUFFER_SIZE, 2);

        // Command List
        command_list = static_cast<command_header*>(allocate_virtual_memory(sizeof(command_header) * 32, 1024));
        port->command_list_base = static_cast<u32>(reinterpret_cast<u64>(command_list));
        port->command_list_base_upper = static_cast<u32>(reinterpret_cast<u64>(command_list) >> 32);

        // Received FIS
        received = static_cast<received_fis*>(allocate_virtual_memory(sizeof(received_fis), 256));
        port->fis_base_address = static_cast<u32>(reinterpret_cast<u64>(received));
        port->fis_base_address_upper = static_cast<u32>(reinterpret_cast<u64>(received) >> 32);

        for (int i = 0; i < 32; ++i) {
            command_list[i].prd_table_length = 0;

            cmd_tables[i] = static_cast<command_table*>(allocate_virtual_memory(sizeof(command_table), 128));
            command_list[i].cmd_table_base_address = static_cast<u32>(reinterpret_cast<u64>(cmd_tables[i]));
            command_list[i].cmd_table_base_address_upper = static_cast<u32>(reinterpret_cast<u64>(cmd_tables[i]) >> 32);
        }

        start();

        // Configure which interrupts to enable
        port->interrupts_enabled.cold_port_detect_interrupt = true;
        port->interrupts_enabled.task_file_error_interrupt = true;
        port->interrupts_enabled.host_bus_fatal_error_interrupt = true;
        port->interrupts_enabled.host_bus_data_error_interrupt = true;
        port->interrupts_enabled.interface_fatal_error_interrupt = true;
        port->interrupts_enabled.overflow_interrupt = true;
        port->interrupts_enabled.incorrect_port_multiplier_interrupt = true;
        port->interrupts_enabled.unknown_fis_interrupt = true;
        port->interrupts_enabled.port_connect_change_interrupt = true;
    }

    void ahci_port::debug_print_identify_info() {
        if (!identify()) {
            std::kernel::printf("&4Port %d: identify failed!\n", port_num);
        }
        const auto* data = static_cast<u16*>(buffer);

        char model[41];
        for (int j = 0; j < 20; j++) {
            model[j * 2] = static_cast<char>(data[27 + j] >> 8);
            model[j * 2 + 1] = static_cast<char>(data[27 + j] & 0xFF);
        }
        model[40] = '\0';

        char firmware[9];
        for (int i = 0; i < 4; i++) {
            firmware[i * 2] = static_cast<char>(data[23 + i] >> 8);
            firmware[i * 2 + 1] = static_cast<char>(data[23 + i] & 0xFF);
        }
        firmware[8] = '\0';

        std::kernel::printf("&a\tModel: %s\n"
                                "\tFirmware Version: %s\n\n",
                                model, firmware);
    }

    void ahci_port::start() const {
        while (port->command_status & CMD_CR_BIT) {}
        port->command_status |= CMD_FRE_BIT | CMD_ST_BIT;
    }

    void ahci_port::stop() const {
        port->command_status &= ~(CMD_FRE_BIT | CMD_ST_BIT);
        while ((port->command_status & CMD_FR_BIT) || (port->command_status & CMD_CR_BIT)) {}
    }

    bool ahci_port::identify() {
        // clear interrupts & errors
        has_errored = false;
        *reinterpret_cast<volatile u32*>(&port->interrupt_status) = 0xFFFFFFFF;

        const auto slot = get_command_slot();
        if (slot == -1) {
            return false;
        }

        auto& header = command_list[slot];
        header.fis_length = 5;
        header.write = 0;
        header.prd_table_length = 1;
        header.prefetchable = 1;
        header.clear = 1;

        const auto table = cmd_tables[slot];
        mem::memset(table, 0, sizeof(command_table));
        table->prdt[0].data_base_address = static_cast<u32>(reinterpret_cast<u64>(buffer));
        table->prdt[0].data_base_address_upper = static_cast<u32>(reinterpret_cast<u64>(buffer) >> 32);
        table->prdt[0].data_byte_count = 512 - 1;
        table->prdt[0].interrupt_on_complete = 1;

        const auto command_fis = reinterpret_cast<fis::reg_h2d*>(table->command_fis);
        mem::memset(command_fis, 0, sizeof(fis::reg_h2d));
        command_fis->fis_type = static_cast<u8>(fis::type::FIS_TYPE_REG_H2D);
        command_fis->command_control = 1;
        command_fis->command = ATA_CMD_IDENTIFY;

        return issue_command(slot);
    }

    void ahci_port::on_interrupt() {
        if (port->interrupt_status.cold_port_detect_interrupt)
            std::kernel::printf("&4AHCI: Device on port %i has been removed or unable to be detected!\n", port_num);
        if (port->interrupt_status.task_file_error_interrupt) {
            std::kernel::printf("&4AHCI: task file error (tfd: %x)\n", port->tfd);
            has_errored = true;
        }
        if (port->interrupt_status.host_bus_fatal_error_interrupt) {
            std::kernel::printf("&4AHCI: host bus fatal error\n");
            has_errored = true;
        }
        if (port->interrupt_status.host_bus_data_error_interrupt) {
            std::kernel::printf("&4AHCI: host bus data error\n");
            has_errored = true;
        }
        if (port->interrupt_status.interface_fatal_error_interrupt) {
            std::kernel::printf("&4AHCI: interface fatal error (serr: %x)\n", port->serr);
            has_errored = true;
        }
        if (port->interrupt_status.interface_non_fatal_error_interrupt)
            std::kernel::printf("&4AHCI: interface non-fatal error (serr: %x)\n", port->serr);
        if (port->interrupt_status.overflow_interrupt)
            std::kernel::printf("&4AHCI: overflow error\n");
        if (port->interrupt_status.incorrect_port_multiplier_interrupt)
            std::kernel::printf("&4AHCI: incorrect port multiplier\n");
        if (port->interrupt_status.phy_ready_change_interrupt)
            std::kernel::printf("&4AHCI: PHY ready change\n");
        if (port->interrupt_status.port_connect_change_interrupt)
            std::kernel::printf("&4AHCI: device connected/disconnected on port %i\n", port_num);

        *reinterpret_cast<volatile u32*>(&port->interrupt_status) = 0xFFFFFFFF;
    }

    bool ahci_port::issue_command(const u8 slot) {
        if (!wait_for_port())
            return false;
        port->command_issue = 1 << slot;
        if (!wait_for_port_completion(slot))
            return false;

        return true;
    }

    bool ahci_port::is_active() const {
        return active;
    }
}
