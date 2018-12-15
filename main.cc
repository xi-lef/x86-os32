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
#include "user/shell/shell.h"
#include "user/time/time.h"
#include "user/time/rtc.h"
#include "machine/ioapic.h"
#include "machine/cpu.h"
#include "device/keyboard.h"
#include "machine/spinlock.h"
#include "machine/ticketlock.h"
#include "guard/secure.h"

extern CGA_Stream kout;
extern APICSystem system;

Ticketlock ticket;

static const unsigned long CPU_STACK_SIZE = 4096;
// Stack fuer max. 7 APs
static unsigned char cpu_stack[(CPU_MAX - 1) * CPU_STACK_SIZE];

//*
static void test_irq() {
    //return;
    int id = system.getCPUID();
    for (uint32_t i = 0; ; i++) {
        { Secure s;
            int x, y;
            kout.getpos(x, y);
            kout.setpos(4, id + 2);
            kout << i << flush;
            kout.setpos(x, y);
        }
    }
}
//*/

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

    kout.reset(); // ?

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

    DBG << "CPU " << (int) system.getCPUID()
        << "/LAPIC " << (int) lapic.getLAPICID() << " in main()" << endl;

    ioapic.init();
    keyboard.plugin();
    rtc.init_RTC();
    CPU::enable_int();

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
    test_irq(); // fuck g++

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

    switch (system.getCPUID()) {
        case 1:
            {
                Console serial;
                for (;;) {
                    DBG << "bla" << flush;
                    int in = serial.read(true);
                    if (in == -1) {
                        DBG << "console: invalid char" << endl;
                        continue;
                    }
                    //serial.write(in, true);
                    serial << char(in) << flush;
                }
            } break;
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
