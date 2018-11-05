// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält mit den main() und main_ap() Funktionen den Startpunkt für
 *  das System
 */

/* INCLUDES */
#include "machine/apicsystem.h"
#include "machine/lapic.h"
#include "debug/output.h" // TODO ???

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
    DBG << "L" << endl;

    kout.reset();
    
    kout << "hallooo " << ((char) 1) << endl;

    //*
	kout << "Test        <stream result> -> <expected>" << endl;
	kout << "bool:       " << true << " -> true" << endl;
	kout << "zero:       " << 0 << " -> 0" << endl;
	kout << "ten:        " << (10) << " -> 10" << endl;
	kout << "uint max:   " << ~((unsigned int)0) << " -> 4294967295" << endl;
	kout << "int max:    " << ~(1<<31) << " -> 2147483647" << endl;
	kout << "int min:    " << (1<<31) << " -> -2147483648" << endl;
	kout << "some int:   " << (-123456789) << " -> -123456789" << endl;
	kout << "some int:   " << (123456789) << " -> 123456789" << endl;
	kout << "binary:     " << bin << 42 << dec << " -> 0b101010" << endl;
	kout << "octal:      " << oct << 42 << dec << " -> 052" << endl;
	kout << "hex:        " << hex << 42 << dec << " -> 0x2a" << endl;
	kout << "pointer:    " << ((void*)(3735928559u)) << " -> 0xdeadbeef" << endl;
	kout << "smiley:     " << ((char)1) << endl; 
    //*/
    for (int i = 0; i < 20; i++) {
        kout << "durchlauf " << i << endl;
    }

    CGA_Screen::Attribute a0(CGA_Screen::RED, CGA_Screen::GREEN, true);
    CGA_Screen::Attribute a1(CGA_Screen::WHITE, CGA_Screen::BLACK, true);
    
    kout.print("abc", 3, a0);

    //while (1) {}

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
    DBG << "L" << endl;
    DBG << "W" << endl;

	return 0;
}
