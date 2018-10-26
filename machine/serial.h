// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse zur Kommunikation über den seriellen Anschluss
 */

#pragma once

#include "types.h"

/*! \brief Serielle Schnittstelle.
 *  \ingroup io
 *
 *  Mit Hilfe dieser Klasse kann man über die serielle Schnittstelle
 *  (COM1 - COM4) einfach mit der Außenwelt kommunizieren.
 */

class Serial
{
public:
	/*! \brief COM-Port
	 *
	 *  Die seriellen Schnittstelle (und ihre Hardwareaddresse).
	 *  Moderne Desktop-PCs besitzen derzeit meist maximal einen physikalischen
	 *  Anschluss (COM1).
	 */
	typedef enum {
		COM1 = 0x3f8,
		COM2 = 0x2f8,
		COM3 = 0x3e8,
		COM4 = 0x238,
	} comPort;

	/*! \brief Übertragungsgeschwindigkeit
	 *
	 *  Baud ist die Einheit für die Übertragungsrate von Symbolen, dabei
	 *  entspricht 1 Baud der Geschwindigkeit für die Übertragung eines Symbols
	 *  pro Sekunde.
	 *  Die möglichen Baudraten sind ganzzählige Teiler der Taktrate von
	 *  115200 Hz.
	 */
	typedef enum {
		BAUD_300 = 10,
		BAUD_600 = 9,
		BAUD_1200 = 8,
		BAUD_2400 = 7,
		BAUD_4800 = 6,
		BAUD_9600 = 5,
		BAUD_19200 = 4,
		BAUD_38400 = 3,
		BAUD_57600 = 2,
		BAUD_115200 = 1,
	} baudRate;

	/*! \brief Anzahl der Datenbits pro Zeichen */
	typedef enum {
		DATA_5BIT = 0,
		DATA_6BIT = 1,
		DATA_7BIT = 2,
		DATA_8BIT = 3,
	} dataBits;

	/*! \brief Anzahl der Stopbits pro Zeichen */
	typedef enum {
		STOP_1BIT = 0,
		STOP_1_5BIT = 4,
		STOP_2BIT = 4,
	} stopBits;

	/*! \brief Paritätsbit */
	typedef enum {
		PARITY_NONE = 0,
		PARITY_ODD = 8,
		PARITY_EVEN = 24,
		PARITY_MARK = 40,
		PARITY_SPACE = 56,
	} parity;

private:
	/*! \brief Registerindex */
	enum regIndex {
		rbr = 0, // Receive Buffer Register (on reading)
		tbr = 0, // Transmit Buffer Register (on writing)
		dll = 0, // DLAB Low (With DLAB set to 1)
		ier = 1, // Interrupt Enable Register
		dlh = 1, // DLAB High (With DLAB set to 1)
		irr = 2, // Interrupt Registration Register
		fcr = 2, // FIFO Control Register
		lcr = 3, // Line Control Register (the most significant bit of this register is the DLAB)
		mcr = 4, // Modem Control Register
		lsr = 5, // Line Status Register
		msr = 6  // Modem Status Register
	};

	/*! \brief gewähltes Register */
	const Serial::comPort port;

	/*! \brief Lese Wert aus Register
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param reg Registerindex
	 *  \return gelesener Wert
	 */
	char readReg(Serial::regIndex reg);

	/*! \brief Schreibe Wert in Register
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param reg Registerindex
	 *  \param out zu schreibender Wert
	 */
	void writeReg(Serial::regIndex reg, char out);

public:
	/*! \brief Konstruktor
	 *
	 * Festlegen des Ports und setzen der Parameter für die serielle Verbindung
	 * Standardmäßig ist dies \c 8N1 (8 bit, kein Paritätsbit, ein Stopbit)
	 * mit 115200 Baud auf COM1.
	 *
	 *  \opt Konstruktor implementieren
	 */
	Serial(Serial::comPort port = COM1, Serial::baudRate baudrate = BAUD_115200, Serial::dataBits databits = DATA_8BIT, Serial::stopBits stopbits = STOP_1BIT, Serial::parity parity = PARITY_NONE);

	/*! \brief Lese ein Byte von serieller Schnittstelle
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param blocking Blockiere - Warte bis ein Byte anliegt
	 *  \return gelesener Wert (oder \b -1 bei nicht blockierend falls kein Datum verfügbar)
	 */
	int read(bool blocking = true);

	/*! \brief Schreibe ein Byte auf die serieller Schnittstelle
	 *
	 *  \opt Methode implementieren
	 *
	 *  \param out zu schreibendes Byte
	 *  \param blocking Blockiere - Warte bis ein Byte geschrieben werden kann
	 *  \return geschriebenes Byte (oder \b -1 falls kein Byte geschrieben werden konnte)
	 */
	int write(char out, bool blocking = true);
};

