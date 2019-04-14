// vim: set et ts=4 sw=4:

#include "thread/scheduler.h"
#include "guard/guard.h"
#include "guard/secure.h"
#include "debug/output.h"
#include "machine/plugbox.h"
#include "machine/cpu.h"
#include "user/status/status.h"
#include "meeting/bellringer.h"

Scheduler scheduler;

void Scheduler::dispatch_next() {
    dispatch(is_empty() ? idlethread[system.getCPUID()] : ready_list.dequeue());
}

void Scheduler::schedule() {
    go(is_empty() ? idlethread[system.getCPUID()] : ready_list.dequeue());
}

void Scheduler::ready(Thread *that) {
    ready_list.enqueue(that);
    system.sendCustomIPI((1 << system.getNumberOfOnlineCPUs()) - 1, Plugbox::Vector::wakeup);
}

void Scheduler::exit() {
    dispatch_next();
}

void Scheduler::kill(Thread *that) {
    // check ready_list for "that"
    if (ready_list.remove(that) != 0) {
        DBG << "Scheduler: kill: was in ready_list" << endl;
        that->Thread::~Thread();
        return;
    }

    // check if "that" is in a waitingroom
    if (Waitingroom *w = that->waiting_in()) {
        DBG << "Scheduler: kill: was in waitingroom" << endl;
        w->remove(that);
        //that->waiting_in(nullptr);
        that->Thread::~Thread();
        return;
    }

    // otherwise, set kill flag and send IPI to the correct CPU
    that->set_kill_flag();

    // find the CPU that is currently executing "that"
    uint8_t dest = 255;
    for (unsigned int i = 0; i < sizeof(life) / sizeof(life[0]); i++) {
        if (life[i] == that) {
            dest = i;
            break;
        }
    }
    if (dest == 255) {
        DBG << "Scheduler: kill: you fucked up" << endl;
        return;
    }

    if (dest == system.getCPUID()) {
        that->Thread::~Thread();
        dispatch_next();
    } else {
        DBG << "Scheduler: kill: IPI to " << (int) dest << endl;
        system.sendCustomIPI(system.getLogicalLAPICID(dest), Plugbox::Vector::assassin);
    }
}

void Scheduler::resume() {
    Thread *prev = active();
    if (prev->dying()) {
        //prev->reset_kill_flag();
        prev->Thread::~Thread();
    } else {
        // dont queue idlethreads! but update the status correctly.
        if (prev != idlethread[system.getCPUID()]) {
            ready_list.enqueue(prev);
        } else {
            status.set_idle(false);
        }
    }

    dispatch_next();
}

void Scheduler::block(Waitingroom *w) {
    Thread *t = active();
    w->enqueue(t);
    t->waiting_in(w);
    dispatch_next();
}

bool Scheduler::is_empty() {
    return ready_list.first() == nullptr;
}

void Scheduler::set_idle_thread(int cpuid, Thread *thread) {
    idlethread[cpuid] = thread;
}

void Scheduler::wakeup(Thread *customer) {
    customer->waiting_in()->remove(customer);
    customer->waiting_in(nullptr);
    ready(customer);
}
