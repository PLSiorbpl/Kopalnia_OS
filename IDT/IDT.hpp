#pragma once
#include "types.hpp"

namespace IDT {
    //#define PIC1 0x20
    //#define PIC2 0xA0
    //#define PIC1_COMMAND PIC1
    //#define PIC1_DATA    (PIC1+1)
    //#define PIC2_COMMAND PIC2
    //#define PIC2_DATA    (PIC2+1)
    //#define ICW1_INIT    0x11
    //#define ICW4_8086    0x01

    //struct IDTEntry {
    //    uint16_t offset_low;
    //    uint16_t selector;
    //    uint8_t  zero;
    //    uint8_t  type_attr;
    //    uint16_t offset_high;
    //} __attribute__((packed));
//
    //struct IDTPtr {
    //    uint16_t limit;
    //    uint32_t base;
    //} __attribute__((packed));
//
    //extern IDTEntry idt[256];
    //extern IDTPtr idt_ptr;
//
    //// IDT main
    //void idt_set_entry(int n, uint32_t handler, uint16_t sel, uint8_t flags);
    //void idt_install();
//
    //// Handlers
    //extern "C" void isr_dummy();
//
    //extern "C" void irq0_handler(); // Timer
    //extern "C" void irq0_handler_c();
    //inline volatile uint64_t tick = 0;
//
    //extern "C" void irq1_handler(); // Keyboard
    //extern "C" void irq1_handler_c();
//
    //void pic_remap();
}