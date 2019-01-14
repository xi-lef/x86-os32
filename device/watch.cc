// vim: set et ts=4 sw=4:

#include "device/watch.h"
#include "machine/plugbox.h"
#include "machine/lapic.h"
#include "utils/math.h"
#include "debug/output.h"
#include "thread/scheduler.h"
#include "syscall/guarded_scheduler.h"

Watch watch;

bool Watch::windup(uint32_t us) {
    uint64_t tmp = Math::div64((uint64_t) us * lapic.timer_ticks(), 1000);
    int x = 0;
    while (tmp >> (32 + x) != 0) {
        if (++x > 7) { // TODO hmmm
            DBG << "Watch: windup overflow" << endl;
            return false;
        }
    }

    // save everything required for interval/activate
    irq_interval = us;
    initial_count = tmp >> x;
    divide = 1 << x;
    //DBG << "initial_count: " << initial_count << ", divide: " << int(divide) << endl;

    plugbox.assign(Plugbox::Vector::timer, &watch);

    return true;
}

bool Watch::prologue() {
    //DBG << "Watch: prologue " << flush;
    return true; // TODO hm
}

void Watch::epilogue() {
    scheduler.resume(); // TODO deadlock ?
}

uint32_t Watch::interval() {
    return irq_interval;
}

void Watch::activate() {
    lapic.setTimer(initial_count, divide, Plugbox::Vector::timer, true, false);
}
