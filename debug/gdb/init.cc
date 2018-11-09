// vim: set et ts=4 sw=4:

#include "stub.h"

/*!\brief Array mit Addressen der jeweiligen spezifischen Debug-Unterbrechungsbehandlungsroutinen
 * aus der gdb_interrupt.asm ( dbg_irq_entry_%INDEX )
 */
extern void const * const dbg_irq_entries[];

bool GDB_Stub::install_handler(int vector){
	//TODO: Implementieren. Falls du Lust hast. Ist wieder freiwillig.
	// Braucht allerdings die Klasse Serial (welche in der vorherigen Aufgabe freiwillig war).
	// Und Ticketlock (Du kannst zu Testzwecken gerne stattdessen einmal Spinlock probieren - was fällt bei vielen Breakpoints auf?).
	(void) vector;
	return false;
}

