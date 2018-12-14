// vim: set et ts=4 sw=4:

#include "user/time/rtc.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/time.h"

RTC rtc;

void RTC::init_RTC() {
    hz = init_CMOS(true, freq_0hz);
    set_time();
    increment_seconds(HOURS_TO_SECONDS(timezone));
    DBG << "RTC: init done (" << hz << "hz)" << endl;

    Plugbox::Vector rtc_vector = Plugbox::Vector::rtc;
    unsigned char   rtc_slot   = system.getIOAPICSlot(APICSystem::Device::rtc);

    plugbox.assign(rtc_vector, &rtc);
    ioapic.config(rtc_slot, rtc_vector);
    ioapic.allow(rtc_slot);
    clear_statusC();
}

bool RTC::prologue() {
    //DBG << "RTC: prologue " << flush;
    static uint32_t jiffies = 0;
    jiffies = (jiffies + 1) % hz;
    if (is_update_irq()) {
        increment_seconds();
        return true;
    }
    return false;
}

void RTC::epilogue() {
    //DBG << "RTC: epilogue " << flush;
    dout_clock.reset();
    dout_clock << *this << flush; // TODO not *this
}

uint16_t RTC::get_value(CMOS_offset offset) {
    while ((read_port(offset_statusA) & (1 << 7)) == 1) ;
    return bcd_to_int(read_port(offset));
}

uint16_t RTC::get_second () {
    return get_value(offset_second);
}

uint16_t RTC::get_minute () {
    return get_value(offset_minute);
}

uint16_t RTC::get_hour   () {
    return get_value(offset_hour);
}

uint16_t RTC::get_day    () {
    return get_value(offset_day);
}

uint16_t RTC::get_month  () {
    return get_value(offset_month);
}

uint16_t RTC::get_year   () {
    return get_value(offset_year);
}

uint16_t RTC::get_weekday() {
    return get_value(offset_weekday);
}

uint16_t RTC::get_century() {
    return get_value(offset_century);
}

void RTC::set_time() {
    second  = get_second ();
    minute  = get_minute ();
    hour    = get_hour   ();
    day     = get_day    ();
    month   = get_month  ();
    year    = get_year   ();
    weekday = get_weekday();
    century = get_century();
}

void RTC::sleep(unsigned int time) {
    //DBG << "sleep for " << time - 1 << "s to " << time << "s" << endl;
    uint8_t start = get_second();
    while (get_second() < (start + time) % 60) {
        //CPU::idle();
    }
}
