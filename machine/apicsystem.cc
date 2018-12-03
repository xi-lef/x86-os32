// vim: set et ts=4 sw=4:

#include "machine/apicsystem.h"
#include "device/cgastr.h"
#include "machine/io_port.h"
#include "machine/lapic.h"
#include "machine/ioapic.h"
#include "debug/output.h"
#include "machine/acpi.h"

// global object definition
APICSystem system;

void system_configuration::pretty_print()
{
	//kout << "# cpus: " << cpu_count << " bsp_id: " << bsp_id
	//	<< " ioapic_id: " << ioapic_id << " disPICs: " << disablePICs << endl;
	//kout << "IOAPIC IRQMap: ";
	for(int i = 0; i < 24; ++i) {
		//kout << (int) ioapic_irqs[i] << ", ";
	}
	//kout << endl;
	//kout << "ISA Devices IRQMap: ";
	for(int i = 0; i < 16; ++i) {
		//kout << (int) slot_map[i] << ", ";
	}
	//kout << endl;
}
void APICSystem::detectSystemType()
{
	// System base memory
	unsigned short memsize = *((unsigned short *) (0x413));
#ifdef VERBOSE
	DBG << "memsize is " << dec << memsize << endl;
#endif

	/*
	 * The MPFP structure can be located in one of three different memory areas:
	 * 1) In the upper 1K of the EBDA (BIOS data area 0x40:0e holds segment of EBDA)
	 * 2) In the upper 1K of the base RAM (639K-640K or 511K-512K)
	 * 3) In the BIOS ROM (0x0f0000-0x0fffff)
	 */

	if (searchConfig(*((unsigned short *) 0x40e) << 4, 1024, mps_conf) ||
			searchConfig(memsize * 1024, 1024, mps_conf) ||
			searchConfig(511 * 1024, 1024, mps_conf) ||
			searchConfig(639 * 1024, 1024, mps_conf) ||
			searchConfig(0x000f0000, 0x000fffff - 0x000f0000 + 1, mps_conf)) {
	}
	/*
	 * We may have processors with hyperthreading installed,
	 * which will only be recognized using ACPI.
	 */
	queryACPI(acpi_conf);
	generateIRQMap(acpi_conf.ioapic_irqs, acpi_conf.slot_map);

#ifdef VERBOSE
	DBG << "MPS Configuration: " << endl;
	mps_conf.pretty_print();
	DBG << "ACPI Configuration: " << endl;
	acpi_conf.pretty_print();
#endif

	if (acpi_conf.cpu_count >= mps_conf.cpu_count) {
		sys_conf = &acpi_conf;
	} else {
		sys_conf = &mps_conf;
	}
	if (sys_conf->disablePICs) {
		disablePICs();
	}
	if (sys_conf->cpu_count > 1) {
		system_type = MP_APIC;
	} else {
		if (sys_conf->ioapic_id < 0) {
			system_type = UNDETECTED;
		}
		system_type = UP_APIC;
	}
	if (system_type != MP_APIC) {
#ifdef VERBOSE
		DBG << "SMP Detection using ACPI: FAIL" << endl
		    << "Did not detect SMP system." << endl;
#endif
		sys_conf->cpu_count = 1;
		onlineCPUs = 1;
	}
	return;
}

