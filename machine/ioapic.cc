// vim: set et ts=4 sw=4:

#include "ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "plugbox.h"

volatile uint32_t *IOAPIC::IOREGSEL_REG = (volatile uint32_t*)0xfec00000;
volatile uint32_t *IOAPIC::IOWIN_REG = (volatile uint32_t*)0xfec00010;

IOAPIC ioapic;

void IOAPIC::init() {
    // set APIC ID
    *IOREGSEL_REG = 0x00;
    IOAPICRegister_t id;
    id.IOAPICID.ID = system.getIOAPICID();
    *IOWIN_REG = id.value;

    //set redirection table entries
    IOAPICRegister_t rte_l;
    rte_l.IOREDTBL_L = {
        .vector              = Plugbox::Vector::panic,
        .delivery_mode       = DELIVERY_MODE_LOWESTPRI,
        .destination_mode    = DESTINATION_MODE_LOGICAL,
        .delivery_status     = 0,
        .polarity            = POLARITY_HIGH,
        .remote_irr          = 0,
        .trigger_mode        = TRIGGER_MODE_EDGE,
        .mask                = MASK_DISABLED,
        .reserved            = 0
    };
    IOAPICRegister_t rte_h;
    rte_h.IOREDTBL_H = {
        .reserved            = 0,
        .logical_destination = 0x0f
    };

    for (int i = 0x10; i <= 0x3e; i += 0x02) {
        *IOREGSEL_REG = i;
        *IOWIN_REG    = rte_l.value;
        *IOREGSEL_REG = i + 1;
        *IOWIN_REG    = rte_h.value;
    }
    
    DBG << "IOAPIC init done" << endl;
}

void IOAPIC::config(unsigned char slot, Plugbox::Vector vector) {
    *IOREGSEL_REG = 0x10 + 2 * slot;
    IOAPICRegister_t cur_l = {
        .value = *IOWIN_REG
    };
    cur_l.IOREDTBL_L.vector = vector;
    *IOWIN_REG = cur_l.value;
    DBG << "config done for slot " << int(slot) << endl;
}

void IOAPIC::allow(unsigned char slot) {
    *IOREGSEL_REG = 0x10 + 2 * slot;
    IOAPICRegister_t cur_l = {
        .value = *IOWIN_REG
    };
    cur_l.IOREDTBL_L.mask = MASK_ENABLED;
    *IOWIN_REG = cur_l.value;
}

void IOAPIC::forbid(unsigned char slot) {
    *IOREGSEL_REG = 0x10 + 2 * slot;
    IOAPICRegister_t cur_l = {
        .value = *IOWIN_REG
    };
    cur_l.IOREDTBL_L.mask = MASK_DISABLED;
    *IOWIN_REG = cur_l.value;
}

bool IOAPIC::status(unsigned char slot) {
    *IOREGSEL_REG = 0x10 + 2 * slot;
    IOAPICRegister_t cur_l = {
        .value = *IOWIN_REG
    };
    return !cur_l.IOREDTBL_L.mask;
}
