// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält Funktionen für den Systemstart, die nicht mehr in
 *  Assembler geschrieben werden müssen.
 */

#include "types.h"
#include "machine/apicsystem.h"
#include "machine/cpu.h"
#include "machine/io_port.h"

/*! \brief Die Zeiger auf die Multiboot-Strukturen wird vom Assembler-
 * startup zugewiesen.
 */
void *multiboot_addr = 0;

/*! \brief Hilfsfunktion der PIT-Initialisierung, die ein kurzes Delay
 *  verursacht.
 */
static void delay()
{
	asm volatile("nop\n\t":::"memory");
}

/*! \brief Initalisierung der PICs (Programmierbare Interrupt-Controller),
 *  damit alle 15 Hardware-Interrupts nacheinander in der idt liegen.
 */
static void initialise_pics()
{
	IO_Port p20(0x20);
	IO_Port p21(0x21);
	IO_Port pa0(0xa0);
	IO_Port pa1(0xa1);

	//ICW1: 8086 Modus mit ICW4
	p20.outb(0x11);
	delay();
	pa0.outb(0x11);
	delay();

	//ICW2 Master: IRQ # Offset (32)
	p21.outb(0x20);
	delay();

	//ICW2 Slave: IRQ # Offset (40)
	pa1.outb(0x28);
	delay();

	//ICW3 Master: Slaves an IRQs
	p21.outb(0x04);
	delay();

	//ICW3 Slave: Verbunden mit IRQ2 des Masters
	pa1.outb(0x02);
	delay();

	//ICW4: 8086 Modus und automatischer EIO
	p21.outb(0x03);
	delay();
	pa1.outb(0x03);
	delay();

	//Alle Hardware-Interrupts durch Legacy-PICs ausmaskieren.
	pa1.outb(0xff);
	delay();

	p21.outb(0xff);
}

/*! \brief Ab gcc 4.7 weitere Initialisierung globaler Objekte.
 *
 */
static void csu_init()
{
	extern void (*__init_array_start []) ();
	extern void (*__init_array_end[]) ();

	const size_t size = __init_array_end - __init_array_start;
	for (size_t i = 0; i < size; i++) {
		(*__init_array_start[i])();
	}
}

extern "C" void _init();
extern "C" void _fini();
extern "C" int main_ap() __attribute__ ((weak));
extern "C" int main();

static bool bootCPU = true;
extern "C" void kernel_init()
{

	if(bootCPU) {
		bootCPU = false;

		//initialisierung der PICs
		initialise_pics();

		//Aufruf globaler Konstruktoren
		_init();
		csu_init();

		//weitere Initialisierungen
		system.detectSystemType();
		system.setupThisProcessor();

		//Die main-Funktion
		main();

		//falls die main-Funktion zurückkehren sollte werden die Destruktoren
		//globaler Objekte aufgerufen
		_fini();
	}
	else {
		// Initialisiere diesen AP Prozessor
		system.setupThisProcessor();

		// Springe die Anwendung an.
		main_ap();
	}

	for(;;) {
		CPU::die();
	}
}
