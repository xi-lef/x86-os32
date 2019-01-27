// vim: set et ts=4 sw=4:

#include "thread/dispatcher.h"
#include "debug/output.h"
#include "guard/guard.h"
#include "machine/cpu.h"

Thread *Dispatcher::active() {
    Thread *ret = life[system.getCPUID()];
    return ret;
}

void Dispatcher::go(Thread *first) {
    if (active() != 0) {
        DBG << "Dispatcher: invalid go" << endl;
    }
    set_active(first);
    first->go();
}

void Dispatcher::dispatch(Thread *next) {
    Thread *prev = active();
    set_active(next);
    prev->resume(next);
}

void Dispatcher::kickoff(Thread *object) {
    guard.leave();

    // this function wont really return. see machine/toc.cc
    object->action();
}
