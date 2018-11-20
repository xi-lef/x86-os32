// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die für Remotedebugging notwendigen Klassen und Funktionen
 */

/*! \defgroup debug Debugging Tools
 *    \brief Unterstützung bei der Entkäferung des Betriebssystems
*/

#pragma once

#include "machine/serial.h"

/*! \brief Setze einen manuellen Debug Breakpoint.
 *
 *  Es wird ein Breakpoint-Trap ausgelöst
 */
inline void breakpoint(void){
	asm volatile("int $3;\r\n" ::: "memory");
}

/*! \brief Gesicherter Debug-Unterbrechungskontext
 *
 * \see debug_handler
 * \see irq_context
  */
struct debug_context {
	uint32_t ss;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	uint32_t vector;
	uint32_t error_code;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
} __attribute__((packed));

/*! \brief Low-Level Debug-Interrupt-Behandlung.
 *
 *  Debug-Unterbrechungsbehandlungsroutine des Systems, analog zu \ref guardian .
 *  Aufruf dieser Funktion durch den Assembler Teil der Debug-
 *  Unterbrechungsbehandlung (\c dbg_irq_entry_* in der debug/gbd/handler.asm) --
 *  diese Routinen müssen zuvor mittels GDB_Stub::install_handler installiert
 *  werden.
 *  Nach dem vorbereiten der Daten wird GDB_Stub::handle aufgerufen, welches
 *  mit dem Host über die serielle Schnittstelle kommuniziert.
 *
 *  \note Für korrekte Funktionalität im Multiprozessorfall wird eine korrekte
 *  Implementierung der Klasse Ticketlock vorausgesetzt!
 *
 *  \param context beinhaltet einen Pointer auf den eigenen Stack, über den auf
 *                 den Debug-Unterbrechungskontext zugegriffen werden kann.
 */
extern "C" void debug_handler(struct debug_context * context);

/*! \brief GDB Remote Stub Klasse
 *  \ingroup debug
 *
 *  Diese Klasse ermöglicht ein entferntes Debuggen des Betriebssystems mit GDB
 *  auf echter Hardware (aber auch im Emulator), in dem sie einen Teil des
 *  <a href="https://sourceware.org/gdb/onlinedocs/gdb/Remote-Protocol.html">GDB Remote Serial Protocols (RSP)</a>
 *  implementiert, eigene Unterbrechungsbehandlungsroutinen für Traps
 *  installiert und über die serielle Schnittstelle kommuniziert.
 *
 *  \note GDB liegt bereits eine <a href="https://sourceware.org/git/gitweb.cgi?p=binutils-gdb.git;a=blob;f=gdb/stubs/i386-stub.c;hb=HEAD">i386-stub.c</a>
 *        bei, welche jedoch hässlich, schlecht gewartet und nicht sonderlich
 *        gut in unser objektorientiertes Betriebssystem integrierbar ist.
 *        Deshalb verwenden wir eine überarbeitete Version von <a href="https://github.com/mborgerson/gdbstub">Matt Borgersons gdbstub</a>
 *        (veröffentlicht 2016 unter der GPL v2 Lizenz).
 */
