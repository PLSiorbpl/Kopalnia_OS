#include "ahci_device.h"

bool drivers::ahci::ahci_device::initialize() {
    auto buffer = static_cast<u16*>(allocate_virtual_memory(PORT_BUFFER_SIZE, 4));
    if (!port->identify(buffer)) {
        return false;
    }

    Time::Sleep(100);

    for (int j = 0; j < 20; j++) {
        model[j * 2] = static_cast<char>(buffer[27 + j] >> 8);
        model[j * 2 + 1] = static_cast<char>(buffer[27 + j] & 0xFF);
    }
    model[40] = '\0';

    for (int i = 0; i < 4; i++) {
        firmware[i * 2] = static_cast<char>(buffer[23 + i] >> 8);
        firmware[i * 2 + 1] = static_cast<char>(buffer[23 + i] & 0xFF);
    }
    firmware[8] = '\0';

    const bool large_sectors = buffer[106] & (1 << 12);
    sector_size = large_sectors ? ((static_cast<u32>(buffer[118]) << 16) | buffer[117]) * 2 : 512;

    sector_count = (static_cast<u64>(buffer[103]) << 48) | (static_cast<u64>(buffer[102]) << 32) | (static_cast<u64>(buffer[101]) << 16) | static_cast<u64>(buffer[100]);
    if (sector_count == 0)
        sector_count = (static_cast<u32>(buffer[61]) << 16) | buffer[60];
    return true;
}
