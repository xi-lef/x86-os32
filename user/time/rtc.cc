// vim: set et ts=4 sw=4:

#include "user/time/rtc.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/time.h"
#include "machine/ticketlock.h"
#include "syscall/guarded_scheduler.h"
#include "utils/math.h"

RTC rtc;

int32_t RTC::get_freq() const {
    return hz;
}

void RTC::init(bool enable_update_irq, CMOS::IRQ_freq periodic_irq_freq) {
    hz = CMOS::init(enable_update_irq, periodic_irq_freq);
    update_time();

    Plugbox::Vector rtc_vector = Plugbox::Vector::rtc;
    unsigned char   rtc_slot   = system.getIOAPICSlot(APICSystem::Device::rtc);

    plugbox.assign(rtc_vector, this);
    ioapic.config(rtc_slot, rtc_vector, TRIGGER_MODE_LEVEL);
    ioapic.allow(rtc_slot);

    clear_status_c();

    DBG << "RTC: init done (" << hz << "hz)" << endl;
}

bool RTC::prologue() {
    if (is_update_irq()) {
        increment_seconds();
        return true;
    }
    return false;
}

void RTC::epilogue() {
    dout_clock.reset();
    dout_clock << *this << flush;
}

bool RTC::is_updating() {
    CMOS::Status_A a = { .value = read_port(CMOS::Offset::statusA) };
    return a.updating;
}

uint16_t RTC::get_value(CMOS::Offset offset) {
    // Wait until the RTC is done updating the values, then read it. This is
    // not ideal, but the ideal way is slow.
    while (is_updating()) ;

    return Math::bcd_to_int(read_port(offset));
}

uint16_t RTC::get_second()  { return get_value(Offset::second); }
uint16_t RTC::get_minute()  { return get_value(Offset::minute); }
uint16_t RTC::get_hour()    { return get_value(Offset::hour); }
uint16_t RTC::get_day()     { return get_value(Offset::day); }
uint16_t RTC::get_month()   { return get_value(Offset::month); }
uint16_t RTC::get_year()    { return get_value(Offset::year); }
uint16_t RTC::get_weekday() { return get_value(Offset::weekday); }
uint16_t RTC::get_century() { return get_value(Offset::century); }

uint16_t RTC::get_real_year() {
    return get_century() * 100 + get_year();
}

void RTC::set_value(CMOS::Offset offset, uint16_t value) {
    if (value > 99) {
        DBG << "RTC: invalid value (> 99)" << endl;
        return;
    }

    while (is_updating()) ;
    write_port(offset, Math::int_to_bcd(value));
}

void RTC::set_second(uint16_t value)  { return set_value(Offset::second, value); }
void RTC::set_minute(uint16_t value)  { return set_value(Offset::minute, value); }
void RTC::set_hour(uint16_t value)    { return set_value(Offset::hour, value); }
void RTC::set_day(uint16_t value)     { return set_value(Offset::day, value); }
void RTC::set_month(uint16_t value)   { return set_value(Offset::month, value); }
void RTC::set_year(uint16_t value)    { return set_value(Offset::year, value); }
void RTC::set_weekday(uint16_t value) { return set_value(Offset::weekday, value); }
void RTC::set_century(uint16_t value) { return set_value(Offset::century, value); }

void RTC::set_local_hour(uint16_t hour) {
    long h = (hour - timezone) % 24;
    while (h < 0) h += 24;
    set_hour(h);
}

void RTC::set_real_year(uint16_t year) {
    set_century(year / 100);
    set_year(year % 100);
}

void RTC::update_time() {
    Time::second  = get_second();
    Time::minute  = get_minute();
    Time::hour    = get_hour();
    Time::day     = get_day();
    Time::month   = get_month();
    Time::year    = get_year();
    Time::weekday = get_weekday();
    Time::century = get_century();

    apply_timezone();
}
