// vim: set et ts=4 sw=4:

#include "thread/thread.h"
#include "thread/dispatcher.h"
#include "debug/output.h"
#include "user/mutex/mutex.h"

Thread::Thread(void *tos) : waitingroom(0), killed(false) {
    toc_settle(&regs, tos, Dispatcher::kickoff, this);
}

void Thread::go() {
    toc_go(&regs);

    // go should not return here, but in Dispatcher::kickoff
    DBG << "Thread: bad return (from toc_go)" << endl;
}

void Thread::resume(Thread *next) {
    toc_switch(&(this->regs), &(next->regs));
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

Waitingroom *Thread::waiting_in() {
    return waitingroom;
}

void Thread::waiting_in(Waitingroom *w) {
    waitingroom = w;
}

void Thread::mutex_hold(Mutex *m) {
    mutex_list.enqueue(m);
}

bool Thread::mutex_release(Mutex *m) {
    return mutex_list.remove(m);
}

// must be called from epilogue-level!
// (or: if e.g. kill and exit at same time, v() would called twice)
bool Thread::mutex_release_all() {
    bool ret = true;
    for (Mutex *m : mutex_list) {
        ret &= m->unlock();
    }
    return ret;
}
