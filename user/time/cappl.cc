// vim: set et ts=4 sw=4:

#include "user/time/cappl.h"
#include "user/app1/appl.h"
#include "device/cgastr.h"
#include "guard/secure.h"
#include "thread/scheduler.h"
#include "syscall/guarded_scheduler.h"
#include "user/time/rtc.h"

extern Application a0;

void Clock_Application::action() {
    rtc.sleep(2);
    Guarded_Scheduler::kill(&a0);
    for (;;) {
        //DBG << "Clock_App " << id << ": action " << flush;
        rtc.set_time();
        dout_clock.reset();
        dout_clock << rtc.get_time() << flush;
    }
}

void Clock_Application::setID(int i) {
    id = i;
}
