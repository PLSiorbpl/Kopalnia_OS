#include "PCI.hpp"

#include <std/printf.hpp>

#include "std/types.hpp"
#include "arch/x86_64/Common/Common.hpp"

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

                    std::printf("&fVendor: &a%x&f, Device: &a%x ", std::Output::std_out, vendor, device);
                    switch (vendor) {
                        case 0x8086: {
                            std::printf("&bIntel ");
                            switch (device) {
                                case 0x5914: {
                                    std::printf("&7Xeon E3-1200 Host Bridge");
                                    break;
                                }
                                case 0x5917: {
                                    std::printf("&7UHD Graphics 620");
                                    break;
                                }
                                case 0x9D4E: {
                                    std::printf("&7100 Series Chipset Family");
                                    break;
                                }
                                case 0x1237: {
                                    std::printf("&7440FX - 82441FX PMC");
                                    break;
                                }
                                case 0x7000: {
                                    std::printf("&782371SB PIIX3 ISA");
                                    break;
                                }
                                case 0x100E: {
                                    std::printf("&7Gigabit Ethernet Controller");
                                    break;
                                }
                            }
                            break;
                        }
                        case 0x10DE: {
                            std::printf("&aNvidia ");
                            if (device == 0x1d10)
                                std::printf("&7GeForce MX150");
                            break;
                        }
                        case 0x10EC: {
                            std::printf("&9Realtek ");
                            if (device == 0x5287)
                                std::printf("&7RTL8411B PCI Express Card Reader");
                            break;
                        }
                        case 0x168C: {
                            std::printf("&dAtheros ");
                            if (device == 0x003e)
                                std::printf("&7Wireless Network Adapter");
                            break;
                        }
                        default: std::printf("&cUnknown");
                    }
                    std::printf("\n");
                    devices++;
                }
            }
        }
        std::printf("&ffound &a%u &fPCI Devices\n", std::Output::std_out, devices);
    }

    PCI_Device Find(const uint32_t vendor, const uint32_t device_) {
        for (int bus = 0; bus < 256; bus++) {
            for (int dev = 0; dev < 32; dev++) {
                for (int fn = 0; fn < 8; fn++) {
                    const uint16_t vid = pci_read16(bus, dev, fn, 0x00);
                    const uint16_t did = pci_read16(bus, dev, fn, 0x02);
                    if (vid == vendor && did == device_) {
                        PCI_Device device{};
                        device.bus = bus; device.device = dev; device.function = fn;
                        device.vendor_id = vid; device.device_id = did;
                        for (int i = 0; i < 6; ++i) {
                            device.bar[i] = pci_read32(bus, dev, fn, 0x10 + i*4);
                        }
                        return device;
                    }
                }
            }
        }
        constexpr PCI_Device none = {};
        return none;
    }

    uint32_t get_class(const uint8_t bus, const uint8_t dev, const uint8_t fn) {
        const uint8_t base = pci_read8(bus, dev, fn, 0x0B);
        const uint8_t sub  = pci_read8(bus, dev, fn, 0x0A);
        const uint8_t ifc  = pci_read8(bus, dev, fn, 0x09);

        return (ifc << 16) | (sub << 8) | base;
    }

    PCI_Device Find_Class(const uint32_t Class) {
        for (int bus = 0; bus < 256; bus++) {
            for (int dev = 0; dev < 32; dev++) {
                uint16_t vid = pci_read16(bus, dev, 0, 0x00);
                if (vid == 0xFFFF) continue;
                const uint8_t header = pci_read8(bus, dev, 0, 0x0E);
                const int fn_limit = (header & 0x80) ? 8 : 1;

                for (int fn = 0; fn < fn_limit; fn++) {
                    vid = pci_read16(bus, dev, fn, 0x00);
                    if (vid == 0xFFFF || vid == 0x0000) continue;
                    uint32_t test = get_class(bus, dev, fn);
                    if (test == Class) {
                        PCI_Device device{};
                        device.bus = bus;
                        device.device = dev;
                        device.function = fn;
                        device.vendor_id = vid;
                        device.device_id = pci_read16(bus, dev, fn, 0x02);

                        for (int i = 0; i < 6; ++i) {
                            device.bar[i] =
                                pci_read32(bus, dev, fn, 0x10 + i * 4);
                        }
                        return device;
                    }
                }
            }
        }
        return {};
    }

    PCI_Device find_class_with_sub(const u8 base_class, const u8 sub_class) {
        for (int bus = 0; bus < 256; bus++) {
            for (int dev = 0; dev < 32; dev++) {
                uint16_t vid = pci_read16(bus, dev, 0, 0x00);
                if (vid == 0xFFFF) continue;
                const uint8_t header = pci_read8(bus, dev, 0, 0x0E);
                const int fn_limit = (header & 0x80) ? 8 : 1;

                for (int fn = 0; fn < fn_limit; fn++) {
                    vid = pci_read16(bus, dev, fn, 0x00);
                    if (vid == 0xFFFF || vid == 0x0000) continue;

                    const uint8_t base = pci_read8(bus, dev, fn, 0x0B);
                    const uint8_t sub  = pci_read8(bus, dev, fn, 0x0A);

                    if (base == base_class && sub == sub_class) {
                        PCI_Device device{};
                        device.bus = bus;
                        device.device = dev;
                        device.function = fn;
                        device.vendor_id = vid;
                        device.device_id = pci_read16(bus, dev, fn, 0x02);
                        for (int i = 0; i < 6; ++i)
                            device.bar[i] = pci_read32(bus, dev, fn, 0x10 + i * 4);
                        return device;
                    }
                }
            }
        }
        return {};
    }
}
