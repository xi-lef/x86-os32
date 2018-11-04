// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält mit den main() und main_ap() Funktionen den Startpunkt für
 *  das System
 */

/* INCLUDES */
#include "machine/apicsystem.h"
#include "machine/lapic.h"
#include "debug/output.h" // TODO ???

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

	return 0;
}

/*! \brief Einsprungpunkt für Applikationsprozessoren
 *
 *  Code in dieser Funktion wird auf allen Applikationsprozessoren ausgeführt
 *  (entspricht allen CPUs außer derjenigen mit der ID 0).
 */
extern "C" int main_ap()
{
	DBG_VERBOSE << "CPU " << (int) system.getCPUID()
	            << "/LAPIC " << (int) lapic.getLAPICID() << " in main_ap()" << endl;

	return 0;
}
