#include "PCI.hpp"

#include <std/printf.hpp>

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
        term::print_number(devices);
        term::print(" PCI Devices\n");
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

        //std::printf("class: %x  ", (ifc << 16) | (sub << 8) | base);
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
}
