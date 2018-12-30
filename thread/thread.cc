// vim: set et ts=4 sw=4:

#include "thread/thread.h"
#include "thread/dispatcher.h"
#include "debug/output.h"

Thread::Thread(void *tos) : killed(false) {
    toc_settle(&regs, tos, Dispatcher::kickoff, this);
}

void Thread::go() {
    toc_go(&regs);

    // go should not return here, but in Dispatcher::kickoff
    DBG << "Thread: bad return (go)" << endl;
}

void Thread::resume(Thread *next) {
    //DBG << "Thread: pre switch " << flush;
    toc_switch(&(this->regs), &(next->regs));
    //DBG << "Thread: post switch " << flush;
}

void Thread::set_kill_flag() {
    killed = true;
}

void Thread::reset_kill_flag() {
    killed = false;
}

bool Thread::dying() {
    return killed;
}
