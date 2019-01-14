// vim: set et ts=4 sw=4:

#include "machine/toc.h"
#include "debug/output.h"
#include "syscall/guarded_scheduler.h"

static void fail() {
    DBG << endl << "Dispatcher: bad return (kickoff) :(" << endl;
    Guarded_Scheduler::exit();
}

void toc_settle(struct toc *regs, void *tos, void (*kickoff)(Thread*), Thread *object) {
    void **esp = (void **) tos;

    regs->ebp = esp;
    *esp      = (void *) object;
    *(--esp)  = (void *) fail;
    *(--esp)  = (void *) kickoff;
    regs->esp = esp;
}
