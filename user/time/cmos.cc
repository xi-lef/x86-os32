#include "cmos.h"
#include "debug/output.h"

uint32_t CMOS::init_CMOS(bool enable_update_irq, CMOS_irq_freq periodic_irq_freq) {
    uint8_t statusB_flags = 0;
    uint32_t irqs_per_second = 0;

    // set up periodic interrupt
    if (periodic_irq_freq != freq_0hz) {
        statusB_flags |= irq_periodic;
        // set frequency of periodic interrupt to periodic_irq_freq
        uint8_t statusA_old = read_port(offset_statusA);
        // the top four bits should not be changed
        write_port(offset_statusA, (statusA_old & 0xf0) | periodic_irq_freq);

        irqs_per_second += CMOS_CALC_FREQ(periodic_irq_freq);
    }

    // set up update interrupt
    if (enable_update_irq) {
        statusB_flags |= irq_update;
        irqs_per_second++;
    }

    uint8_t statusB_old = read_port(offset_statusB);
    write_port(offset_statusB, statusB_old | statusB_flags);

    DBG_VERBOSE << "CMOS: init done" << endl;
    return irqs_per_second;
}

void CMOS::select_offset(CMOS_offset offset) {
    uint8_t ctrl_orig = CMOS_ctrl_port.inb();
    // bit 7 (nmi_bit) must NOT be changed!
    uint8_t ctrl_new = (ctrl_orig & nmi_bit) | (offset & ~nmi_bit);

    CMOS_ctrl_port.outb(ctrl_new);
}

uint8_t CMOS::read_port(CMOS_offset offset) {
    select_offset(offset);
    return CMOS_data_port.inb();
}

uint8_t CMOS::write_port(CMOS_offset offset, uint8_t value) {
    uint8_t data_old = read_port(offset);
    select_offset(offset); // see "Note1" at https://wiki.osdev.org/CMOS#Accessing_CMOS_Registers
    CMOS_data_port.outb(value);
    return data_old;
}

void CMOS::clear_statusC() {
    read_port(offset_statusC);
}

bool CMOS::is_update_irq() {
    return (read_port(offset_statusC) & irq_update);
}

bool CMOS::is_alarm_irq() {
    return (read_port(offset_statusC) & irq_alarm);
}

bool CMOS::is_periodic_irq() {
    return (read_port(offset_statusC) & irq_periodic);
}

uint16_t CMOS::bcd_to_int(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0xf);
}

uint16_t CMOS::int_to_bcd(uint8_t i) {
    return (i / 10 << 4) + i % 10;
}
