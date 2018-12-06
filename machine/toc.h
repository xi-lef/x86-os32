// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Struktur 'Thread of context'.
 */

#pragma once

/*! \brief Die Struktur toc dient dazu, bei einem Koroutinenwechsel die Werte
 *  der nicht-flüchtigen Register zu sichern.
 *
 *  Beim GNU C Compiler sind `eax`,  `ecx` und `edx` flüchtige Register, die bei
 *  Funktionsaufrufen und somit auch bei einem Koroutinenwechsel keine später
 *  noch benötigten Werte haben duerfen. Daher muss in der Struktur toc auch
 *  kein Platz für sie bereitgestellt werden.
 *
 *  \note Für den Zugriff auf die Elemente der Struktur ::toc aus einer
 *  Assemblerfunktion heraus werden in der Datei *toc.inc* Namen für die
 *  benötigten Abstände der einzelnen Elemente zum Anfang der Struktur
 *  definiert. Damit dann auch auf die richtigen Elemente zugegriffen wird,
 *  müssen sich die Angaben von toc.h und *toc.inc* exakt entsprechen. Wer also
 *  toc.h ändert, muss auch *toc.inc* anpassen (und umgekehrt).
 */
struct toc {
	public:
	void *ebx;
	void *esi;
	void *edi;
	void *ebp;
	void *esp;
} __attribute__ ((packed));

class Thread;

/*! \brief Diese Funktion bereitet ein struct toc für die erste Aktivierung
 *  vor.
 *
 *  Dazu müssen Registerinhalte und Stack so initialisiert werden, dass bei
 *  der ersten Aktivierung die Ausführung mit der Funktion kickoff beginnt,
 *  die wiederum den Zeiger object als ersten Parameter auf dem Stack
 *  vorfinden muss.
 *
 *  toc_settle kann in C++ in der Datei toc.cc implementiert werden und
 *  deshalb muss sie nicht als extern "C" deklariert werden.
 *
 *  \param regs Kontext, der auf eine Aktivierung vorbereitet werden soll
 *  \param tos Zeiger auf die oberste Adresse des Stackbereichs.
 *  \param kickoff Adresse der Funktion kickoff, mit der die Ausführung
 *  eines Fadens beginnt.
 *  \param object Zeiger auf ein Thread Objekt; Parameter für die
 *  Funktion kickoff.
 *
 *  \todo Funktion implementieren
 *
 */
void toc_settle(struct toc *regs, void *tos, void (*kickoff)(Thread*),
				Thread *object);

extern "C" {
	/*! \brief Diese Funktion lädt die Prozessorregister mit den Inhalten aus
	 *  \b regs.
	 *
	 *  toc_go wird verwendet, um auf jeder CPU den ersten Thread überhaupt zu
	 *  starten. Die Implementierung von toc_go muss in Assembler in der Datei
	 *  toc.asm erfolgen. Sie muss als extern "C" deklariert werden, da sie als
	 *  Assemblerfunktion nicht dem C++ Namemangeling unterliegt.
	 *  \param regs Initialer Kontext der Koroutine, die gestartet werden soll.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void toc_go(struct toc *regs);
	/*! \brief Diese Funktion führt einen Kontextwechsel durch. Dazu brauchen
	 *  nur die aktuellen Registerwerte in regs_now gesichert und durch die
	 *  Werte von regs_then ersetzt zu werden.
	 *
	 *  Auch toc_switch wird in Assembler in der Datei toc.asm implementiert.
	 *  Sie muss als extern "C" deklariert werden, da sie als
	 *  Assemblerfunktion nicht dem C++ Namemangeling unterliegt.
	 *  \param regs_now Zeiger auf ein struct toc, in den der
	 *  aktuelle Prozessorkontext gesichert werden soll.
	 *  \param reg_then Zeiger auf ein struct toc, der den zu
	 *  ladenden Kontext enthält.
	 *
	 *  \todo Methode implementieren
	 *
	 */
	void toc_switch(struct toc *regs_now, struct toc *reg_then);
}
