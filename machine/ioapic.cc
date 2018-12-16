// vim: set et ts=4 sw=4:

#include "ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "plugbox.h"

volatile uint32_t *IOAPIC::IOREGSEL_REG = (volatile uint32_t*)0xfec00000;
volatile uint32_t *IOAPIC::IOWIN_REG = (volatile uint32_t*)0xfec00010;

IOAPIC ioapic;

void IOAPIC::write_reg(uint32_t reg, IOAPICRegister_t entry) {
    *IOREGSEL_REG = reg;
    *IOWIN_REG = entry.value;
}

IOAPICRegister_t IOAPIC::read_reg(uint32_t reg) {
    *IOREGSEL_REG = reg;
    IOAPICRegister_t ret = {
        .value = *IOWIN_REG
    };
    return ret;
}

void IOAPIC::init() {
    // set APIC ID
    IOAPICRegister_t id = read_reg(0x00);
    id.IOAPICID.ID = system.getIOAPICID();
    write_reg(0x00, id);

    // create bitmask for logical_destination
    uint8_t cpu_bitmask = (1 << system.getNumberOfCPUs()) - 1;

    // set redirection table entries
    for (int i = 0x10; i <= 0x3e; i += 0x02) {
        IOAPICRegister_t rte_l = read_reg(i);
        rte_l.IOREDTBL_L.vector           = Plugbox::Vector::panic;
        rte_l.IOREDTBL_L.delivery_mode    = DELIVERY_MODE_LOWESTPRI;
        rte_l.IOREDTBL_L.destination_mode = DESTINATION_MODE_LOGICAL;
        rte_l.IOREDTBL_L.polarity         = POLARITY_HIGH;
        rte_l.IOREDTBL_L.trigger_mode     = TRIGGER_MODE_EDGE;
        rte_l.IOREDTBL_L.mask             = MASK_DISABLED;

        IOAPICRegister_t rte_h = read_reg(i);
        rte_h.IOREDTBL_H.logical_destination = cpu_bitmask;

        write_reg(i, rte_l);
        write_reg(i + 1, rte_h);
    }

    DBG_VERBOSE << "IOAPIC: init done" << endl;
}

void IOAPIC::config(unsigned char slot, Plugbox::Vector vector,
        int trigger_mode, int polarity) {
    uint32_t reg = 0x10 + 2 * slot;

    IOAPICRegister_t cur_l = read_reg(reg);
    cur_l.IOREDTBL_L.vector = vector;
    cur_l.IOREDTBL_L.polarity = polarity;
    cur_l.IOREDTBL_L.trigger_mode = trigger_mode;
    write_reg(reg, cur_l);
    
    DBG_VERBOSE << "IOAPIC: config done for slot " << int(slot) << endl;
}

void IOAPIC::allow(unsigned char slot) {
    uint32_t reg = 0x10 + 2 * slot;

    IOAPICRegister_t cur_l = read_reg(reg);
    cur_l.IOREDTBL_L.mask = MASK_ENABLED;
    write_reg(reg, cur_l);
}

void IOAPIC::forbid(unsigned char slot) {
    uint32_t reg = 0x10 + 2 * slot;

    IOAPICRegister_t cur_l = read_reg(reg);
    cur_l.IOREDTBL_L.mask = MASK_DISABLED;
    write_reg(reg, cur_l);
}

bool IOAPIC::status(unsigned char slot) {
    uint32_t reg = 0x10 + 2 * slot;

    IOAPICRegister_t cur_l = read_reg(reg);
    return !(cur_l.IOREDTBL_L.mask);
}
