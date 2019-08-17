// vim: set et ts=4 sw=4:

#include "debug/output.h"
#include "debug/gdb/stub.h"
#include "device/console.h"
#include "device/keyboard.h"
#include "device/watch.h"
#include "machine/apicsystem.h"
#include "machine/lapic.h"
#include "machine/keyctrl.h"
#include "machine/ioapic.h"
#include "machine/cpu.h"
#include "syscall/guarded_scheduler.h"
#include "syscall/guarded_keyboard.h"
#include "thread/scheduler.h"
#include "thread/assassin.h"
#include "thread/idlethread.h"
#include "thread/wakeup.h"
#include "user/app1/appl.h"
#include "user/app2/kappl.h"
#include "user/status/sappl.h"
#include "user/time/cappl.h"
#include "user/time/rtc.h"

// Stack fuer max. 7 APs
static const unsigned long CPU_STACK_SIZE = 4096;
static unsigned char cpu_stack[(CPU_MAX - 1) * CPU_STACK_SIZE];

Thread *victim; // to test scheduler.kill() in user/app2

extern "C" int main() {
    // print starting stuff
    APICSystem::SystemType type = system.getSystemType();
    unsigned int numCPUs = system.getNumberOfCPUs();
    DBG_VERBOSE << "Is SMP system? " << (type == APICSystem::MP_APIC) << endl
        << "Number of CPUs: " << numCPUs << endl;

    DBG << "CPU " << (int) system.getCPUID()
        << "/LAPIC " << (int) lapic.getLAPICID() << " in main()" << endl;

    // initialize various stuff
    kout.reset();
    ioapic.init();
    keyboard.plugin();
    GDB_Stub gdb; // must be before console.listen (or IRQs are disabled)
    console.listen();
    rtc.init();
    watch.windup(1000); // 1 irq per ms
    wakeup.activate();
    assassin.hire();

    // set up idle threads
    for (int i = 0; i < CPU_MAX; i++) {
        IdleThread *idle = new IdleThread;
        scheduler.set_idle_thread(i, idle);
    }

    // set up normal applications
    int i = 0;
#if 1
    for (; i < 10; i++) {
        Application *a = new Application(i);
        Guarded_Scheduler::ready(a);
        if (i == 0) {
            victim = a;
        }
    }
#endif

    // set up special applications
    KeyboardApplication *a_kb = new KeyboardApplication(i++);
    Guarded_Scheduler::ready(a_kb);

#if 1
    StatusApplication *a_st = new StatusApplication(i++);
    Guarded_Scheduler::ready(a_st);
#endif

#if 0
    ClockApplication *a_cl = new ClockApplication(i++);
    Guarded_Scheduler::ready(a_cl);
#endif

    switch (type) {
    case APICSystem::MP_APIC: {
        //Startet die AP-Prozessoren
        for (unsigned int i = 1; i < numCPUs; i++) {
            void *startup_stack = (void *) &(cpu_stack[(i) * CPU_STACK_SIZE]);
            DBG_VERBOSE << "Booting CPU " << i << ", Stack: " << startup_stack << endl;
            system.bootCPU(i, startup_stack);
        }
        break;
    }
    case APICSystem::UP_APIC:
    case APICSystem::UNDETECTED:
        break;
    }

    CPU::enable_int();
    watch.activate();
    guard.enter();
    scheduler.schedule();

    for (;;) { // dont die
        CPU::idle();
    }
    return 0;
}

extern "C" int main_ap() {
    DBG << "CPU " << (int) system.getCPUID()
        << "/LAPIC " << (int) lapic.getLAPICID() << " in main_ap()" << endl;

    switch (system.getCPUID()) {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }

    CPU::enable_int();
    watch.activate();
    guard.enter();
    scheduler.schedule();

    // dont die
    for (;;) {
        CPU::idle();
    }
    return 0;
}
