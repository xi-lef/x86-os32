#include "cmos.h"
#include "machine/io_port.h"
#include "debug/output.h"

static const IO_Port ctrl_port(0x70);
static const IO_Port data_port(0x71);

uint32_t CMOS::init(bool enable_update_irq, IRQ_freq periodic_irq_freq) {
    uint32_t irqs_per_second = 0;
    Status_B b { .value = read_port(Offset::statusB) };

    // set up periodic interrupt
    if (periodic_irq_freq != freq_0hz) {
        b.periodic_irq = true;

        // set frequency of periodic interrupt to periodic_irq_freq
        Status_A a = { .value = read_port(Offset::statusA) };
        a.freq = periodic_irq_freq;
        write_port(Offset::statusA, a.value);

        irqs_per_second += calc_freq(periodic_irq_freq);
    }

    // set up update interrupt
    if (enable_update_irq) {
        b.update_irq = true;
        irqs_per_second++;
    }

    write_port(Offset::statusB, b.value);

    DBG_VERBOSE << "CMOS: init done" << endl;
    return irqs_per_second;
}

void CMOS::select_offset(Offset offset) {
    uint8_t val = ctrl_port.inb();
    // the NMI-bit must NOT be changed!
    val = (val & nmi_bitmask) | (offset & ~nmi_bitmask);
    ctrl_port.outb(val);
}

uint8_t CMOS::read_port(Offset offset) {
    select_offset(offset);
    return data_port.inb();
}

uint8_t CMOS::write_port(Offset offset, uint8_t value) {
    uint8_t data_old = read_port(offset);
    // see "Note1" at https://wiki.osdev.org/CMOS#Accessing_CMOS_Registers
    select_offset(offset);
    data_port.outb(value);
    return data_old;
}

void CMOS::clear_status_c() {
    read_port(Offset::statusC);
}

bool CMOS::is_update_irq() {
    return (read_port(Offset::statusC) & irq_update);
}

bool CMOS::is_alarm_irq() {
    return (read_port(Offset::statusC) & irq_alarm);
}

bool CMOS::is_periodic_irq() {
    return (read_port(Offset::statusC) & irq_periodic);
}