void APICSystem::queryACPI(SystemConfig& conf)
{
	using namespace ACPI_Subsystem;

	RSDP *rsdp = 0; //Root System Description Pointer
	RSDT *rsdt = 0; //Root System Description Table
	//XSDT *xsdt = 0; //Xtended System Description Table
	MADT *madt = 0; //Multiple APIC Description Table
	int rsdt_num_entries; //Number of SDT pointers in RSDT
	//int xsdt_num_entries; //Number of SDT pointers in XSDT
	int idx;

	/*
	 * look for the root system description pointer
	 */
	if ( !( rsdp = RSDP::find() ) ) {
		DBG_VERBOSE << "No ACPI..." << endl;
		return;
	}

	/*
	 * look for the root system description table
	 */
	if ( !( rsdt = rsdp->getRSDTPtr() ) ) {
		DBG_VERBOSE << "No ACPI Root System Description Table :-(" << endl;
	}

	/*
	 * parse the root system description table and search for the MADT
	 */
	rsdt_num_entries = rsdt->getNumSDTPtrs();
	DBG_VERBOSE << "RSDT contains " << rsdt_num_entries
	            << " SDT pointers" << endl;

	for ( idx = 0; idx < rsdt_num_entries; idx++) {
		System_Description_Table *entry = rsdt->getSDTPtr( idx );

		DBG_VERBOSE << "SDT pointer" << idx << ": type "
		            << entry->getSignature()[0] << entry->getSignature()[1]
		            << entry->getSignature()[2] << entry->getSignature()[3]
		            << endl;

		if ( entry->getType() == System_Description_Table::MADT ) {
			DBG_VERBOSE << "MADT found (yay) !!!" << endl;
			madt = (MADT *) entry;
		}
	}

	/*
	 * no MADT found -> give up
	 */
	if ( !madt ) {
		DBG_VERBOSE << "No MADT Ptr found in RSDT" << endl;
		if ( rsdp->is_ACPI_1_0()) {
			DBG_VERBOSE << "   and no XSDT given (ACPI 1.0)" << endl
			            << "   -> giving up" << endl;
		} else {
			DBG_VERBOSE << "   and XSDT (ACPI>1.0) not supported yet"
			            << endl
			            << "   PLEASE TELL THE BS(B) STAFF!" << endl;
		}
		return;
	}

	/*
	 * parse the madt
	 */
	if ( madt->has_PCAT_COMPAT() ) {
		conf.disablePICs = true;
	}

	{
		APIC_Structure *entry = madt->getFirstAPICStructurePtr();
		APIC_Structure *end   = madt->getRearPtr();
		bool found_bsp = false;
		idx=0;

		DBG_VERBOSE << "MADT begins at" << (void *) madt << endl;
		DBG_VERBOSE << "APIC Structures begin at" << (void *) entry << endl;
		DBG_VERBOSE << "MADT ends at" << (void *) end << endl;

		while ( entry < end ) {
			int entry_type = entry->getType();

			DBG_VERBOSE << "APIC Structure " << idx++ << " at " << (void *) entry;
			DBG_VERBOSE << " type " << entry_type << endl;

			if ( entry->getLength() == 0 ) {
				DBG_VERBOSE << "found zero length APIC Structure in MADT... WTF?" << endl;
				break;
			}

			switch ( entry_type ) {
					/*
					 * found a LAPIC
					 */
				case APIC_Structure::LOCAL_APIC: {
					Local_APIC_Structure *lapic =
						(Local_APIC_Structure *) entry;
					unsigned char lapic_id;
					unsigned char current_cpu;

					if ( !lapic->isEnabled() )
						break;

					if ( conf.cpu_count == CPU_MAX )
						break;

					/* CPU IDs are hard. We therefore use surrogate
					 * CPU IDs. We furthermore assume that lapic_id=0
					 * is our boot CPU. */
					lapic_id = lapic->getAPIC_ID();
					current_cpu = conf.cpu_count;
					// We deliberately ignore the CPU ID value from Local_APIC_Structure

					DBG_VERBOSE << "detected APIC with ID "
					            << (int) lapic_id
					            << " and corresponding CPU with ID "
					            << (int) current_cpu << endl;

					if ( current_cpu >= CPU_MAX ) {
						DBG_VERBOSE << "detected CPU ID >= max supported #CPUs" << endl;
						break;
					}

					conf.cpu_count++;

					conf.lapic_id[current_cpu] = lapic_id;
					//unkown: lapicVersion,	cpuFamily, cpuModel

					//according to the spec (5.2.11.4.1)
					if ( !found_bsp ) {
						conf.bsp_id = lapic_id;
						DBG_VERBOSE << "bspID is " << conf.bsp_id << endl;
						//as done in readMPCT and checked for in bootCPU
						found_bsp = true;
					}

					break;
				}
				/*
				 * found an IO APIC
				 */
				case APIC_Structure::IO_APIC: {
					IO_APIC_Structure *io_apic =
						(IO_APIC_Structure *) entry;

					conf.ioapic_id = io_apic->getAPIC_ID();
					DBG_VERBOSE << "detected IO APIC with ID "
					            << (int) conf.ioapic_id << endl;
					break;
				}
				case APIC_Structure::SOURCE_OVERRIDE: {
					Int_Source_Override_Structure* override =
						(Int_Source_Override_Structure *) entry;
					if(override->getBus() != 0) {
						DBG_VERBOSE << "Found override for bus != ISA. Does not conform to ACPI." << endl;
						break;
					}
					DBG_VERBOSE << "source override: remapping IRQ" << (int) override->getSource()
					            << " to IRQ" << (int) override->getGlobalSystemInterrupt() << endl;
					conf.ioapic_irqs[override->getGlobalSystemInterrupt()] = override->getSource();
					break;
				}
			}

			entry = entry->getNextAPICStructurePtr();
		}
	}

	if ( !rsdp->is_ACPI_1_0() )	{
		DBG_VERBOSE << "WARNING: ACPI > 1.0 detected, but will not parse XSDT" << endl;
	}
	return;
}

