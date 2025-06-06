// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Diese Datei enthält die Klasse Keyboard
 */

#pragma once

#include "machine/keyctrl.h"
#include "guard/gate.h"
#include "machine/key.h"
#include "syscall/guarded_semaphore.h"
#include "object/bbuffer.h"
#include "user/string/string.h"
#include "device/cgastr.h"

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
    BBuffer<Key, 64> prebuf;
    BBuffer<Key, 64> buf;
    Semaphore sem;

public:
	/*! \brief Konstruktor
	 *
	 */
	Keyboard() : sem(0) {}

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

    bool prologue() override;
    void epilogue() override;

    Key getkey();

    size_t read(String *s, size_t count, CGA_Stream& out = kout);

    Keyboard& operator >>(char &c);
    Keyboard& operator >>(String &s);
    Keyboard& operator >>(long &i);
};

