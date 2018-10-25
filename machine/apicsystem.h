// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse APICSystem, die zum Hochfahren des
 *  Systems benötigt wird.
 */

#pragma once

#include "machine/mp_registers.h"

/// maximum number of CPUs
#define CPU_MAX 8

struct system_configuration {
	unsigned int cpu_count;
	unsigned int bsp_id;
	int ioapic_id;
	bool disablePICs;
	//speichert zu jedem Slot am IOAPIC die Legacy ISA IRQ Nummer (APICSystem::Device
	char ioapic_irqs[24];
	//Speichert zu jeder Legacy ISA IRQ Nummer(APICSystem::Device) den Slot am
	//IOAPIC, an dem das entsprechende Gerät hängt
	char slot_map[16];
	unsigned int lapic_id[CPU_MAX];
	system_configuration() : cpu_count(0), bsp_id(0), ioapic_id(-1), disablePICs(false) {
		for (int i = 0; i < 24; ++i) {
			ioapic_irqs[i] = -1;
			if(i < 16) {
				slot_map[i] = -1;
			}
		}
	}
	void pretty_print();
};

typedef struct system_configuration SystemConfig;

/*! \brief Abstraktion für die Erkennung und das Booten eines PCs mit APIC.
 */
class APICSystem
{
public:
	///Art des Systems
	enum SystemType {
		MP_APIC,		///< Multiprozessor System
		UP_APIC,		///< Singleprozessor System mit APIC HW
		UNDETECTED      ///< Unbekannte Systemkonfiguration
	};
	enum Device {
		timer = 0,
		keyboard = 1,
		com1 = 4,
		com2 = 3,
		com3 = 4,
		com4 = 3,
		floppy = 6,
		lpt1 = 7,
		rtc = 8,
		ps2mouse = 12,
		ide1 = 14,
		ide2 = 15
	};
private:
	SystemType system_type;
	unsigned int onlineCPUs; ///< running CPUs
	//unsigned int numberOfCPUs; ///< number of CPUs present in the system
	//unsigned int bspID; ///< local APIC ID of the bootstrap CPU
	//unsigned char ioapicID; ///< the ID of the detected I/O APIC, needed for programming its ID register (default: 1)
	int compatibilityMode; ///< the compatibility mode that was detected (PIC, virtual wire, symmetric I/O)
	int cpuFamily[CPU_MAX]; ///< the CPU family per CPU
	int cpuModel[CPU_MAX]; ///< the CPU model per CPU
	unsigned char cpuID[255]; ///< local CPU ID indexed with local APIC ID
	unsigned int lapicVersion[CPU_MAX]; ///< the version of the local APICs per CPU
	unsigned char logicalLAPICID[CPU_MAX]; ///< logical LAPIC IDs per CPU, needed for IPI
	volatile char callout_cpu_number;   ///< set by BSP, taken by next booting CPU
	void *relocatedSetupAP; ///< Zeiger auf den Setupcode für die Applikation
	SystemConfig mps_conf;
	SystemConfig acpi_conf;
	SystemConfig sys_conf;
public:
	APICSystem() : system_type(UNDETECTED), onlineCPUs(1),
		callout_cpu_number(0), // The boot processor has an waiting callout with id = 0
		relocatedSetupAP((void *)42) {
		// create entry point for APs
		// (setup_ap is copied to an address < 1M)
		copySetupAPtoLowMem();
	}
	/*! \brief Führt Systemerkennung aus.
	 *
	 *  Diese Funktion erkennt ein eventuell vorhandenes Mehrprozessorsystem.
	 *  Nach erfolgter Erkennung kann der Systemtyp mit Hilfe der Methode
	 *  getSystemType() abgefragt werden.
	 */
	void detectSystemType();
	/*! \brief Startet einen Applikationsprozessor.
	 *
	 *  \param cpu_id Gib an, welcher Prozessor gestartet werden soll.
	 *  Applikationsprozessoren haben die IDs 1 bis n-1
	 *  \param top_of_stack Zeiger auf die oberste Adresse des Stacks, der von
	 *  der zu startenden CPU verwendet werden soll.
	 *  \return True, falls CPU Bootup erfolgreich; sonst false.
	 */
	bool bootCPU(unsigned int cpu_id, void *top_of_stack);
	/*! \brief Liefert die CPUID der aktuellen CPU.
	 *
	 *  \return CPUID der aktuellen CPU.
	 */
	int getCPUID();
	/*! \brief Auslösen eines Interprozessorinterrupts
	 *
	 *  Mit Hilfe dieser Methode kann ein Interprozessorinterrupt (IPI) an eine
	 *  ganze Gruppe von Prozessoren geschickt werden. In MPStuBS ist das System
	 *  derart konfiguriert, dass insgesamt 8 CPUs bzw. deren Local APICs
	 *  angesprochen werden können. Jeder Local APIC wird dabei durch ein Bit
	 *  in \a logicalDestination repräsentiert. Dabei entspricht ein gesetztes
	 *  Bit 0 der logischen LAPIC-ID der CPU 0, Bit 1 der ID der CPU 1 usw..
	 *  Welcher Interruptvektor auf den Ziel-CPUs ausgelöst werden soll, wird
	 *  durch \a vector festgelegt. Zum konkreten Absenden eines IPIs wird die
	 *  Methode LAPIC::sendIPI() verwendet.
	 *
	 *  \param logicalDestination Menge von Prozessoren, die einen IPI erhalten
	 *  sollen.
	 *  \param vector Interruptvektor, der ausgelöst werden soll.
	 */
	void sendCustomIPI(unsigned char logicalDestination, unsigned char vector);

