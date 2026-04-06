#pragma once
#include "PLlib/types.hpp"

namespace PCI {
    struct PCI_Device {
        uint8_t bus;
        uint8_t device;
        uint8_t function;
        uint16_t vendor_id;
        uint16_t device_id;
        uint32_t bar[6];
    };

    uint32_t pci_read32(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
    uint16_t pci_read16(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
    uint8_t pci_read8(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

    void Test();

    PCI_Device Find(uint32_t vendor, uint32_t device);
}