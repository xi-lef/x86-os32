#include "cmos.h"
#include "debug/output.h"

uint32_t CMOS::init_CMOS(bool enable_update_irq, CMOS_irq_freq periodic_irq_freq) {
    if (initialized) {
        DBG << "Tried to init CMOS multiple times!" << endl;
        return CMOS_CALC_FREQ(periodic_irq_freq) + (enable_update_irq ? 1 : 0);
    }
    uint32_t jiffies_per_second = 0;
    uint8_t statusB_flags = 0;

    // set frequency of periodic interrupt to periodic_irq_freq
    statusB_flags |= irq_periodic;

    uint8_t statusA_old = read_port(offset_statusA);
    write_port(offset_statusA, (statusA_old & 0xf0) | periodic_irq_freq);

    jiffies_per_second += CMOS_CALC_FREQ(periodic_irq_freq);

    // possibly enable update interrupt and periodic interrupt
    if (enable_update_irq) {
        statusB_flags |= irq_update;
        jiffies_per_second++;
    }
    uint8_t statusB_old = read_port(offset_statusB);
    write_port(offset_statusB, statusB_old | statusB_flags);

    DBG_VERBOSE << "CMOS: init done" << endl;

    return jiffies_per_second;
}

uint8_t CMOS::read_port(CMOS_offset offset) {
    // bit 7 (MSB) must NOT be changed! (NMI)
    uint8_t ctrl_orig = CMOS_ctrl_port.inb();
    uint8_t ctrl_new = (ctrl_orig & (1 << 7)) | offset;

    CMOS_ctrl_port.outb(ctrl_new);
    return CMOS_data_port.inb();
}

uint8_t CMOS::write_port(CMOS_offset offset, uint8_t value) {
    uint8_t data_old = read_port(offset);
    CMOS_data_port.outb(value);

    return data_old;
}

void CMOS::clear_statusC() {
    read_port(offset_statusC);
}

bool CMOS::is_update_irq() {
    return (read_port(offset_statusC) & (1 << 4));
}

bool CMOS::is_alarm_irq() {
    return (read_port(offset_statusC) & (1 << 5));
}

bool CMOS::is_periodic_irq() {
    return (read_port(offset_statusC) & (1 << 6));
}

uint16_t CMOS::bcd_to_int(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0xf);
}
