#pragma once

#include "types.h"

#define SELECTOR_KERNEL_CS 8
#define TYPE_TASK 5
#define TYPE_IRQ  6
#define TYPE_TRAP 7
#define MODE_16 0
#define MODE_32 1
#define STORAGE_IRQ 0
#define PRESENT_ACTIVE 1
#define PRESENT_INACTIVE 0

union idt_entry {
    struct {
    uint32_t address_low:16,    // lower 2 bytes of address
             selector:16,       // codesegment during irq handler
             unused:8,          // 0
             type:3,            // see TYPE_*
             mode:1,            // 16 bit or 32 bit, see MODE_*
             storage:1,         // 0 for irq or trap
             privilege_level:2, // no idea lol, just 0
             present:1,         // active or inactive
             address_high:16;   // higher 2 bytes of address
    } __attribute__((packed));
    uint64_t value;
} __attribute__((packed));

extern idt_entry idt[]; // from boot/startup.asm
