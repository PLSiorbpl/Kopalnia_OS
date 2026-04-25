#include "ahci.h"

#include "ahci_helper.h"
#include "../PCI.hpp"
#include "../../kernel/Paging.hpp"
#include "../../libs/std/printf.hpp"
#include "kernel/Memory/heap.hpp"
#include "std/mem_common.hpp"

drivers::ahci::ahci::ahci() {

}

drivers::ahci::ahci::~ahci() {

}

void drivers::ahci::ahci::init() {
    if (hba != nullptr) {
        std::kernel::printf("&4ACHI was already initialized.");
        return;
    }

    const auto device = PCI::find_class_with_sub(0x01, 0x06);

    const auto name = [&]() -> const char* {
        switch (device.vendor_id) {
            case 0x8086: return "Intel";
            case 0x1022: return "AMD";
            case 0x10DE: return "NVIDIA";
            case 0x1B4B: return "Marvell";
            case 0x1039: return "SiS";
            case 0x1106: return "VIA";
            default: return "Unknown";
        }
    }();

    std::kernel::printf("AHCI controller: %s (vendor: %x, device: %x)\n\n", name, device.vendor_id, device.device_id);

    hba = reinterpret_cast<volatile hba_memory*>(static_cast<u64>(device.bar[5] & 0xFFFFFFF0));
    Paging::Map_memory(reinterpret_cast<u64>(hba), reinterpret_cast<u64>(hba) + sizeof(hba_memory), Paging::Profile::MMIO);

    hba->ghc |= AHCI_ENABLE_BIT;
    while (!(hba->ghc & AHCI_ENABLE_BIT)) {}

    probe_ports();
}

void drivers::ahci::ahci::probe_ports() {
    const u32 ports_implemented = hba->pi;
    std::kernel::printf("ports: %i\n\n", ports_implemented);

    for (int i = 0; i < 32; ++i) {
        if (ports_implemented & (1 << i)) {
            auto port_type = get_port_type(&hba->ports[i]);
            if (port_type == port_type::sata || port_type == port_type::satapi) {
                std::kernel::printf("&aFound %s on port %d\n", port_type == port_type::sata ? "SATA" : "SATAPI", i);

                ports[i].initialize(port_type, &hba->ports[i], i, hba);
                ports[i].configure();
            }
        }
    }
    std::kernel::printf("\n");
}
