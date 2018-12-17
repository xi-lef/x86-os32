// vim: set et ts=4 sw=4:

#include "guard/guard.h"
#include "object/queue.h"
#include "machine/spinlock.h"
#include "machine/ticketlock.h"
#include "machine/cpu.h"

Guard guard;
static Ticketlock bkl;
static volatile bool in_epilogue[CPU_MAX] = {};
static Queue<Gate> queue[CPU_MAX];

void Guard::enter() {
    //DBG << "entering " << flush;
    in_epilogue[system.getCPUID()] = true;
    bkl.lock();
    //DBG << "entered " << flush;
}

void Guard::leave() {
    int id = system.getCPUID();
    Gate *g;
    CPU::disable_int();
    while ((g = queue[id].dequeue()) != 0) {
        //DBG << "setdeq " << g << " " << flush;
        g->set_dequeued();
        CPU::enable_int();
        g->epilogue();
        CPU::disable_int();
    }

    bkl.unlock();
    in_epilogue[system.getCPUID()] = false;
    CPU::enable_int();

    //DBG << "left " << flush;
}

void Guard::relay(Gate *item) {
    int id = system.getCPUID();

    if (in_epilogue[id]) {
        //DBG << "setq " << flush;
        if (!item->set_queued()) {
            queue[id].enqueue(item);
        }
    } else {
        in_epilogue[id] = true;
        CPU::enable_int();
        bkl.lock();
        item->epilogue();
        leave();
    }
}
