#include "PCI.hpp"
#include "PLlib/types.hpp"
#include "arch/x86_64/Common/Common.hpp"
#include "PLlib/String_common.hpp"

namespace PCI {
    uint32_t pci_read32(const uint8_t bus, const uint8_t device, const uint8_t func, const uint8_t offset) {
        const uint32_t address = (1u << 31) |
        (static_cast<uint32_t>(bus) << 16) |
        (static_cast<uint32_t>(device) << 11) |
        (static_cast<uint32_t>(func) << 8) |
        (offset & 0xFC);
        x64::outl(0xCF8, address);
        return x64::inl(0xCFC);
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
        uint32_t devices = 0;
        for (int bus = 0; bus < 256; bus++) {
            for (int dev = 0; dev < 32; dev++) {
                const uint16_t vendor = pci_read16(bus, dev, 0, 0x00);

                if (vendor != 0xFFFF && vendor != 0000) {
                    const uint16_t device = pci_read16(bus, dev, 0, 0x02);

                    term::print("Vendor: ");
                    term::print_hex(vendor);
                    term::print(" Device: ");
                    term::print_hex(device);
                    term::print(" ");
                    switch (vendor) {
                        case 0x8086: {
                            term::print("Intel ");
                            switch (device) {
                                case 0x5914: {
                                    term::print("Xeon E3-1200 Host Bridge");
                                    break;
                                }
                                case 0x5917: {
                                    term::print("UHD Graphics 620");
                                    break;
                                }
                                case 0x9D4E: {
                                    term::print("100 Series Chipset Family");
                                    break;
                                }
                                case 0x1237: {
                                    term::print("440FX - 82441FX PMC");
                                    break;
                                }
                                case 0x7000: {
                                    term::print("82371SB PIIX3 ISA");
                                    break;
                                }
                                case 0x100E: {
                                    term::print("Gigabit Ethernet Controller");
                                    break;
                                }
                            }
                            break;
                        }
                        case 0x10DE: {
                            term::print("Nvidia ");
                            if (device == 0x1d10)
                                term::print("GeForce MX150");
                            break;
                        }
                        case 0x10EC: {
                            term::print("Realtek ");
                            if (device == 0x5287)
                                term::print("RTL8411B PCI Express Card Reader");
                            break;
                        }
                        case 0x168C: {
                            term::print("Atheros ");
                            if (device == 0x003e)
                                term::print("Wireless Network Adapter");
                            break;
                        }
                        default: term::print("Unknown");
                    }
                    term::print("\n");
                    devices++;
                }
            }
        }
        term::print("found: ");
        term::print_uint(devices);
        term::print(" PCI Devices\n");
    }

    PCI_Device Find(const uint32_t vendor, const uint32_t device) {
        for (int bus = 0; bus < 256; ++bus) {
            for (int dev = 0; dev < 32; ++dev) {
                for (int fn = 0; fn < 8; ++fn) {
                    const uint16_t vid = pci_read16(bus, dev, fn, 0x00);
                    uint16_t did = pci_read16(bus, dev, fn, 0x02);
                    if (vid == vendor && did == device) {
                        const uint16_t cmd = pci_read16(bus, dev, fn, 0x04);
                        term::print_hex(cmd);
                        term::print("\n");
                        PCI_Device gpu{};
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
