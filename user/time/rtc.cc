// vim: set et ts=4 sw=4:

#include "user/time/rtc.h"
#include "machine/ioapic.h"
#include "machine/apicsystem.h"
#include "debug/output.h"
#include "user/time/time.h"
#include "machine/ticketlock.h"
#include "syscall/guarded_scheduler.h"

RTC rtc;
static Ticketlock tlock;
static bool init;

void RTC::init_RTC(bool enable_update_irq, CMOS_irq_freq periodic_irq_freq) {
    if (init) {
        DBG << "RTC: tried to init multiple times" << endl;
        return;
    }
    hz = init_CMOS(enable_update_irq, periodic_irq_freq);
    set_time();

    Plugbox::Vector rtc_vector = Plugbox::Vector::rtc;
    unsigned char   rtc_slot   = system.getIOAPICSlot(APICSystem::Device::rtc);

    plugbox.assign(rtc_vector, &rtc);
    ioapic.config(rtc_slot, rtc_vector);
    ioapic.allow(rtc_slot);
    clear_statusC();

    init = true;
    DBG << "RTC: init done (" << hz << "hz)" << endl;
}

bool RTC::prologue() {
    jiffies = (jiffies + 1) % hz;
    if (is_update_irq()) {
        time++;
        return true;
    }
    return false;
}

void RTC::epilogue() {
    dout_clock.reset();
    dout_clock << time << flush;
}

uint16_t RTC::get_value(CMOS_offset offset) {
    // wait until the RTC is done updating the values, then read it.
    // lock in case multiple CPUs try to read at the same time.
    // this should not happen, but it would be racey.
    tlock.lock();
    while ((read_port(offset_statusA) & (1 << 7)) == 1) ;
    uint16_t ret = read_port(offset);
    tlock.unlock();
    return bcd_to_int(ret);
}

uint16_t RTC::get_second () { return get_value(offset_second);  }
uint16_t RTC::get_minute () { return get_value(offset_minute);  }
uint16_t RTC::get_hour   () { return get_value(offset_hour);    }
uint16_t RTC::get_day    () { return get_value(offset_day);     }
uint16_t RTC::get_month  () { return get_value(offset_month);   }
uint16_t RTC::get_year   () { return get_value(offset_year);    }
uint16_t RTC::get_weekday() { return get_value(offset_weekday); }
uint16_t RTC::get_century() { return get_value(offset_century); }

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

void RTC::sleep(unsigned int t) {
    // if RTC was not initialized, time will not be updated through update IRQ
    if (init) {
        uint16_t end = (time.second + t) % 60;
        while (time.second != end) { // '!=' instead of '<' in case starting time is e.g. 59
            Guarded_Scheduler::resume(); // do not waste CPU time
        }
    } else {
        uint16_t end = (get_second() + t) % 60;
        while (get_second() != end) {
            Guarded_Scheduler::resume();
        }
    }
}
