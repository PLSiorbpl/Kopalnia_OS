#include "PCI.hpp"
#include "PLlib/types.hpp"
//#include "arch/x86/Common/common.hpp"
#include "PLlib/String_common.hpp"

namespace PCI {
    uint32_t pci_read32(const uint8_t bus, const uint8_t device, const uint8_t func, const uint8_t offset) {
        //const uint32_t address = (1u << 31) |
        //(static_cast<uint32_t>(bus) << 16) |
        //(static_cast<uint32_t>(device) << 11) |
        //(static_cast<uint32_t>(func) << 8) |
        //(offset & 0xFC);
        //x86::outl(0xCF8, address);
        //return x86::inl(0xCFC);
        return 0;
    }

    uint16_t pci_read16(const uint8_t bus, const uint8_t device, const uint8_t func, const uint8_t offset) {
        const uint32_t val = pci_read32(bus, device, func, offset);
        return (val >> ((offset & 2) * 8)) & 0xFFFF;
    }

    uint8_t pci_read8(const uint8_t bus, const uint8_t device, const uint8_t func, const uint8_t offset) {
        const uint32_t val = pci_read32(bus, device, func, offset);
        return (val >> ((offset & 3) * 8)) & 0xFF;
    }

    void Test() {
        for (int bus = 0; bus < 256; bus++) {
            for (int dev = 0; dev < 32; dev++) {
                const uint16_t vendor = pci_read16(bus, dev, 0, 0x00);

                if (vendor != 0xFFFF) {
                    const uint16_t device = pci_read16(bus, dev, 0, 0x02);

                    term::print("Vendor: ");
                    term::print_hex(vendor);
                    term::print(" Device: ");
                    term::print_hex(device);
                    term::print("\n");
                }
            }
        }
    }

    PCI_Device
    Find(uint32_t vendor, uint32_t device) {
        for (uint8_t bus = 0; bus < 256; ++bus) {
            for (uint8_t dev = 0; dev < 32; ++dev) {
                for (uint8_t fn = 0; fn < 8; ++fn) {
                    const uint16_t vid = pci_read16(bus, dev, fn, 0x00);
                    uint16_t did = pci_read16(bus, dev, fn, 0x02);
                    if (vid == vendor && did == device) {
                        uint16_t cmd = pci_read16(bus, dev, fn, 0x04);
                        term::print_hex(cmd);
                        term::print("\n");
                        PCI_Device gpu;
                        gpu.bus = bus; gpu.device = dev; gpu.function = fn;
                        gpu.vendor_id = vid; gpu.device_id = did;
                        for (int i = 0; i < 6; ++i) {
                            gpu.bar[i] = pci_read32(bus, dev, fn, 0x10 + i*4);
                        }
                        return gpu;
                    }
                }
            }
        }
        constexpr PCI_Device none = {};
        return none;
    }
}
