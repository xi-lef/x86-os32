#include "thread/idlethread.h"
#include "machine/cpu.h"
#include "syscall/guarded_scheduler.h"
#include "device/watch.h"
#include "meeting/bellringer.h"
#include "debug/output.h"

void IdleThread::action() {
    for (;;) {
        CPU::disable_int();
        if (scheduler.is_empty()) {
            //DBG << "idle " << flush;
            if (system.getCPUID() != 0 || !bellringer.bell_pending()) {
                watch.block();
                CPU::idle();
                watch.unblock();
            } else {
                CPU::idle();
            }
            //DBG << "ja moin " << flush;
        } else {
            CPU::enable_int();
            Guarded_Scheduler::resume();
        }
    }
}