void APICSystem::generateIRQMap(char ioapic_irqs[], char slot_map[])
{
	//All non remapped IRQs are configured like legacy PIC layout
	for(int i = 0; i < 16; ++i) {
		if(ioapic_irqs[i] == -1) {
			ioapic_irqs[i] = i;
			continue;
		}
	}
	for(int i = 0; i < 24; ++i) {
		char device = ioapic_irqs[i];
		if (device >= 0 && device < 16) {
			slot_map[(int)device] = i;
		}
	}
}

unsigned char APICSystem::getIOAPICSlot(APICSystem::Device device)
{
	return sys_conf->slot_map[device];
}

int APICSystem::getCPUID()
{
	if (system_type == UNDETECTED) {
		return 0;
	} else {
		return cpuID[lapic.getLAPICID()];
	}
}

bool APICSystem::searchConfig(unsigned long base, unsigned long length, SystemConfig& conf)
{
	struct mpfps *mpfps;

	DBG_VERBOSE << "searching MPFP structure starting at " << hex << base << " (length=" << dec << length << ")" << endl;

	// structure is 16 bytes long and beginning on a 16-byte boundary
	unsigned long *bp = (unsigned long *) (base & ~0x0000000f);

	for ( ; length > 0; bp += 4, length -= 16) {
		if (*bp != MPFPS_ID_SIGNATURE) {
			continue;
		}

		mpfps = (struct mpfps *) bp;

		// length reasonable?
		if (mpfps->length != 1) {
			DBG_VERBOSE << "invalid size (16*" << (int)mpfps->length << ")" << endl;
			continue;

			// checksum OK?
		} else if (!mpbChecksumOK((unsigned char *) bp, mpfps->length * sizeof(struct mpfps))) {
			DBG_VERBOSE << "invalid checksum" << endl;
			continue;

			// valid version of the MP spec?
		} else if (mpfps->spec_rev != 1 && mpfps->spec_rev != 4) {
			DBG_VERBOSE << "unknown MPFS spec_rev = " << hex << (int)mpfps->spec_rev << dec << endl;
			continue;
		}

		// extract compatibility mode from structure
		compatibilityMode = mpfps->feature2 & COMPATIBILITY_PIC;

		// switch to symmetric I/O if necessary
		if (compatibilityMode == COMPATIBILITY_PIC) {
			conf.disablePICs = true;
		}

		// check for standard configuration
		if (mpfps->feature1 != 0) {
			// standard configuration
			DBG_VERBOSE << "Standard configuration detected." << endl;
		} else if (mpfps->physptr != 0x0) {
			// there is an MPCT
			DBG_VERBOSE << "MPCT detected." << endl;
			readMPCT((struct mpcth *) mpfps->physptr, conf);
		}

		// the configuration that was found first is used
		/* In the P4 case, we should return false, in order
		   to give the ACPI SMP detection a chance */
		return (conf.cpu_count > 1);
	}

	return false; // no signature was found
}

