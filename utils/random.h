// vim: set et ts=4 sw=4:

#pragma once

/*! \file
 *  \brief Enthält die Klasse Random
 */

class Random
{
private:
	Random(const Random &copy); // Verhindere Kopieren
private:
	unsigned long r0, r1, r2, r3, r4, r5, r6;
	unsigned long multiplier, addend, ic_state;
public:
	/*! \brief Konstruktor; Initialisierung mit \b seed
	 *  \param seed Initialwert für den Pseudozufallszahlengenerator.
	 */
	Random(int seed);

	/*! \brief Liefert eine Zufallszahl.
	 *  \return Zufallszahl.
	 */
	int number();
};

