// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält Debugmacros, um Debugausgaben auf einen eigenen
 *  Screen umzuleiten.
 *
 *  Für den Uniprozessorfall reicht es ein CGA_Stream Objekt für Debugausgaben
 *  (\b dout) anzulegen.
 */

#pragma once

/*!
 *  \def DBG_VERBOSE
 *  \brief Stream, welche nur im Verbose-Modus im Debugfenster angezeigt wird
 *  \note Sofern eine serielle Konsole implementiert wurde, kann diese (durch
 *        Änderung des Makros) als Ausgabe verwendet werden - dadurch wird
 *        die ggf. sehr umfangreiche Ausgaben besser lesbar (Stichwort Scrollen)
 */
#ifdef VERBOSE
// Sofern VERBOSE gesetzt ist, leite alles an DBG (unten definiert) weiter
#define DBG_VERBOSE DBG
#else
// Andernfalls wird alles an den Null_Stream gesendet, der alles einfach verwirft
#define DBG_VERBOSE nullstream
// Wir müssen noch den Null_Stream inkludieren
#include "debug/null_stream.h"
#endif

/*!
 *  \def DBG
 *  \brief Ausgabestrom, welcher in dem der CPU zugeordneten Debugfenster
 *         angezeigt wird
 *
 *  \note In MPStuBS muss für jede CPU ein Objekt für Debugausgaben
 *        mit dem Namen `dout_CPUx` angelegt werden.
 *
 *  Während dies in OOStuBS nur ein Alias auf `dout` ist, ist der Fall für
 *  die Mehrprozessorvariante etwas komplexer:
 *
 *  Das Debugmacro wählt den Ausgabestream durch Aufruf der anonymen,
 *  im Makro selbst definierten Funktion aus.
 *  Dieser durchaus umständliche Weg (im vergleich zu Inline Funktionen)
 *  ist notwendig, damit in den nachfolgend inkludierten Dateien selbst das
 *  Debug Makro verwendet werden kann
 *
 *  \warning Bei ungünstiger Einplanung ist es möglich, dass die Ausgabe auf
 *           der falschen (vorherigen) CPU geschieht.
 */
#define DBG ( \
		[]()->CGA_Stream& { \
			switch(system.getCPUID()){ \
				case 0:  return dout_CPU0; \
				case 1:  return dout_CPU1; \
				case 2:  return dout_CPU2; \
				default: return dout_CPU3; \
			} \
		}() \
	)

#include "device/cgastr.h"
#include "machine/apicsystem.h"

extern APICSystem system;

/*! \brief Debugausgabefenster für die CPU 0
 *  Zeigt Ausgaben zur Fehlersuche wie
 *      `DBG << "Var = " << var << endl`
 *  in einem separaten Debugfenster für die entsprechende CPU an.
 *
 *  Während für OOStuBS nur ein Objekt `dout` angelegt werden muss, ist bei
 *  MPStuBS ein Objekt je CPU mit dem Namen `dout_CPUx` notwendig
 *
 *  Dieses sollte im Idealfall überschneidungsfrei unterhalb des normalen
 *  Ausgabefensters liegen und mindestens 3 Zeilen lang sein.
 *  Zwei Debugfenster können problemlos nebeneinander liegen.
 *
 *  \todo Objekt anlegen
 */
extern CGA_Stream dout_CPU0;

/*! \brief Debugausgabefenster für die CPU 1
 *  \see dout_CPU0
 *
 *  \todo Objekt (in MPStuBS) anlegen
 */
extern CGA_Stream dout_CPU1;

/*! \brief Debugausgabefenster für die CPU 2
 *  \see dout_CPU0
 *
 *  \todo Objekt (in MPStuBS) anlegen
 */
extern CGA_Stream dout_CPU2;

/*! \brief Debugausgabefenster für die CPU 3
 *  \see dout_CPU0
 *
 *  \todo Objekt (in MPStuBS) anlegen
 */
extern CGA_Stream dout_CPU3;
