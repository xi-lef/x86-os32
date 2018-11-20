/*
 * Copyright (C) 2016  Matt Borgerson
 * Copyright (C) 2018  Bernhard Heinloth
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "stub.h"

#include "debug/output.h"
#include "debug/assert.h"
#include "machine/ticketlock.h"

/*! \brief Mutex für Unterbrechungsbehandlung
 *
 * Selbstverständlich sollte man bei der Kombination 'aktives Warten' und
 * 'Unterbrechungsbehandlung' sofort üble Bauchschmerzen bekommen.
 * Als Ausrede dient hier, dass es für den Debugger ist, welcher zum einen
 * so einfach wie möglich sein sollte, zum anderen auch nicht im produktiven
 * Betrieb verwendet werden sollte.
 *
 * Also bitte nur als 'simple Hack' und nicht als 'best practice' im Hinterkopf
 * behalten.
 *
 * Theoretisch kann hier auch ein Spinlock verwendet werden, das kann aber
 * bei sich schnell wiederholenden Breakpoints (z.B. in Schleifen) leicht dazu
 * führen, dass immer nur ein Kern an die Reihe kommt, während die anderen
 * ewig warten -- deshalb sollte das faire Ticketlock bevorzugt werden.
 */
static Ticketlock mutex;

/*! \brief GDB Instanz
 *
 * für Quasi-Singleton
 */
static GDB_Stub * instance = 0;

/*** Hilfsfunktion ***/

/*!\brief Setze einen Speicherbereich auf einen Wert
 *
 * \param ptr Zeiger zum Speicherbereich
 * \param data Quelldaten mit roher Darstellung
 * \param len Größe der Quelldaten
 * \return Anzahl der in Puffer verwendeten Bytes oder \c -1 falls Puffer zu klein
 */
static void * memset(void *ptr, int data, size_t len){
	char *p = (char*)ptr;
	while (len--)
		*p++ = (char)data;
	return ptr;
}

/*** Debug-Unterbrechungsbehandlung (generischer Teil) ***/

extern "C" void debug_handler(struct debug_context *context){
	// Benötigt eine aktive GDB Instanz
	if (instance == 0)
		return;

	// Hole den internen Zustandsspeicher
	struct GDB_Stub::state * state = &(instance->state);

	// Da wir nur einen internen Zustandsspeicher haben, kann dies im Mehrkern-
	// betrieb schnell böse enden, wenn mehrere Kerne gleichzeitig schreibend
	// darauf zugreifen wollen. Deshalb exklusiver Ausschluss!
	mutex.lock();

	// Lösche den vorherigen Inhalt
	memset(&state->registers, 0, sizeof(state->registers));

	// Setze den Interrupt Vektor
	state->signum = context->vector;

	// Übergebe den Zustand der Register
	state->registers[GDB_Stub::REG_EAX] = context->eax;
	state->registers[GDB_Stub::REG_ECX] = context->ecx;
	state->registers[GDB_Stub::REG_EDX] = context->edx;
	state->registers[GDB_Stub::REG_EBX] = context->ebx;
	state->registers[GDB_Stub::REG_ESP] = context->esp;
	state->registers[GDB_Stub::REG_EBP] = context->ebp;
	state->registers[GDB_Stub::REG_ESI] = context->esi;
	state->registers[GDB_Stub::REG_EDI] = context->edi;
	state->registers[GDB_Stub::REG_PC]  = context->eip;
	state->registers[GDB_Stub::REG_CS]  = context->cs;
	state->registers[GDB_Stub::REG_PS]  = context->eflags;
	state->registers[GDB_Stub::REG_SS]  = context->ss;
	state->registers[GDB_Stub::REG_DS]  = context->ds;
	state->registers[GDB_Stub::REG_ES]  = context->es;
	state->registers[GDB_Stub::REG_FS]  = context->fs;
	state->registers[GDB_Stub::REG_GS]  = context->gs;

	// Verarbeite den aktuellen Zustand
	instance->handle();

	// Stelle die (ggf. geänderten) Register wieder her
	context->eax    = state->registers[GDB_Stub::REG_EAX];
	context->ecx    = state->registers[GDB_Stub::REG_ECX];
	context->edx    = state->registers[GDB_Stub::REG_EDX];
	context->ebx    = state->registers[GDB_Stub::REG_EBX];
	context->esp    = state->registers[GDB_Stub::REG_ESP];
	context->ebp    = state->registers[GDB_Stub::REG_EBP];
	context->esi    = state->registers[GDB_Stub::REG_ESI];
	context->edi    = state->registers[GDB_Stub::REG_EDI];
	context->eip    = state->registers[GDB_Stub::REG_PC];
	context->cs     = state->registers[GDB_Stub::REG_CS];
	context->eflags = state->registers[GDB_Stub::REG_PS];
	context->ss     = state->registers[GDB_Stub::REG_SS];
	context->ds     = state->registers[GDB_Stub::REG_DS];
	context->es     = state->registers[GDB_Stub::REG_ES];
	context->fs     = state->registers[GDB_Stub::REG_FS];
	context->gs     = state->registers[GDB_Stub::REG_GS];

	// Kritischen Bereich freigeben
	mutex.unlock();
}

GDB_Stub::GDB_Stub(bool wait, bool debugOutput, Serial::comPort port, Serial::baudRate baudrate) : Serial(port, baudrate, DATA_8BIT, STOP_1BIT, PARITY_NONE), debug(debugOutput){
	// Quasi-Singleton
	assert(instance == 0);

	// Instanzvariable setzen
	instance = this;

	// Debug-Unterbrechungsbehandlungen installieren
	for (int i=0; i<17; i++)
		if (    i != 2  // NMI
		     && i != 15 // reserved
		   ){
			install_handler(i);
		}

	// Warte auf Verbindung
	if (wait){
		breakpoint();
	}
}

