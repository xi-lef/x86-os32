// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Diese Datei enthält die Klasse Keyboard
 */

#pragma once

#include "machine/keyctrl.h"
#include "guard/gate.h"
#include "machine/key.h"

/*! \brief Die Klasse Keyboard stellt die Abstraktion der Tastatur dar.
 *  \ingroup io
 *
 *
 *  Sie sorgt für die korrekte Initialisierung und vor allem für die
 *  Unterbrechungsbehandlung. Später wird Keyboard auch die Tastaturabfrage
 *  durch die Anwendung ermöglichen.
 */
class Keyboard
	: public Gate, public Keyboard_Controller
{
	// Verhindere Kopien und Zuweisungen
	Keyboard(const Keyboard&)            = delete;
	Keyboard& operator=(const Keyboard&) = delete;

private:
public:
	/*! \brief Konstruktor
	 *
	 */
	Keyboard() {}

	/*! \brief 'Anstöpseln' der Tastatur.
	 *
	 *  Initialisierung der Tastatur und Aktivieren der spezifischen
	 *  Interruptbehandlung. Dazu muss sich das Keyboard-Objekt bei der Plugbox
	 *  plugbox anmelden und dem IOAPIC Objekt ioapic mitteilen, dass
	 *  Unterbrechungen der Tastatur fortan dem Prozessor gemeldet werden
	 *  sollen.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void plugin();

	/*! \brief Bearbeitung der Tastaturunterbrechung.
	 *
	 * Hier soll die Bearbeitung des von der Tastatur ausgelösten Interrupts
	 * erfolgen. trigger gibt dazu entsprechend gültige Zeichen auf einer
	 * eigenen Zeile auf dem Bildschirm aus. Wird die Tastenkombination
	 * Ctrl-Alt-Delete gedrückt, soll ein Reboot ausgelöst werden.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void trigger();

};

extern Keyboard keyboard;
