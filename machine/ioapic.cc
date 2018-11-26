// vim: set et ts=4 sw=4:

#include "ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "plugbox.h"

volatile uint32_t *IOAPIC::IOREGSEL_REG = (volatile uint32_t*)0xfec00000;
volatile uint32_t *IOAPIC::IOWIN_REG = (volatile uint32_t*)0xfec00010;

IOAPIC ioapic;

void IOAPIC::write_reg(uint32_t reg, IOREDTBL_L values) {
    *IOREGSEL_REG = reg;
    IOAPICRegister_t r;
    r.value = *IOWIN_REG;
    r.IOREDTBL_L = {
        .vector           = values.vector,
        .delivery_mode    = values.delivery_mode,
        .destination_mode = values.destination_mode,
        .delivery_status  = r.IOREDTBL_L.delivery_status,
        .polarity         = values.polarity,
        .remote_irr       = r.IOREDTBL_L.remote_irr,
        .trigger_mode     = values.trigger_mode,
        .mask             = values.mask,
        .reserved         = r.IOREDTBL_L.reserved
    };
    *IOWIN_REG = r.value;
}

void IOAPIC::write_reg(uint32_t reg, IOREDTBL_H values) {
    *IOREGSEL_REG = reg;
    IOAPICRegister_t r;
    r.value = *IOWIN_REG;
    r.IOREDTBL_H = {
        .reserved            = r.IOREDTBL_H.reserved,
        .logical_destination = values.logical_destination
    };
    *IOWIN_REG = r.value;
}

void IOAPIC::write_reg(uint32_t reg, IOAPICID values) {
    *IOREGSEL_REG = reg;
    IOAPICRegister_t r;
    r.value = *IOWIN_REG;
    r.IOAPICID = {
        .reserved_2 = r.IOAPICID.reserved_2,
        .ID         = values.ID,
        .reserved_1 = r.IOAPICID.reserved_1
    };
    *IOWIN_REG = r.value;
}

IOREDTBL_L IOAPIC::read_rte_l(uint32_t reg) {
    *IOREGSEL_REG = reg;
    IOAPICRegister_t rte;
    rte.value = *IOWIN_REG;
    return rte.IOREDTBL_L;
}

IOREDTBL_H IOAPIC::read_rte_h(uint32_t reg) {
    *IOREGSEL_REG = reg;
    IOAPICRegister_t rte;
    rte.value = *IOWIN_REG;
    return rte.IOREDTBL_H;
}

IOAPICID IOAPIC::read_rte_id(uint32_t reg) {
    *IOREGSEL_REG = reg;
    IOAPICRegister_t rte;
    rte.value = *IOWIN_REG;
    return rte.IOAPICID;
}


void IOAPIC::init() {
    // set APIC ID
    IOAPICID id;
    id.ID = system.getIOAPICID();
    write_reg(0x00, id);

    //set redirection table entries
    IOREDTBL_L rte_l;
    rte_l = {
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
    IOREDTBL_H rte_h;
    rte_h.logical_destination = 0x0f;

    for (int i = 0x10; i <= 0x3e; i += 0x02) {
        write_reg(i, rte_l);
        write_reg(i + 1, rte_h);
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