bool APICSystem::mpbChecksumOK(unsigned char *start_address, int len)
{
	int sum = 0;
	while (len--) {
		sum += *start_address++;
	}
	if ((sum & 0xff) == 0) {
		return true;
	} else {
		return false;
	}
}

void APICSystem::readMPCT(struct mpcth *mpcth, SystemConfig& conf)
{
	int count = sizeof(*mpcth);
	unsigned char *mpt = ((unsigned char *) mpcth) + count;	// pointer to table

	// check signature "PCMP"
	if (((mpcth->signature)[0] != 'P') ||
			((mpcth->signature)[1] != 'C') ||
			((mpcth->signature)[2] != 'M') ||
			((mpcth->signature)[3] != 'P')) {
		// wrong signature
		return;
	}

	// check checksum
	if (! mpbChecksumOK((unsigned char *) mpcth, mpcth->length)) {
		// wrong checksum
		return ;
	}
	// initialize slot_map with ISA legacy interrupts
	conf.slot_map[0] = 2;
	conf.slot_map[1] = 1;
	conf.slot_map[2] = -1;
	for (int i = 3; i < 16; ++i) {
		conf.slot_map[i]=i;
	}
	// investigate configuration blocks
	// loop while less bytes (count) have been read than the MPCT is long (length)
	while (count < mpcth->length) {
		switch (*mpt) {
			case MPCT_PROCESSOR: {
				struct mpct_processor *m = (struct mpct_processor *) mpt;
				if (m->cpuflag & CPU_ENABLED) {
					conf.cpu_count++;
					if (m->cpuflag & CPU_BOOTPROCESSOR) {
						// ID of the bootstrap CPU
						conf.bsp_id = m->lapicid;
					}

					if (conf.cpu_count <= CPU_MAX) {
						int current = conf.cpu_count - 1;
						conf.lapic_id[current] = m->lapicid;
						lapicVersion[current] = m->lapicver;
						// save family and model
						cpuFamily[current] = (m->cpusignature & 0xf00) >> 8;
						cpuModel[current] = (m->cpusignature & 0xf0) >> 4;
					}
				}
				mpt += sizeof(*m); // set mpt to address after MPT_PROCESSOR structure
				count += sizeof(*m); // increase count by number of read bytes
				DBG_VERBOSE << "found processor with lapicid " << (int) m->lapicid << " CPU "
				            << ((m->cpuflag & CPU_ENABLED) ? ((char*) " enabled") : ((char*) " disabled")) << endl;
				break;
			}
			case MPCT_IOAPIC: {
				struct mpct_ioapic *m = (struct mpct_ioapic *) mpt;
				mpt += sizeof(*m);
				count += sizeof(*m);
				// save I/O-APIC ID for later initialization
				conf.ioapic_id = m->apicid;
				DBG_VERBOSE << "found IOAPIC with ID " << (int) conf.ioapic_id << endl;
				break;
			}
			// the pieces of information from the other structures are not important for us
			case MPCT_BUS: {
				struct mpct_bus *m = (struct mpct_bus *) mpt;
				mpt += sizeof(*m);
				count += sizeof(*m);
				break;
			}
			case MPCT_IOINT: {
				struct mpct_int *m = (struct mpct_int *) mpt;
				mpt += sizeof(*m);
				count += sizeof(*m);
				if (m->srcbus == 0) {
					DBG_VERBOSE << "ie type " << (int) m->irqtype
					            << " src bus id:" << (int) m->srcbus
					            << " src bus irq:" << (int) m->srcbusirq
					            << " dapicid:" << (int) m->dstapic
					            << " dstirq: " << (int) m->dstirq;
					if (m->srcbusirq < 16 && m->dstirq < 24) {
						conf.ioapic_irqs[m->dstirq] = m->srcbusirq;
						conf.slot_map[m->srcbusirq] = m->dstirq;
					} else {
						DBG_VERBOSE << " not mappable";
					}
					DBG_VERBOSE << endl;
				}
				break;
			}
			case MPCT_LINT: {
				struct mpct_int *m = (struct mpct_int *) mpt;
				mpt += sizeof(*m);
				count += sizeof(*m);
				if (m->srcbus == 0) {
					DBG_VERBOSE << "ie type " << (int) m->irqtype
					            << " src bus id:" << (int) m->srcbus
					            << " src bus irq:" << (int) m->srcbusirq
					            << " dapicid:" << (int) m->dstapic
					            << " dstirq: " << (int) m->dstirq;
					if (m->srcbusirq < 16 && m->dstirq < 24) {
						//	conf.ioapic_irqs[m->dstirq] = m->srcbusirq;
						//	conf.slot_map[m->srcbusirq] = m->dstirq;
						DBG_VERBOSE << " not mappable";
					}
					DBG_VERBOSE << endl;
				}
				break;
			}
		}
	}
}

