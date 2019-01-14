// vim: set et ts=4 sw=4:

#include "thread/scheduler.h"
#include "guard/guard.h"
#include "guard/secure.h"
#include "debug/output.h"
#include "machine/plugbox.h"
#include "machine/lapic.h"

Scheduler scheduler;

void Scheduler::schedule() {
    guard.enter();
    Thread *t = ready_list.dequeue();
    go(t);
}

void Scheduler::ready(Thread *that) {
    ready_list.enqueue(that);
}

void Scheduler::exit() {
    schedule();
}

void Scheduler::kill(Thread *that) {
    if (ready_list.remove(that) != 0) {
        // remove was successful
        return;
    }

    // otherwise, set kill flag and send IPIs to all CPUs
    that->set_kill_flag();

    uint8_t dest = (1 << system.getNumberOfCPUs()) - 1; // TODO all CPUs?
    struct ICR_L data = {};
    data.vector           = Plugbox::Vector::assassin;
    data.destination_mode = DESTINATION_MODE_LOGICAL;
    data.level            = LEVEL_ASSERT;
    lapic.sendIPI(dest, data);
}

void Scheduler::resume() {
    //DBG << "Scheduler: resume " << flush;
    Thread *prev = active();
    if (!prev->dying()) {
        ready_list.enqueue(prev);
    } else {
        prev->reset_kill_flag();
    }

    Thread *next = ready_list.dequeue();
    assert(next != nullptr);
    set_active(next);
    prev->resume(next);
}
