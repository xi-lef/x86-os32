// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse CPU
 */

/*! \defgroup sync CPU Synchronisation
 *
 * Im Synchronisationsmodul sind die Funktionalitäten implementiert,
 * die benötigt werden um mehrere Prozessoren und Aktivitäten
 * miteinander in Einklang zu bringen. Dies bezieht sich insbesondere
 * darauf, wenn mehrere Teilnehmer auf eine Ressource zugreifen
 * wollen. Dabei können die Teilnehmer auf der gleichen CPU oder
 * unterschiedlichen Prozessoren laufen.
 */

#pragma once

#include "types.h"

/*! \brief Implementierung einer Abstraktion fuer den Prozessor.
 *
 *  Derzeit wird angeboten: Interrupts zuzulassen, zu verbieten, den
 *  Prozessor in den Haltmodus zu schicken oder ganz anzuhalten.
 */
class CPU
{
private:
	CPU(const CPU&) = delete;
	CPU& operator=(const CPU&) = delete;
	/*! \brief Konstruktor
	 *
	 */
	CPU() {}
public:

	/*! \brief Erlauben von (Hardware-)Interrupts
	 *  \ingroup sync
	 *
	 *
	 *  Lässt die Unterbrechungsbehandlung zu, indem die Assembleranweisung
	 *  \b sti ausgeführt wird.
	 */
	static void enable_int() {
		asm volatile("sti\n\t nop\n\t" : : : "memory");
	}
	/*! \brief Interrupts werden ignoriert/verboten
	 *  \ingroup sync
	 *
	 *
	 *  Verhindert eine Reaktion auf Unterbrechungen, indem die
	 *  Assembleranweisung \b cli ausgeführt wird. Der Rückgabewert gibt an ob
	 *  Unterbrechungen schon gesperrt waren.
	 *  \return true wenn die Unterbrechungen zum Zeitpunkt des aufrufs frei waren, false wenn sie schon gesperrt waren.
	 */
	static bool disable_int() {
		uint32_t out;
		asm volatile (
			"pushf\n\t"
			"pop %0\n\t"
			"cli\n\t"
			: "=r"(out)
			:
			: "memory"
		);

		bool enabled = (out & 0x200) != 0;
		return enabled;
	}
	/*! \brief Unterbrechungszustand wiederherstellen
	 *
	 *  Zusammen mit dem Rückgabewert von CPU::disable_int() kann
	 *  diese Funktion verwendet werden, um das Sperren und freigeben
	 *  von Unterbrechungen zu schachteln.
	 *  \param val Gibt an ob Unterbrechungen wieder freigegeben werden sollen oder nicht.
	 */
	static void restore_int(bool val) {
		if(val) {
			enable_int();
		}
	}
	/*! \brief Prozessor bis zum nächsten Interrupt anhalten
	 *
	 *  Versetzt den Prozessor in den Haltezustand, aus dem er nur durch einen
	 *  Interrupt wieder erwacht. Intern werden dazu die Interrupts mit \b sti
	 *  freigegeben und der Prozessor mit \b hlt angehalten. Intel garantiert,
	 *  dass die Befehlsfolge \b sti \b hlt atomar ausgeführt wird.
	 */
	static void idle() {
		asm volatile("sti\n\t hlt\n\t" : : : "memory");
	}
	/*! \brief Prozessor dauerhaft anhalten
	 *
	 *  Hält den Prozessor an. Intern werden dazu die Interrupts mit \b cli
	 *  gesperrt und anschließend der Prozessor mit \b hlt angehalten. Da der
	 *  Haltezustand nur durch einen Interrupt verlassen werden könnte, ist
	 *  somit garantiert, dass die CPU bis zum nächsten Kaltstart "steht".
	 *  Das Programm kehrt aus die() nie zurück. In einer
	 *  Multiprozessorumgebung hat die Ausführung der Funktion nur
	 *  Auswirkungen auf die CPU, die ihn ausführt. Die anderen CPUs laufen
	 *  weiter.
	 */
	static void die() {
		asm volatile("cli\n\t hlt\n\t" : : : "memory");
	}

	/*! \brief Prozessor anhalten
	 *
	 *  Hält den Prozessor an. Intern wird dazu die \b hlt Instruktion ausgeführt.
	 *  In diesem Zustand verbraucht der Prozessor sehr wenig Strom.
	 *  Nur ein Interrupt kann den Prozessor aufwecken.
	 */
	static void halt() {
		asm volatile("hlt\n\t" : : : "memory");
	}