const char *APICSystem::getCPUModel(unsigned int cpuID)
{
	int family = cpuFamily[cpuID];
	int model = cpuModel[cpuID];
	static char *model_defs[] = {
		"80486DX", "80486DX",
		"80486SX", "80486DX/2 or 80487",
		"80486SL", "Intel5X2(tm)",
		"Unknown", "Unknown",
		"80486DX/4"
	};
	if (family == 0x04 && model < 9) // 100
		return model_defs[model];
	else if (family == 0x5) { // 101
			return model == 4 || model == 8 ? "Pentium with MMX technology" : "Pentium";
	} else if (family == 0x6) { // 110
		// TODO: AMD
		switch(model){
			case 0x01: return "Pentium Pro";
			case 0x03: return "Pentium II (model 3)";
			case 0x05: return "Pentium II (model 5) or Celeron";
			case 0x06: return "Mobile Pentium II or Celeron";
			case 0x07: return "Pentium III (model 7)";
			case 0x08: return "Pentium III (model 8) or Celeron";
			case 0x15:
			case 0x0D:
			case 0x09: return "Pentium M";
			case 0x0A: return "Pentium III Xeon (model A)";
			case 0x0B: return "Mobile Pentium III or Celeron";
			case 0x0E: return "Core (Yonah)";
			case 0x16:
			case 0x17:
			case 0x0F:
			case 0x1D: return "Core 2";
			case 0x1E:
			case 0x1F:
			case 0x1A:
			case 0x2E: return "Core-i (Nehalm)";
			case 0x2A:
			case 0x2D: return "Core-i (Sandy Bridge)";
			case 0x25:
			case 0x2C:
			case 0x2F: return "Core-i (Westmere)";
			case 0x3E:
			case 0x3A: return "Core-i (Ivy Bridge)";
			case 0x3C:
			case 0x3F:
			case 0x45:
			case 0x46: return "Core-i (Haswell)";
			case 0x3D: return "Core-i (Broadwell)";
			case 0x4A: return "Intel Edison";
			case 0x1C:
			case 0x26:
			case 0x27:
			case 0x35:
			case 0x5A:
			case 0x5D:
			case 0x36: return "Atom";
			case 0x37:
			case 0x4D: return "Atom or Pentium or Celeron";
			default: return ("P6 family");
		}
	} else if (family == 0x0F)	// 111
		switch(model){
			case 0x03:
			case 0x04: return "Pentium 4/Xeon";
			case 0x06: return "Pentium 4";
			case 0x0F: return ("Special controller");
			default: return "Unknown Intel CPU";
		}
	else
		return ("Unknown CPU");
}