class GDB_Stub
	: public Serial
{
	// Verhindere Kopien und Zuweisungen
	GDB_Stub(const GDB_Stub&)            = delete;
	GDB_Stub& operator=(const GDB_Stub&) = delete;

	/*! \brief zusätzliche Debug-Ausgaben auf dem Zielrechner
	 * (zum debuggen des Debuggerstubs)
	 */
	const bool debug;

public:
	/*! \brief Konstruktor
	 *
	 * Konfiguriert die serielle Schnittstelle (als \c 8N1 ) sowie
	 * Unterbrechungsbehandlungen
	 *
	 * \param wait        Warte nach der Konfiguration auf eine GDB-Verbindung
	 * \param debugOutput Debugge den GDB-Stub durch Bildschirmausgaben der
	 *                    Kommunikation (hilfreich bei Erweiterung des RSP)
	 * \param port        COM-Port für die serielle Verbindung
	 * \param baudrate    Baud Rate, Standard bei GDB ist \c 9600
	 *                    (kann aber zu Engpässen führen)
	 */
	GDB_Stub(bool wait = false, bool debugOutput = false, Serial::comPort port = COM1, Serial::baudRate baudrate = BAUD_9600);

protected:
	/*! \brief Datentyp für Speicheradresse (mit Zeigerarithmetik)
	 */
	typedef unsigned int address;

	/*! \brief Datentyp für Register
	 */
	typedef unsigned int reg;

	/*! \brief zur Verfügung stehende (gesicherte) Register
	 */
	typedef enum {
		REG_EAX       = 0,
		REG_ECX       = 1,
		REG_EDX       = 2,
		REG_EBX       = 3,
		REG_ESP       = 4,
		REG_EBP       = 5,
		REG_ESI       = 6,
		REG_EDI       = 7,
		REG_PC        = 8,
		REG_PS        = 9,
		REG_CS        = 10,
		REG_SS        = 11,
		REG_DS        = 12,
		REG_ES        = 13,
		REG_FS        = 14,
		REG_GS        = 15,
		NUM_REGISTERS = 16
	} Registers ;

	/*! \brief Struktur mit dem Systemzustands nach Traps
	 */
	struct state {
		int signum;
		reg registers[NUM_REGISTERS];
	};

	/*! \brief Speicher für den aktuelle CPU Zustand
	 *
	 * Wird am Beginn einer Unterbrechungsbehandlung aktualisiert und an
	 * deren Ende wieder zurückgeschrieben (mit ggf. modifizierten Werten)
	 */
	struct state state;

	/*! \brief Behandlung eines Traps
	 *
	 * Diese Funktion wird nach der Speicherung des aktuellen CPU Zustandes von
	 * der generischen Debug-Unterbrechungsbehandlungsroutine aufgerufen.
	 * Sie ist das Herzstück und übernimmt die Kommunikation mit dem GDB Host.
	 */
	void handle(void);

	/*! \brief Erlaube der generischen Unterbrechungsbehandlung Zugriff auf die
	 * geschützten Methoden dieser Klasse
	 */
	friend void debug_handler(struct debug_context * context);

	/*! \brief Sende eine Zeichenkette über die serielle Schnittstelle
	 *
	 * \param buf Zeiger auf den Puffer
	 * \param len Größe des Puffers
	 * \retval  0 falls erfolgreich,
	 * \retval -1 falls kein/nicht alle Bytes gesendet werden konnten
	 */
	int writeString(const char *buf, size_t len);

	/*! \brief Empfange eine Zeichenkette über die serielle Schnittstelle
	 *
	 * \param buf Zeiger auf den Puffer
	 * \param buf_len Größe des Puffers
	 * \param len Anzahl der zu empfangenden Bytes
	 * \retval  0 falls erfolgreich,
	 * \retval -1 falls kein/nicht alle Bytes empfangen werden konnten
	 */
	int readString(char *buf, size_t buf_len, size_t len);

	/*! \brief Sende ein Datenpaket
	 *
	 * Datenpakete haben die Form
	 * \code{.gdb}
	 *    $<Daten>#<Prüfsumme>
	 * \endcode
	 *
	 * \param pkt Zeiger auf den Paketpuffer
	 * \param pkt_len Größe des Paketpuffers
	 * \retval  0 falls Paket erfolgreich gesendet und bestätigt wurde,
	 * \retval  1 falls Paket zwar gesend aber nicht bestätigt wurde,
	 * \retval -1 andernfalls
	 */
	int send_packet(const char *pkt, size_t pkt_len);

	/*! \brief Empfange ein Datenpaket
	 *
	 * Bedingt eine fehlerfreie Verbindung zur Übertragung von 7-Bit ANSI Zeichen
	 *
	 * \param pkt_buf Zeiger auf den Paketpuffer
	 * \param pkt_buf_len Größe des Paketpuffers
	 * \param pkt_len Größe des zu empfangenden Pakets
	 * \retval  0 falls Paket erfolgreich empfangen wurde,
	 * \retval -1 andernfalls
	 */
	int recv_packet(char *pkt_buf, size_t pkt_buf_len, size_t *pkt_len);

	/*! \brief Berechne die Prüfsumme
	 *
	 * \param buf Zeiger auf den Puffer
	 * \param len Größe des Puffers
	 * \return 8-Bit Prüfsumme des Puffers
	 */
	int checksum(const char *buf, size_t len);

	/*! \brief Empfange ein Bestätigung für ein Paket
	 *
	 * \retval  0 eine [positive] Bestätigung (ACK, \c + ) wurde empfangen
	 * \retval  1 eine negative Bestätigung (NACK, \c - ) wurde empfangen
	 * \retval -1 andernfalls
	 */
	int recv_ack(void);

 	/*! \brief Erstelle und sende ein \c OK Paket
	 *
	 * \return Statuscode von send_packet
	 */
	int send_ok_packet();

	/*! \brief Erstelle und sende ein Signalpaket
	 *
	 * beinhaltet aktuellen Interruptvektor (\c S Vektor)
	 *
	 * \return Statuscode von send_packet
	 */
	int send_signal_packet(char *buf, size_t buf_len, char signal);

	/*! \brief Erstelle und sende ein Fehlerpaket (\c E Errorcode)
	 *
	 * \return Statuscode von send_packet
	 */
	int send_error_packet(char *buf, size_t buf_len, char error);

	/*! \brief Lese den Speicherinhalt einer Adresse in den Puffer
	 *
	 * \param buf Zeiger auf den Puffer
	 * \param buf_len Größe des puffers
	 * \param addr Startadresse des zu lesenden Speichers
	 * \param len Größe des zu lesenden Speichers
	 * \param hex Speichere als Hexadezimal (\c true) oder binär (\c false) in den Puffer
	 * \return Anzahl der gelsenen Bytes oder \c -1 falls der Puffer zu klein
	 */
	int mem_read(char *buf, size_t buf_len, address addr, size_t len, bool hex);

	/*! \brief Schreibe den Pufferinhalt an eine Adresse
	 *
	 * \param buf Zeiger auf den Puffer
	 * \param buf_len Größe des puffers
	 * \param addr Startadresse des zu schreibenden Speichers
	 * \param len Größe des zu schreibenden Speichers
	 * \param hex Interpretiere den Pufferinhalt als Hexadezimal (\c true) oder binär (\c false)
	 * \return Anzahl der geschrieben Bytes oder \c -1 falls der Puffer zu gross
	 */
	int mem_write(const char *buf, size_t buf_len, address addr, size_t len, bool hex);

	/*! \brief Setze das Programm am aktuellen Instruktionszeiger fort
	 */
	void sys_continue(void);

	/*! \brief Springe zur nächsten Instruktion
	 */
	 void sys_step(void);

	/*! \brief Installiere Unterbrechungsbehandlung für Trap-Vektoren
	 *
	 * Der Code für die spezifischen Debugger-Unterbrechungsbehandlungen der
	 * Traps ist in Assembler in der `debug/gdb/handler.asm` verfasst. Diese ähneln den
	 * ebenfalls in Assembler verfassten Einsprungscode in der `boot/startup.asm` für
	 * den \ref guardian , sichern jedoch explizit den kompletten Zustand des
	 * Systems (inklusive callee save und Segment-Register) und springen eine
	 * eigene generische Debug-Unterbrechungsbehandlung an: den GDB_Stub::debug_handler
	 *
	 * Da der GDB Stub dynamisch (zur Laufzeit) aktivierbar sein soll, wird vor
	 * dem Initialisieren der Klasse die zentrale Unerbrechungsbehandlungsroutine
	 * \ref guardian für den Vektor verwendet.
	 * Erst durch Aufruf dieser Methode wird die spezifische Debug-
	 * Unterbrechungsbehandlung für den Vektor eingestellt.
	 *
	 * Hierfür muss die Interrupt Deskriptiortabelle (IDT) modifiziert werden:
	 * Beim Eintrag an der für den Vektor entsprechenden Stelle (Gate) wird die
	 * Adresse der Behandlungsfunktion geändert. Diese zeigt ursprünglich auf
	 *    \c irq_entry_%VEKTORNUMMER (definiert in der `boot/startup.asm`),
	 * wird nun jedoch auf die Adresse des `debug/gdb/handler.asm`-Äquivalents
	 *    \c dbg_irq_entry_%VEKTORNUMMER
	 * geändert.
	 *
	 *  \opt Methode in der Datei debug/gdb/init.cc implementieren
	 *
	 * \param vector der zu ändernde Trap-Vektor
	 * \retval true bei erfolgreicher Änderung der Unterbrechungsbehandlungsroutine
	 * \retval false bei Fehler (z.B. ungültige Vektornummer)
	 */
	bool install_handler(int vector);
};

