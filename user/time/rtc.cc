// vim: set et ts=4 sw=4:

#include "user/time/rtc.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/time.h"
#include "machine/ticketlock.h"

RTC rtc;
static Ticketlock tlock;

void RTC::init_RTC(bool enable_update_irq, CMOS_irq_freq periodic_irq_freq) {
    hz = init_CMOS(enable_update_irq, periodic_irq_freq);
    set_time();

    Plugbox::Vector rtc_vector = Plugbox::Vector::rtc;
    unsigned char   rtc_slot   = system.getIOAPICSlot(APICSystem::Device::rtc);

    plugbox.assign(rtc_vector, &rtc);
    ioapic.config(rtc_slot, rtc_vector);
    ioapic.allow(rtc_slot);
    clear_statusC();
    DBG << "RTC: init done (" << hz << "hz)" << endl;
}

/*
#include "object/bbuffer.h"
#include "device/keyboard.h"
#include "guard/guardian.h"
extern BBuffer<Key, 64> buf;
void stresstest() {
    Key k;
    k.ascii(id + 97);
    buf.produce(k);
    guardian(33, NULL);
    //asm volatile ("int 33\n" : );
}
//*/

bool RTC::prologue() {
    //stresstest();
    //DBG << "RTC: prologue " << flush;
    jiffies = (jiffies + 1) % hz;
    if (is_update_irq()) {
        time++;
        return true;
    }
    return false;
}

void RTC::epilogue() {
    //DBG << "RTC: epilogue " << flush;
    dout_clock.reset();
    dout_clock << time << flush;
}

uint16_t RTC::get_value(CMOS_offset offset) {
    // wait until the RTC is done updating the values, then read it.
    // lock in case multiple CPUs try to read at the same time.
    // this would be racey.
    tlock.lock();
    while ((read_port(offset_statusA) & (1 << 7)) == 1) ;
    uint16_t ret = read_port(offset);
    tlock.unlock();
    return bcd_to_int(ret);
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
    time.second  = get_second ();
    time.minute  = get_minute ();
    time.hour    = get_hour   ();
    time.day     = get_day    ();
    time.month   = get_month  ();
    time.year    = get_year   ();
    time.weekday = get_weekday();
    time.century = get_century();
    
    // set timezone
    time = time + HOURS_TO_SECONDS(time.timezone);
}

void RTC::sleep(unsigned int time) {
    //DBG << "sleep for " << time - 1 << "s to " << time << "s" << endl;
    uint16_t start = get_second();
    uint16_t end   = (start + time) % 60;
    uint16_t s;
    //DBG << "start: " << start << ", end: " << end << endl; // TODO wtf man
    while ((s = get_second()) < end) ;//DBG << "start: " << start << ", second: " << s << ", end: " << end << endl;
    //DBG << "s: " << s << endl;
}
