// vim: set et ts=4 sw=4:

#include "user/time/cappl.h"
#include "user/app1/appl.h"
#include "device/cgastr.h"
#include "guard/secure.h"
#include "thread/scheduler.h"
#include "user/time/rtc.h"

extern Application *opfer; // from main.cc

void Clock_Application::action() {
    //rtc.sleep(2);
    //scheduler.kill(opfer);
    for (;;) {
        //DBG << "Clock_App " << id << ": action " << flush;

        rtc.set_time();
        dout_clock.reset();
        dout_clock << rtc.get_time() << flush;
        //break;

        Secure s;
        scheduler.resume();
    }
    rtc.sleep(2);
    scheduler.ready(opfer);
    scheduler.exit();
}

void Clock_Application::setID(int i) {
    id = i;
}
