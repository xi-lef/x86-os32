// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Stringbuffer
 */

#pragma once

/*! \brief Die Klasse Stringbuffer dient dazu, einzelne Zeichen zu längeren Texten
 *  zusammenzustellen, die dann an einem Stück verarbeitet werden können.
 *
 *  Damit ein möglichst vielseitiger Einsatz möglich ist, trifft die Klasse
 *  keine Annahme darüber, was "verarbeiten" in diesem Zusammenhang bedeutet.
 *  Nur der Zeitpunkt der Verarbeitung steht bereits fest, nämlich immer,
 *  wenn dies explizit gewünscht wird oder der Text so lang geworden ist,
 *  dass keine weiteren Zeichen hinzugefügt werden können. Dies geschieht durch
 *  Aufruf der Methode flush(). Da Stringbuffer geräteunabhängig sein soll,
 *  ist flush() eine virtuelle Methode, die von den abgeleiteten Klassen
 *  definiert werden muss.
 *
 *  \par Hinweise zur Implementierung
 *  Zur Pufferung der Zeichen eignet sich ein fest dimensioniertes Feld, auf
 *  das die abgeleiteten Klassen zugreifen können müssen. Auch die Anzahl der
 *  Zeichen oder das zuletzt beschriebene Feldelement sollte in den
 *  spezialisierten flush() Methoden erkennbar sein.
 *
 *  \par Anmerkung
 *  Anlass für die Einführung dieser Klasse war die Erkenntnis, dass Ausgaben
 *  eines Programmes sehr häufig aus vielen kleinen Komponenten bestehen, zum
 *  Beispiel, wenn die Namen und Inhalte von Variablen dargestellt werden
 *  sollen. Andererseits können wenige längere Texte meist viel effizienter
 *  ausgegeben werden als viele kurze. Daher erscheint es sinnvoll, vor der
 *  Ausgabe die einzelnen Komponenten mit Hilfe eines Stringbuffer Objekts
 *  zusammenzufügen und erst später, z. B. bei einem Zeilenumbruch, gesammelt
 *  auszugeben.
 */
class Stringbuffer {
	// Verhindere Kopien und Zuweisungen
	Stringbuffer(const Stringbuffer&)            = delete;
	Stringbuffer& operator=(const Stringbuffer&) = delete;
	// Alle Variablen und Methoden dieser Klasse sind "protected",
	// da die abgeleiteten Klassen einen direkten Zugriff auf den
	// Puffer, den Konstruktor, den Destruktor und die Methode put
	// benötigen. Die Methode flush() muss sowieso neu definiert
	// werden und kann dann auch public werden.

protected:
	/// Zeichenpuffer
	char buffer[80];
	/// Aktuelle Position im Puffer
	int pos;

	/*! \brief Konstruktor; Markiert Puffer als leer.
	 */
	Stringbuffer() {
		pos = 0;
	}

	/*! \brief Fügt das Zeichen c in den Puffer ein.
	 *
	 *  Wenn der Puffer daraufhin voll ist, wird er durch Aufruf der Methode
	 *  flush() geleert.
	 *
	 *  \todo Methode implementieren
	 *
	 *  \param c Einzufügendes Zeichen
	 */
	void put(char c);

	/*! \brief Methode zur Ausgabe des Pufferinhalts
	 *
	 *  Diese Methode muss in den abgeleiteten Klassen definiert werden,
	 *  denn nur diese wissen, wie die Zeichen ausgegeben werden können.
	 *  flush() muss den Positionszeiger pos zurücksetzen.
	 */
	virtual void flush() = 0;
public:
	/*! \brief Destruktor (hier nichts zu tun)
	 */
	virtual ~Stringbuffer() { }
};

