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

    bool ahci_port::wait_for_port_completion(const u8 slot) const {
        for (int i = 0; i < 1000000; ++i) {
            if (port->is & PXIS_TFES) {
                std::kernel::printf("&4Command failed.\n");
                return false;
            }
            if ((port->command_issue & (1 << slot)) == 0)
                return true;
        }
        std::kernel::printf("&4Command timed out.\n");
        return false;
    }

    void ahci_port::initialize(const port_type type, volatile hba_port* port, u8 port_num, const volatile hba_memory* hba) {
        this->type = type;
        this->port_num = port_num;
        this->port = port;
        this->command_slots = ((hba->capability >> 8) & 0x1F) + 1;
        active = true;
    }

    void ahci_port::configure() {
        if (!active)
            return;

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
            command_list[i].prd_table_length = 8;

            cmd_tables[i] = static_cast<command_table*>(allocate_virtual_memory(sizeof(command_table), 128));
            command_list[i].cmd_table_base_address = static_cast<u32>(reinterpret_cast<u64>(cmd_tables[i]));
            command_list[i].cmd_table_base_address_upper = static_cast<u32>(reinterpret_cast<u64>(cmd_tables[i]) >> 32);
        }

        start();

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
                                "\tFirmware Version: %s\n",
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
        port->is = static_cast<u32>(-1);
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

        auto command_fis = reinterpret_cast<fis::reg_h2d*>(table->command_fis);
        mem::memset(command_fis, 0, sizeof(fis::reg_h2d));
        command_fis->fis_type = static_cast<u8>(fis::type::FIS_TYPE_REG_H2D);
        command_fis->command_control = 1;
        command_fis->command = ATA_CMD_IDENTIFY;

        return issue_command(slot);
    }

    bool ahci_port::issue_command(const u8 slot) const {
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
