// vim: set et ts=4 sw=4:

#include "user/time/rtc.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/time.h"

RTC rtc;

void RTC::init() {
    init_cmos();
    Plugbox::Vector rtc_vector = Plugbox::Vector::rtc;
    unsigned char   rtc_slot   = system.getIOAPICSlot(APICSystem::Device::rtc);

    plugbox.assign(rtc_vector, &rtc);
    ioapic.config(rtc_slot, rtc_vector);
    ioapic.allow(rtc_slot);
    clear_statusC();
}

extern CGA_Stream dout_clock;

void RTC::trigger() {
    DBG << "RTC says hi" << flush;
    update_clock(dout_clock);
    clear_statusC();
}
