// vim: set et ts=4 sw=4:

#include "user/time/cappl.h"
#include "user/app1/appl.h"
#include "device/cgastr.h"
#include "guard/secure.h"
#include "thread/scheduler.h"
#include "syscall/guarded_scheduler.h"
#include "user/time/rtc.h"
#include "syscall/guarded_bell.h"

extern Thread *victim;

void ClockApplication::action() {
    Guarded_Bell::sleep(1000);
    Guarded_Scheduler::kill(victim);
    Guarded_Scheduler::exit();

    for (;;) {
        //DBG << "Clock_App " << id << ": action " << flush;
        rtc.update_time();
        dout_clock.reset();
        dout_clock << rtc << flush;
    }
}

void ClockApplication::setID(int i) {
    id = i;
}