void APICSystem::delay(unsigned int wraparounds)
{
	for (unsigned int i = 0; i < wraparounds; i++) {
		unsigned int curr_count, prev_count = ~0;
		int delta;

		IO_Port port1(0x43);
		IO_Port port2(0x40);
		port1.outb(0x00);
		curr_count = port2.inb();
		curr_count |= port2.inb() << 8;

		do {
			prev_count = curr_count;
			port1.outb(0x00);
			curr_count = port2.inb();
			curr_count |= port2.inb() << 8;
			delta = curr_count - prev_count;

			// Comment from the Linux source code:

			// This limit for delta seems arbitrary, but it isn't, it's
			// slightly above the level of error a buggy Mercury/Neptune
			// chipset timer can cause.

		} while (delta < 300);
	}
}

// the "parameter" for the setup code for the application processors
volatile void *ap_stack;

extern "C" void setup_ap(void);

/* aktiviert die CPU mit der ID id, gibt bei erfolg true, ansonsten false zurück */
bool APICSystem::bootCPU(unsigned int cpu_id, void *top_of_stack)
{
	if (cpu_id >= sys_conf->cpu_count) {
		//return false;
	}

	unsigned int id = sys_conf->lapic_id[cpu_id];

//	if (id == bspID) {
//		// don't start the BSP, just set the LAPIC's logical destination address
//		initLAPIC();
//		onlineCPUs++;
//		DBG_VERBOSE << "CPU " << cpu_id << " is BSP" << endl;
//		return true;
//	}

	ap_stack = top_of_stack; // give top of stack as a "parameter" to setup_ap()

	bool IPIDelivered = false;
	unsigned long start_eip = (unsigned long)relocatedSetupAP; // address of relocated setup_ap()

	DBG_VERBOSE << "start_eip = " << hex << start_eip << dec << endl;

	//this should be a *static* assertion
	unsigned char vector = (start_eip >> 12) & 0xff;
	if ((start_eip & ~0x000ff000) ||
			(vector >= 0xa0 && vector <= 0xbf)) {
		DBG_VERBOSE << "weird start_eip, giving up" << endl;
		return false;
	}

	// set BIOS shutdown code to warm start
	IO_Port port1(0x70);
	IO_Port port2(0x71);
	port1.outb(0xf);
	port2.outb(0xa);

	// set reset vector
	*((volatile unsigned short *) 0x469) = start_eip >> 4;
	*((volatile unsigned short *) 0x467) = start_eip & 0xf;

	// The AP Processor should wait for us.
	callout_cpu_number = -1;

	// assert INIT
	struct ICR_L init_ipi;
	init_ipi.destination_shorthand = DESTINATION_SHORTHAND_NO;
	init_ipi.trigger_mode = TRIGGER_MODE_LEVEL;
	init_ipi.level = LEVEL_ASSERT;
	init_ipi.destination_mode = DESTINATION_MODE_PHYSICAL;
	init_ipi.delivery_mode = DELIVERY_MODE_INIT;
	init_ipi.vector = 0;
	init_ipi.delivery_status = 0; //workaround for qemu
	lapic.sendIPI(id, init_ipi);

	delay(2);

	// deassert INIT
	init_ipi.level = LEVEL_DEASSERT;
	lapic.sendIPI(id, init_ipi);

	// if local APIC is external, we don't need STARTUP IPIs
	if (! lapic.isExternalAPIC()) {
		// otherwise we send up to 3 IPIs
		for (int j = 0; j < 2; j++) {
			// send STARTUP IPI
			DBG_VERBOSE << "Sending STARTUP IPI #" << j << endl;
			struct ICR_L startup_ipi;
			startup_ipi.destination_shorthand = DESTINATION_SHORTHAND_NO;
			startup_ipi.trigger_mode = TRIGGER_MODE_EDGE;
			startup_ipi.level = LEVEL_DEASSERT;
			startup_ipi.destination_mode = DESTINATION_MODE_PHYSICAL;
			startup_ipi.delivery_mode = DELIVERY_MODE_STARTUP;
			startup_ipi.vector = (start_eip >> 12);
			startup_ipi.delivery_status = 0; //workaround for qemu
			lapic.sendIPI(id, startup_ipi);

			int timeout = 0;
			// wait if STARTUP IPI was successful
			do {
				delay(1);
				timeout++;
				IPIDelivered = lapic.isIPIDelivered();
			} while (!IPIDelivered && (timeout < 10));

			if (IPIDelivered) {
				DBG_VERBOSE << "Successful sending of STARTUP IPI #" << j << endl;
				break; // everything went fine
			}
		}
	}
	if (IPIDelivered) {
		// reset callin
		callout_cpu_number = cpu_id;

		DBG_VERBOSE << "Waiting for callin from CPU " << id << endl;
		for (int timeout = 0; timeout < 500; timeout++) {
			if (callout_cpu_number == -1) {
				onlineCPUs++;
				DBG_VERBOSE << "Got callin from CPU " << cpu_id << endl;
				return true; // CPU has booted
			}
			delay(1);
		}
	}

	DBG_VERBOSE << "Did not get callin from CPU " << cpu_id << endl;
	return false; // CPU didn't react
}