	static uint64_t rdmsr(uint32_t id) {
		uint64_t retval;
		asm volatile("rdmsr" : "=A"(retval) : "c"(id) : "memory");
		return retval;
	}

	static void wrmsr(uint32_t id, uint64_t val) {
		asm volatile("wrmsr" : : "A"(val), "c"(id) : "memory");
	}
};

/*! \brief Gesicherter Unterbrechungskontext (generischer Teil)
 *
 *  Der Assemblerteil von OO/MP-StuBS legt die flüchtigen Register auf
 *  den Stapel, bevor die guardian() Funktion betreten wird. Die
 *  irq_context und irq_context_* typen beschreiben das Datenlayout
 *  für unterschiedliche Fälle (Traps mit Error Code, Interrupts). Um
 *  auf die von der Hardware gesichterten Werte zugreifen zu können,
 *  muss der Pointer zu einem der anderen irq_context_* Strukturen
 *  gecastet werden.
 *
 *  Eine genaue Beschreibung des Hardware Kontextes kann im [Intel
 *  Manual](intel_manual_2017-03.pdf) ¶6.12 nachgelesen werden.
 *
 *  Allen CPU Kontexten gemein sind die flüchtigen Register (eax, ecx, edx)
 * \see guardian
 * \see irq_context_without_error_code
 * \see irq_context_with_error_code
 */
struct irq_context {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
} __attribute__((packed));

/*! \brief Gesicherter Unterbrechungskontext falls kein Fehlercode vorhanden ist.
 *
 *  Bei Unterbrechungen sichert zuerst die Hardware, und dann der
 *  Assemblerteil der Unterbrechungbehandlung den Kontext der CPU.
 *  Dieser Kontext beinhaltet alles um den Zustand geordnet wieder
 *  herstellen zu können, wenn die Unterbrechung behandelt ist. Diese
 *  Struktur kann für Unterbrechungen verwendet werden, falls kein
 *  Fehlercode von der Hardware auf den Stack gelegt wurde.
 *
 *  \see guardian
 *  \see irq_context
 *  \extends irq_context
 */
struct irq_context_without_error_code {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;

	// Bei einem normalen Interrupt werden mindestens drei
	// Register von der Hardware auf den Stack geschoben, bevor
	// der Interrupthandler ausgeführt wird
	uint32_t eip; //!< Befehlszähler des unterbrochenen Programms
	uint32_t cs;  //!< Code Segment  des unterbrochenen Programms
	uint32_t eflags; //!< Flags zum Unterbrechungszeitpunkt
	// Wenn ein Ringwechsel vorgenommen wurde, wurde noch der
	// Stackpointer des Benutzers gesichert.
	uint32_t user_esp; //!< Stackpointer zum Unterbrechungszeitpunkt. Nur vorhanden, falls ein Ringwechsel stattgefunden hat.
	uint32_t user_ss;  //!< Stacksegment zum Unterbrechungszeitpunkt. Nur vorhanden, falls ein Ringwechsel stattgefunden hat.
} __attribute__((packed));

/*! \brief Gesicherter Unterbrechungskontext falls ein Fehlercode vorhanden ist.
 *
 *  Bei Unterbrechungen sichert zuerst die Hardware, und dann der
 *  Assemblerteil der Unterbrechungbehandlung den Kontext der CPU.
 *  Dieser Kontext beinhaltet alles um den Zustand geordnet wieder
 *  herstellen zu können, wenn die Unterbrechung behandelt ist. Diese
 *  Struktur kann für Unterbrechungen verwendet werden, falls ein
 *  Fehlercode von der Hardware auf den Stack gelegt wurde.
 *
 *  \see guardian
 *  \see irq_context
 *  \extends irq_context
 */
struct irq_context_with_error_code {
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;

	// Wenn der CPU Kontext durch eine Ausnahme erzeugt wurde, die
	// einen Fehlercode auf den Stack schiebt muss dieses Element
	// der Union verwendet werden.
	uint32_t errorcode;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	// Wenn ein Ringwechsel vorgenommen wurde, wurde noch der
	// Stackpointer des Benutzers gesichert.
	uint32_t user_esp;
	uint32_t user_ss;
} __attribute__((packed));

