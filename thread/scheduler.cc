// vim: set et ts=4 sw=4:

#include "thread/scheduler.h"
#include "guard/guard.h"
#include "guard/secure.h"
#include "debug/output.h"
#include "machine/plugbox.h"
#include "machine/cpu.h"

Scheduler scheduler;

void Scheduler::schedule() {
    go(is_empty() ? idlethread[system.getCPUID()] : ready_list.dequeue());
}

void Scheduler::ready(Thread *that) {
    //DBG << "ready " << flush;
    ready_list.enqueue(that);
    system.sendCustomIPI(0xff, Plugbox::Vector::wakeup);
}

void Scheduler::exit() {
    //DBG << "exit " << flush;
    dispatch(is_empty() ? idlethread[system.getCPUID()] : ready_list.dequeue());
}

void Scheduler::kill(Thread *that) {
    if (ready_list.remove(that) != 0) {
        DBG << "Scheduler: kill: remove was successful" << endl;
        return;
    }

    // otherwise, set kill flag and send IPI to the correct CPU
    that->set_kill_flag();

    // find the CPU that is currently executing "that"
    uint8_t dest = 0;
    for (unsigned int i = 0; i < sizeof(life) / sizeof(life[0]); i++) {
        if (life[i] == that) {
            dest = i;
            break;
        }
    }

    if (dest == system.getCPUID()) {
        exit();
    } else {
        system.sendCustomIPI(system.getLogicalLAPICID(dest), Plugbox::Vector::assassin);
    }
}

void Scheduler::resume() {
    Thread *prev = active();
    if (!prev->dying() && prev != idlethread[system.getCPUID()]) { // dont queue idlethreads!
        ready_list.enqueue(prev);
    } else {
        prev->reset_kill_flag();
    }

    exit();
}

void Scheduler::block(Waitingroom *w) {
    Thread *t = active();
    w->enqueue(t);
    t->waiting_in(w);
    exit();
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
