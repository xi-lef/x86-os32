// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält mit den main() und main_ap() Funktionen den Startpunkt für
 *  das System
 */

/* INCLUDES */
#include "machine/apicsystem.h"
#include "machine/lapic.h"
#include "debug/output.h"
#include "machine/keyctrl.h"
#include "device/console.h"
#include "user/time/rtc.h"
#include "machine/ioapic.h"
#include "machine/cpu.h"
#include "device/keyboard.h"
#include "guard/secure.h"
#include "user/app1/appl.h"
#include "user/time/cappl.h"
#include "thread/scheduler.h"
#include "syscall/guarded_scheduler.h"
#include "device/watch.h"

extern CGA_Stream kout;
extern APICSystem system;

// Stack fuer max. 7 APs
static const unsigned long CPU_STACK_SIZE = 4096;
static unsigned char cpu_stack[(CPU_MAX - 1) * CPU_STACK_SIZE];

static const unsigned long APP_STACK_SIZE = 4096;
static unsigned char app_stack[2 * CPU_MAX * APP_STACK_SIZE];

    Application a0(&(app_stack[1 * APP_STACK_SIZE - 4]), 0);
    Application a1(&(app_stack[2 * APP_STACK_SIZE - 4]), 1);
    Application a2(&(app_stack[3 * APP_STACK_SIZE - 4]), 2);
    Application a3(&(app_stack[4 * APP_STACK_SIZE - 4]), 3);
    Application a4(&(app_stack[5 * APP_STACK_SIZE - 4]), 4);
    Application a5(&(app_stack[6 * APP_STACK_SIZE - 4]), 5);
    Application a6(&(app_stack[7 * APP_STACK_SIZE - 4]), 6);
    Application a7(&(app_stack[8 * APP_STACK_SIZE - 4]), 7);
    Application a8(&(app_stack[9 * APP_STACK_SIZE - 4]), 8);
    Application a9(&(app_stack[10 * APP_STACK_SIZE - 4]), 9);

static void test_irq() {
    return;
    int id = system.getCPUID();
    for (uint32_t i = 0; ; i++) {
        Secure s; // to sync for-body
        int x, y;
        kout.getpos(x, y);
        kout.setpos(4, id + 2);
        kout << i << flush;
        kout.setpos(x, y);
    }
}

/*! \brief Einsprungpunkt ins System
 *
 *  Dieser Code wird nur auf der Boot-CPU (diejenige mit der ID 0) ausgeführt.
 */
extern "C" int main() {
    // Startmeldung ausgeben

    APICSystem::SystemType type = system.getSystemType();
    unsigned int numCPUs = system.getNumberOfCPUs();
    DBG_VERBOSE << "Is SMP system? " << (type == APICSystem::MP_APIC) << endl
        << "Number of CPUs: " << numCPUs << endl;

    DBG << "CPU " << (int) system.getCPUID()
        << "/LAPIC " << (int) lapic.getLAPICID() << " in main()" << endl;

    kout.reset();

    //DBG << "lapic freq (per ms): " << lapic.timer_ticks() << endl;
    watch.windup(1 * 1000);

    //*
    //Clock_Application a10(&(app_stack[11 * APP_STACK_SIZE - 4]), 10);
    //Clock_Application a11(&(app_stack[12 * APP_STACK_SIZE - 4]), 11);

    Guarded_Scheduler::ready(&a0);
    Guarded_Scheduler::ready(&a1);
    Guarded_Scheduler::ready(&a2);
    Guarded_Scheduler::ready(&a3);
    Guarded_Scheduler::ready(&a4);
    Guarded_Scheduler::ready(&a5);
    Guarded_Scheduler::ready(&a6);
    Guarded_Scheduler::ready(&a7);
    Guarded_Scheduler::ready(&a8);
    Guarded_Scheduler::ready(&a9);
    //Guarded_Scheduler::ready(&a10);
    //Guarded_Scheduler::ready(&a11);
    //*/

    switch (type) {
        case APICSystem::MP_APIC:
            {
                //Startet die AP-Prozessoren
                for (unsigned int i = 1; i < numCPUs; i++) {
                    void* startup_stack = (void *) &(cpu_stack[(i) * CPU_STACK_SIZE]);
                    DBG_VERBOSE << "Booting CPU " << i << ", Stack: " << startup_stack << endl;
                    system.bootCPU(i, startup_stack);
                }
                break;
            }
        case APICSystem::UP_APIC:
            break;
        case APICSystem::UNDETECTED:
            {}
    }

    //*
    ioapic.init();
    keyboard.plugin();
    //console.listen();
    //rtc.init_RTC();
    
    CPU::enable_int();
    watch.activate();
    //*/

    scheduler.schedule();

    test_irq();

    /*
    for (int cur_speed = 0, cur_delay = 0; ; ) {
        Key k = keyboard.key_hit();
        if (!k.valid()) {
            continue;
        }

        //DBG << int(k.scancode()) << endl;
        if (k.ctrl()) {
            if (k.ascii() == 't') {
                Shell shell(kout, keyboard);
                shell.start();
            }
            switch (k.scancode()) {
                case Key::scan::up:
                    keyboard.set_repeat_rate((cur_speed == 31) ? 31 : ++cur_speed, cur_delay);
                    break;
                case Key::scan::down:
                    keyboard.set_repeat_rate((cur_speed == 0)  ?  0 : --cur_speed, cur_delay);
                    break;
            }
        } else if (k.alt()) {
            switch (k.scancode()) {
                case Key::scan::up:
                    keyboard.set_repeat_rate(cur_speed, (cur_delay == 3) ? 3 : ++cur_delay);
                    break;
                case Key::scan::down:
                    keyboard.set_repeat_rate(cur_speed, (cur_delay == 0) ? 0 : --cur_delay);
                    break;
            }
        } else {
            kout << k.ascii();
            kout << flush;
        }
    }
    //*/

    for (;;) { // dont die
        CPU::idle();
    }

    return 0;
}

/*! \brief Einsprungpunkt für Applikationsprozessoren
 *
 *  Code in dieser Funktion wird auf allen Applikationsprozessoren ausgeführt
 *  (entspricht allen CPUs außer derjenigen mit der ID 0).
 */
extern "C" int main_ap() {
    DBG << "CPU " << (int) system.getCPUID()
        << "/LAPIC " << (int) lapic.getLAPICID() << " in main_ap()" << endl;

    CPU::enable_int();
    watch.activate();

    //rtc.sleep(5);
    scheduler.schedule();

    switch (system.getCPUID()) {
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
    }

    test_irq();

    // dont die
    for (;;) {
        CPU::idle();
    }

    return 0;
}
