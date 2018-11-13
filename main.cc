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

extern CGA_Stream kout;
extern APICSystem system;

static const unsigned long CPU_STACK_SIZE = 4096;
// Stack fuer max. 7 APs
static unsigned char cpu_stack[(CPU_MAX - 1) * CPU_STACK_SIZE];

/*! \brief Einsprungpunkt ins System
 *
 *  Dieser Code wird nur auf der Boot-CPU (diejenige mit der ID 0) ausgeführt.
 */

extern "C" int main()
{
	// Startmeldung ausgeben

	APICSystem::SystemType type = system.getSystemType();
	unsigned int numCPUs = system.getNumberOfCPUs();
	DBG_VERBOSE << "Is SMP system? " << (type == APICSystem::MP_APIC) << endl
	            << "Number of CPUs: " << numCPUs << endl;
	switch (type) {
		case APICSystem::MP_APIC: {
			//Startet die AP-Prozessoren
			for (unsigned int i = 1; i < numCPUs; i++) {
				void* startup_stack = (void *) &(cpu_stack[(i) * CPU_STACK_SIZE]);
				DBG_VERBOSE << "Booting CPU " << i << ", Stack: " << startup_stack << endl;
				system.bootCPU(i, startup_stack);
			}
			break;
		}
		case APICSystem::UP_APIC: {
			break;
		}
		case APICSystem::UNDETECTED: {
		}
	}

    DBG << "CPU " << (int) system.getCPUID()
        << "/LAPIC " << (int) lapic.getLAPICID() << " in main()" << endl;
    
    //char *bla = (char *) 0xb8000;
    //bla[3999] = 0b01110000;

    kout.reset();
    //*
	kout << "Test        <stream result> -> <expected>" << endl;
	kout << "bool:       " << true << " -> true" << endl;
    kout << "bool:       " << false << " -> false" << endl;
    kout << "zero:       " << 0 << " -> 0" << endl;
	kout << "ten:        " << (10) << " -> 10" << endl;
	kout << "uint max:   " << ~((unsigned int)0) << " -> 4294967295" << endl;
	kout << "int max:    " << ~(1<<31) << " -> 2147483647" << endl;
	kout << "int min:    " << (1<<31) << " -> -2147483648" << endl;
	//sleep(3);
    kout << "some int:   " << (-123456789) << " -> -123456789" << endl;
	kout << "some int:   " << (123456789) << " -> 123456789" << endl;
	kout << "binary:     " << bin << 42 << dec << " -> 0b101010" << endl;
	kout << "octal:      " << oct << 42 << dec << " -> 052" << endl;
	kout << "hex:        " << hex << 42 << dec << " -> 0x2a" << endl;
	kout << "pointer:    " << ((void*)(3735928559u)) << " -> 0xdeadbeef" << endl;
	kout << "smiley:     " << ((char)1) << endl; 
    //*/

    CGA_Screen::Attribute a0(CGA_Screen::RED, CGA_Screen::GREEN, true);
    CGA_Screen::Attribute a1(CGA_Screen::RED, CGA_Screen::GREEN, false);
    
    kout.print("bling bling", 11, a0);
    kout << endl;

    Keyboard_Controller keyboard;

    /*
    kout << "press any key to continue..." << endl;
    keyboard.key_hit();

    short a          = -123;
    unsigned short b =  234;
    int c            =  345;
    unsigned int d   =  456;
    long e           =  567;
    unsigned long f  =  678;
    
    kout << "Testing data types" << endl;
    kout << "short:          " << a << " -> -123" << endl;
    kout << "unsigned short: " << b << " ->  234" << endl;
    kout << "int:            " << c << " ->  345" << endl;
    kout << "unsigned int:   " << d << " ->  456" << endl;
    kout << "long:           " << e << " ->  567" << endl;
    kout << "unsigned long:  " << f << " ->  678" << endl;

    kout << "press any key to continue..." << endl;
    keyboard.key_hit();//*/

    int cur_speed = 0;
    int cur_delay = 0;

    for (;;) {
        Key k = keyboard.key_hit();
        if (!k.valid()) {
            continue;
        }

        //DBG << int(k.scancode()) << endl;
        if (k.ctrl()) {
            if (k.alt() && k.scancode() == Key::scan::del) {
                keyboard.reboot();
            }
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

    return 0;
}

/*! \brief Einsprungpunkt für Applikationsprozessoren
 *
 *  Code in dieser Funktion wird auf allen Applikationsprozessoren ausgeführt
 *  (entspricht allen CPUs außer derjenigen mit der ID 0).
 */
extern "C" int main_ap()
{
	DBG << "CPU " << (int) system.getCPUID()
	    << "/LAPIC " << (int) lapic.getLAPICID() << " in main_ap()" << endl;
    DBG << "hiii " << ((char) 1) << endl;

    switch (system.getCPUID()) {
        case 1: {
            //Console serial;
            } break;
        case 2: 
            dout_status.reset(' ', dout_status.attrib);
            dout_status << "coming soon^TM" << flush;
            break;
        case 3:
            clock(dout_clock);
            break;
    }

	return 0;
}
