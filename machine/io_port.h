// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Zugriffsklasse für den IO-Adressraum des x86
 */

#pragma once

#include "types.h"

/* KLASSENDEFINITION */

/*!  \brief Die IO_Port-Klasse dient dem Zugriff auf die Ein-/Ausgabeports des PC.
 *
 *   Beim PC gibt es einen gesonderten I/O-Adressraum, der nur mittels der
 *   Maschineninstruktionen 'in' und 'out' angesprochen werden kann.
 *   Ein IO_Port-Objekt wird beim Erstellen an eine Adresse des I/O-Adressraums
 *   gebunden und kann dann fuer byte- oder wortweise Ein- oder Ausgaben verwendet
 *   werden.
 */

class IO_Port
{
	/*! \brief Adresse im I/O-Adressraum
	 *
	 */
	uint16_t address;
public:
	/*! \brief Konstruktor
	 *  \param a Adresse des IO-Ports im IO-Adressraum
	 */
	IO_Port(uint16_t a) : address(a) {}
	/*! \brief Byteweise Ausgabe eines Wertes ueber einen I/O-Port.
	 *  \param val Wert, der ausgegeben werden soll.
	 */
	void outb(uint8_t val) const {
		asm volatile(
			"out %%al, %%dx\n\t"
			:
			:"a"(val), "d"(address)
			:
		);
	};
	/*! \brief Wordweise Ausgabe eines Wertes ueber einen I/O-Port.
	 *  \param val Wert, der ausgegeben werden soll.
	 */
	void outw(uint16_t val) const {
		asm volatile(
			"out %%ax, %%dx\n\t"
			:
			:"a"(val), "d"(address)
			:
		);
	};
	/*! \brief Byteweises Einlesen eines Wertes ueber einen I/O-Port.
	 *  \return Gelesenes Byte.
	 */
	uint8_t inb() const {
		uint8_t out = 0;

		asm volatile(
			"in %%dx, %%al\n\t"
			:"=a"(out)
			:"d"(address)
			:
		);

		return out;
	};
	/*! \brief Wordweises Einlesen eines Wertes ueber einen I/O-Port.
	 *  \return Gelesenes Word.
	 */
	uint16_t inw() const {
		uint16_t out = 0;

		asm volatile(
			"inw %%dx, %%ax\n\t"
			:"=a"(out)
			:"d"(address)
			:
		);

		return out;
	};
};

