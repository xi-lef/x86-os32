#pragma once

/*! \defgroup interrupts Unterbrechungsbehandlung
 *  \brief Das Interrupt Subsystem
 *
 * Das Interrupt Subsystem von OO/MPStubBS beinhaltet alle
 * Funktionalität um Unterbrechungen von der Hardware anzunehmen und
 * diese abzuarbeiten. In späteren Übungen werden dann die Interrupts
 * mit der Anwendungs-aktivierten Kernausführung (Systemaufrufe)
 * synchronisiert. Der Eintrittspunkt für das Interrupt Subsystem ist die Funktion \ref guardian.
 */

#include "machine/cpu.h"

/*! \brief Low-Level Interrupt-Behandlung.
 *  \ingroup interrupts
 *
 *
 *  Zentrale Unterbrechungsbehandlungsroutine des Systems. Diese
 *  Funktion wird von dem Assembler Teil der Unterbrechungsbehandlung
 *  aufgerufen. Beim Betreten der Funktion sind die Unterbrechungen
 *  auf dieser CPU gesperrt.
 *
 *  \param vector gibt die Nummer des aufgetretenen Interrupts an.
 *  \param context beinhaltet einen Pointer auf den eigenen Stack, über
 *                 den auf den Unterbrechungskontext zugegriffen werden kann.
 */
extern "C" void guardian(uint32_t vector, irq_context *context);

