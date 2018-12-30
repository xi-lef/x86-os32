// vim: set et ts=4 sw=4:

#include "thread/scheduler.h"
#include "guard/guard.h"
#include "guard/secure.h"
#include "debug/output.h"
#include "user/time/rtc.h"

Scheduler scheduler;

void Scheduler::schedule() {
    guard.enter();
    Thread *t = ready_list.dequeue();
    go(t);
}

void Scheduler::ready(Thread *that) {
    Secure s;
    ready_list.enqueue(that);
}

void Scheduler::exit() {
    schedule();
}

void Scheduler::kill(Thread *that) {
    Secure s;
    if (ready_list.remove(that) == 0) {
        //DBG << "Scheduler: kill: thread exec'd or 404" << endl;
        that->set_kill_flag();
    }
}

void Scheduler::resume() {
    // TODO dispatch?
    Thread *prev = active();
    if (!prev->dying()) {
        ready_list.enqueue(prev);
    } else {
        prev->reset_kill_flag();
    }

    Thread *next = ready_list.dequeue();
    set_active(next);
    prev->resume(next);
}
