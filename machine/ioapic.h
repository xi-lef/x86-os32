// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse IOAPIC zum Zugriff auf den IO-APIC
 */

#pragma once

#include "types.h"
#include "machine/ioapic_registers.h"
#include "machine/plugbox.h"

/*! \brief Abstraktion des IO-APICs, der zur Verwaltung der externen Interrupts
 *  dient.
 *  \ingroup interrupts
 *
 *
 *  Kernstück des IOAPICs ist die IO-Redirection Table. Dort lässt sich frei
 *  konfigurieren, welchem Interruptvektor eine bestimmte externe
 *  Unterbrechung zugeordnet werden soll. Ein Eintrag in dieser Tabelle ist
 *  64 Bit breit. Die Strukturen IOREDTBL_L und IOREDTBL_H sind Bitfelder, die
 *  die einzelnen Einstellungen eines Eintrages zugänglich machen.
 */

class IOAPIC
{
	// Verhindere Kopien und Zuweisungen
	IOAPIC(const IOAPIC&)            = delete;
	IOAPIC& operator=(const IOAPIC&) = delete;
public:

	/*! \brief Memory-Mapped Register des IO-APIC im Adressraum der %CPU.
	 *
	 *  Zugriff auf die eigentlichen Register des IO-APICs ist folgendermaßen
	 *  möglich:
	 *  An die Adresse von IOAPIC::IOREGSEL_REG schreibt man zuerst die Nummer des
	 *  IO-APIC-Registers, welches man lesen oder schreiben möchte. In
	 *  IOAPIC::IOWIN_REG kann man dann den Wert des vorher ausgewählten
	 *  IO-APIC-Registers entweder lesen oder schreiben.
	 *
	 *  \see [IO-APIC manual](intel_ioapic.pdf), p. 8
	 */
	static volatile uint32_t *IOREGSEL_REG;
	/// \copydoc IOREGSEL_REG
	static volatile uint32_t *IOWIN_REG;
	/*! \brief Konstruktor. Tut nichts. Initialisierung erfolgt mit init()
	 *
	 *  \todo Konstruktor implementieren
	 */
	IOAPIC() { }//init(); } // TODO ?
	/*! \brief Initialisierung der IOAPICs
	 *
	 *  Dabei werden alle Einträge in der IO-Redirection Table mit einem
	 *  sinnvollen Wert vorbelegt. Das Feld, welches den auszulösenden
	 *  Interruptvektor angibt, sollte mit einer Vektornummer vorbelegt werden,
	 *  die so konfiguriert ist, dass sie den Panic-Handler auslöst.
	 *  Initial sollten auch alle externen Interrupts im IOAPIC deaktiviert
	 *  werden. Neben der Vektortabelle muss auch die APICID im
	 *  IOAPICID-Register auf den Wert gesetzt werden, der während des
	 *  Bootvorgangs aus den Systembeschreibungstabellen gelesen wurde.
	 *  (APICSystem::getIOAPICID())
	 */
	void init();

	/*! \brief Zuordnung eines Vektors in der Interruptvektortabelle zu einem
	 *  externen Interrupt.
	 *
	 *  \param slot Nummer des zu konfigurierenden Slots (und damit des
	 *  dazugehörigen externen Interrupts) in der IO-Redirection Table.
	 *  \param vector Nummer des Vektors, der durch den mit slot ausgewählten
	 *  externen Interrupt aktiviert werden soll.
	 *
	 *  \todo Methode implementieren
	 */
	void config(unsigned char slot, Plugbox::Vector vector);

	/*! \brief Sorgt dafür, dass Unterbrechungen des zugeordneten Gerätes an
	 *  die CPU(s) weitergereicht werden.
	 *
	 *  Um eine Unterbrechungsbehandlung zu ermöglichen, muss zusätzlich
	 *  CPU::enable_int() in der main() aufgerufen werden.
	 *
	 * \param slot Nummer des freizuschaltenden Slots
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void allow(unsigned char slot);

	/*! \brief Ermöglicht einzelne Interrupts selektiv zu sperren.
	 *  \param slot Nummer des zu sperrenden Slots
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void forbid(unsigned char slot);

	/*! \brief Ermöglicht eine Abfrage des Maskierungsstatus für einzelne
	 *  Interrupts.
	 *  \param slot gibt an, für welchen Slot der Status abgefragt
	 *  werden soll.
	 *  \return gibt `true` zurück, falls der Interrupt zugelassen ist und
	 *  `false`, falls er ausmaskiert wurde.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	bool status(unsigned char slot);
};

extern IOAPIC ioapic;
