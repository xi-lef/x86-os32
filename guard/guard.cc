// vim: set et ts=4 sw=4:

#include "guard/guard.h"
#include "object/queue.h"
#include "machine/ticketlock.h"
#include "machine/cpu.h"

Guard guard;
static Ticketlock bkl;
static volatile bool in_epilogue[CPU_MAX]; // initially false
static Queue<Gate> queue[CPU_MAX];

void Guard::enter() {
    CPU::disable_int(); // TODO hm
    assert(in_epilogue[system.getCPUID()] != true);
    in_epilogue[system.getCPUID()] = true;
    CPU::enable_int();
    bkl.lock();
}

void Guard::leave() {
//    int id = system.getCPUID();
    Gate *g;

    CPU::disable_int();
    while ((g = queue[system.getCPUID()].dequeue()) != 0) {
        g->set_dequeued();
        CPU::enable_int();
        g->epilogue();
        CPU::disable_int();
    }

    bkl.unlock();
    assert(in_epilogue[system.getCPUID()] != false);
    in_epilogue[system.getCPUID()] = false;
    CPU::enable_int();
}

void Guard::relay(Gate *item) {
    int id = system.getCPUID();

    if (in_epilogue[id]) {
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
