// vim: set et ts=4 sw=4:

#include "thread/dispatcher.h"
#include "debug/output.h"
#include "guard/guard.h"

Thread *Dispatcher::active() {
    return life[system.getCPUID()];
}

void Dispatcher::go(Thread *first) {
    if (active() != 0) {
        DBG << "Dispatcher: invalid go " << flush;
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
    //DBG << "kickoff " << flush;
    guard.leave();
    object->action();
    //DBG << "Dispatcher: bad return (kickoff) :(" << endl;
    //for (;;) ; // dont return
}
