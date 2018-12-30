// vim: set et ts=4 sw=4:

#include "machine/toc.h"
#include "thread/scheduler.h"
#include "debug/output.h"
#include "user/time/rtc.h"

static void fail() {
    DBG << endl << "Dispatcher: bad return (kickoff) :(" << endl;
    rtc.sleep(3);
    scheduler.exit();
}

void toc_settle(struct toc *regs, void *tos, void (*kickoff)(Thread*), Thread *object) {
    void **esp = (void **) tos;

    regs->ebp = esp;
    *esp      = (void *) object;
    *(--esp)  = (void *) fail;//0xbaadf00d;
    *(--esp)  = (void *) kickoff;
    regs->esp = esp;
}
