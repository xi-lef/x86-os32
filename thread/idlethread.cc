#include "thread/idlethread.h"
#include "machine/cpu.h"
#include "syscall/guarded_scheduler.h"
#include "device/watch.h"
#include "meeting/bellringer.h"
#include "debug/output.h"
#include "user/status/status.h"

void IdleThread::action() {
    for (;;) {
        CPU::disable_int();
        if (scheduler.is_empty()) {
            status.set_idle(true);
            if (!bellringer.bell_pending() || system.getCPUID() != 0) {
                watch.block();
                CPU::idle();
                watch.unblock();
            } else {
                CPU::idle();
            }
            //status.set_idle(false); is done in Scheduler::resume()
        } else {
            CPU::enable_int();
            Guarded_Scheduler::resume();
        }
    }
}