	/*! \brief Liefert die Nummer des Pins, an dem das Gerät device hängt
	 *
	 */
	unsigned char getIOAPICSlot(APICSystem::Device device);
	const char *getCPUModel(unsigned int cpuID);
	/*! \brief Abfrage des Systemtyps
	 *
	 *  \return Gibt den Systemtyp zurück
	 */
	SystemType getSystemType() {
		return system_type;
	}
	/*! \brief Gibt die Anzahl der CPUs zurück
	 *
	 */
	unsigned int getNumberOfCPUs() {
		return sys_conf.cpu_count;
	}
	/*! \brief Gibt die Anzahl der erfolgreich gebooteten CPUs zurück
	 *
	 */
	unsigned int getNumberOfOnlineCPUs() {
		return onlineCPUs;
	}
	/*! \brief Liefert die während des Bootvorgangs ausgelesene ID des IOAPICs
	 *
	 */
	unsigned char getIOAPICID() {
		return sys_conf.ioapic_id;
	}
	/// Liefert die ID des Bootprozessors (BSP)
	unsigned char getBSPID() {
		return sys_conf.bsp_id;
	}

	void setupThisProcessor();

	/*! \brief Liefert die logische ID des LAPICs der übergebenen \a cpu.
	 *
	 *  Die logische ID der LAPICs wird von uns beim Boot-Vorgang so
	 *  initialisiert, dass genau ein Bit pro CPU gesetzt ist. Dabei ist in der
	 *  logischen ID des LAPICs von CPU 0 Bit 0 gesetzt, in der ID des LAPICs
	 *  von CPU 1 Bit 1, usw.
	 *
	 *  \param cpu Die abzufragende CPU
	 */
	unsigned char getLogicalLAPICID(unsigned char cpu) {
		return logicalLAPICID[cpu];
	}
private:
	void queryACPI(SystemConfig& conf); //use ACPI to detect SMP system
	bool mpbChecksumOK(unsigned char *start_address, int len);
	void readMPCT(struct mpcth *mpcth, SystemConfig& conf);
	bool searchConfig(unsigned long base, unsigned long length, SystemConfig& conf);
	void delay(unsigned int wraparounds);
	void copySetupAPtoLowMem();
	void disablePICs(); //disable PIC compatibility mode
	// finishes IRQ Map initalisation after ACPI/MPS detection
	void generateIRQMap(char ioapic_irqs[], char slot_map[]);

};

// global object declaration
extern APICSystem system;