void APICSystem::setupThisProcessor() {
	// The boot processor starts with callout_cpu_number = 0.
	// Therefore this busy waiting will only take place for the AP
	// processors.
	while (callout_cpu_number < 0) {}
	unsigned char cpu_id = (unsigned char) callout_cpu_number;

	// LAPIC
	unsigned char llapic_id = (1 << cpu_id);
	lapic.init();
	lapic.setLogicalLAPICID(llapic_id);
	logicalLAPICID[cpu_id] = llapic_id;
	cpuID[lapic.getLAPICID()] = cpu_id;

	// Signal the BSP that we have set up our local APIC
	callout_cpu_number = -1;
}

void APICSystem::disablePICs()
{
	DBG_VERBOSE << "PIC comp mode, disabling PICs." << endl;

	// write to IMCR
	IO_Port port1(0x22);
	port1.outb(0x70);
	// disable PIC Mode
	IO_Port port2(0x23);
	port2.outb(0x01);
}

/*
 * setup_ap() ist durch einfaches Kopieren relozierbar, da es (ausser beim
 * Sprung in startup_ap()) keine absoluten Adressen verwendet.
 * Die Funktion muss an die Adresse 0x40000 kopiert werden, damit APs dort
 * loslaufen koennen.
 */
extern char ___SETUP_AP_START__, ___SETUP_AP_END__;
void APICSystem::copySetupAPtoLowMem()
{
	char *src = &___SETUP_AP_START__, *dest = (char *) 0x40000;
	relocatedSetupAP = dest;
	while (src < &___SETUP_AP_END__) {
		*dest++ = *src++;
	}
}

void APICSystem::sendCustomIPI(unsigned char logicalDestination, unsigned char vector)
{
	struct ICR_L ipi;
	ipi.destination_shorthand = DESTINATION_SHORTHAND_NO;
	ipi.level = LEVEL_ASSERT;
	ipi.destination_mode = DESTINATION_MODE_LOGICAL;
	ipi.delivery_mode = DELIVERY_MODE_FIXED;
	ipi.vector = vector;
	lapic.sendIPI(logicalDestination, ipi);
}
