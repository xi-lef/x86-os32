// vim: set et ts=4 sw=4:

#include "guard/guard.h"
#include "object/queue.h"
#include "machine/spinlock.h"
#include "machine/ticketlock.h"
#include "machine/cpu.h"

Guard guard;
static Ticketlock bkl;
static bool in_epilogue[4] = {false, false, false, false};
static Queue<Gate> queue[4];

static void process_queue() {
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
    CPU::enable_int();
}

void Guard::enter() {
    //DBG << "entering " << flush;
    in_epilogue[system.getCPUID()] = true;
    bkl.lock();
    //DBG << "entered " << flush;
}

void Guard::leave() {
    process_queue();
    //CPU::enable_int();
    bkl.unlock();
    in_epilogue[system.getCPUID()] = false;
    //DBG << "left " << flush;
}

void Guard::relay(Gate *item) {
    int id = system.getCPUID();

    if (in_epilogue[id]) {
        //DBG << "setq " << flush;
        if (!item->set_queued()) { // TODO one queue per cpu -> set_queued cpu-wise?
            queue[id].enqueue(item);
        }
    } else {
        in_epilogue[id] = true;
        CPU::enable_int();
        enter();
        //CPU::disable_int();
        //queue[id].enqueue(item); // if set queued...
        item->epilogue(); // TODO hm
        leave();
    }
}
