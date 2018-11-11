// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse zum Zugriff auf den CGA_Screen
 */

#pragma once

/*! \brief Abstraktion des CGA-Textmodus.
 *  \ingroup io
 *
 *
 *  Mit Hilfe dieser Klasse kann man auf den Bildschirm des PCs zugreifen.
 *  Der Zugriff erfolgt direkt auf der Hardwareebene, d.h. über den
 *  Bildschirmspeicher bzw. die I/O-Ports der Grafikkarte.
 *
 *  Die Implementierung soll es dabei ermöglichen die Ausgaben des CGA_Screens
 *  nur auf einem Teil des kompletten CGA-Bildschrims darzustellen, einem
 *  in seiner Position und Größe festgelegtem Fenster (mit eigenem Cursor).
 *  Dadurch ist es möglich die Ausgaben des Programms und etwaige Debugausgaben
 *  auf dem Bildschrim zu trennen, ohne synchronisieren zu müssen.
 */
class CGA_Screen
{
private:
	// Verhindere Kopien und Zuweisungen
	CGA_Screen(const CGA_Screen&)            = delete;
	CGA_Screen& operator=(const CGA_Screen&) = delete;

    int cur_x;
	int cur_y;

public:
    const int from_col;
    const int to_col;
    const int from_row;
    const int to_row;
    const bool use_cursor;

	/*! \brief Konstruktor
	 *
	 *  Der CGA_Screen spannt ein Fenster auf der CGA-Bildschirmfläche auf, dass
	 *  durch das Rechteck colums*rows beschrieben wird.
	 *  \param from_col Fensterrechteck beginnt in Spalte from_col
	 *  \param to_col Fensterrechteck erstreckt sich bis Spalte to_col (inklusive)
	 *  \param from_row Fensterrechteck beginn in Zeile from_row
	 *  \param to_row Fensterrechteck erstreckt sich bis Zeile to_row (inklusive)
	 *  \param use_cursor Gibt an, ob der CGA Hardwarecursor verwendet werden
	 *  soll. Defaultmässig ist dies nicht der Fall.
	 *
	 *  \todo Konstruktor implementieren
	 *
	 */
	CGA_Screen(int from_col, int to_col, int from_row, int to_row, bool use_cursor=false);

	/// Groesse des kompletten CGA-Bildschirms
	static const int ROWS = 25;    // 25 Zeilen
	static const int COLUMNS = 80; // 80 Spalten

	/*! \brief CGA-Farben
	 *
	 *  Konstanten fuer die moeglichen Farben im Attribut-Byte. Für die
	 *  Hintergrundfarbe ist nur \c BLACK bis \c LIGHT_GREY moeglich.
	 */
	typedef enum {
		BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, LIGHT_GREY,
		DARK_GREY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN, LIGHT_RED,
		LIGHT_MAGENTA, YELLOW, WHITE
	} color;

	/*! \brief Struktur für [Farb]Attribute eines Zeichens
	 *
	 * Die Verwendung einer <a href="https://en.cppreference.com/w/cpp/language/bit_field">Bitfeldstruktur</a>
	 * erleichtert den Zugriff deutlich
	 */
	struct Attribute {
		int foreground : 4; // .... XXXX Vordergrundfarbe
		int background : 3; // .XXX .... Hintergrundfarbe
		int blink      : 1; // X... .... Blinken

		/*! \brief Attributkonstruktor
		 *
		 *  \todo Konstruktor fertig implementieren
		 *
		 *  \param foreground Vordergrundfarbe (Standard LIGHT_GREY)
		 *  \param background Hintergrundfarbe (Standard BLACK)
		 *  \param blink blinkend oder nicht (Standard kein blinken)
		 */
		Attribute(color foreground = LIGHT_GREY, color background = BLACK, bool blink = false) :
            foreground(foreground & 0xf), background(background & 0x7), blink(int(blink) & 0x1) {}

        operator char() const {
            return (foreground | (background << 4) | (blink << 7));
        }
	} __attribute__((packed)); // sorgt dafür, dass der Übersetzter keinen Speicher auffüllt

	/*! \brief Setzen des Cursors im Fenster auf Spalte \p x und Zeile \p y.
	 *
	 *  Abhängig vom Konstruktorparameter \c use_cursor wird hier entweder der
	 *  CGA Hardwarecursor verwendet, oder die Position intern im Objekt
	 *  gespeichert.
	 *
	 *  \todo Methode unter Verwendung der Klasse IO_Port implementieren
	 *  \opt Negative x- bzw y-Werte sollen den Cursor relativ vom Fensterrand
	 *       (links bzw unten) positionieren
	 *
	 *  \param x Spalte im Fenster
	 *  \param y Zeile im Fenster
	 */
	void setpos (int x, int y);

	/*! \brief Abfragen der Cursorpostion
	 *
	 * Abhängig vom Konstruktorparameter \c use_cursor wird hier der
	 * CGA Hardwarecursor oder die inter gespeicherte Position verwendet.
	 *
	 *  \todo Methode implementieren
	 *
	 *  \param x Spalte im Fenster
	 *  \param y Zeile im Fenster
	 */
	void getpos (int& x, int& y);

    /*! \brief Bewegt den Bildschirminhalt eine Zeile nach oben
     *
     * Bewegt den Bildschirminhalt eine Zeile nach oben.
     *
     * Hilfsmethode für print.
     */
    void move_up_one_line(void);
    
    /*! \brief Line Feed.
     *
     * Bewegt den Cursor in die linkeste Spalte der nächsten Zeile.
     * Dies Berücksichtigt auch den Fall, in der letzten Zeile zu sein,
     * in welchem move_up_one_line verwendet wird.
     *
     * Hierfür wird die Cursorposition in den Argumenten übergeben,
     * um setpos() nicht so häufig aufrufen zu müssen.
     *
     * Hilfsmethode für print.
     *
     * \param x x-Position des Cursors
     * \param y y-Position des Cursors
     */
    void LF(int& x, int& y);

	/*! \brief Anzeige mehrerer Zeichen im Fenster ab der aktuellen Cursorposition
	 *
	 *  Mit dieser Methode kann eine Zeichenkette \p string ausgegeben werden, wobei
	 *  an der aktuellen Position des Cursors begonnen wird. Da die Zeichenkette
	 *  anders als sonst bei C üblich keine Nullterminierung zu enthalten braucht,
	 *  wird der Parameter \p length benötigt, der angeben muss, aus wievielen Zeichen
	 *  string besteht. Nach Abschluss der Ausgabe soll der Cursor hinter dem zuletzt
	 *  ausgegebenen Zeichen stehen. Der gesamte Text soll einheitlich mit den durch
	 *  \p attrib gewählten Farben dargestellt werden.
	 *
	 *  Wenn bis zum Ende der Zeile nicht mehr genügend Platz ist, soll die Ausgabe
	 *  auf der folgenden Zeile fortgesetzt werden. Sobald die letzte Fensterzeile
	 *  gefüllt ist, soll der gesamte Fensterbereich um eine Zeile nach oben
	 *  geschoben werden. Dadurch verschwindet die erste Zeile. Dafür kann nun die
	 *  letzte Zeile erst gelöscht und dann die Ausgabe dort fortgesetzt werden.
	 *
	 *  Ein Zeilenumbruch muss auch erfolgen, wann immer das Zeichen \c \\n im
	 *  auszugebenen Text enthalten ist.
	 *
	 *  \todo Methode implementieren
	 *
	 *  \param string Auszugebende Zeichenkette
	 *  \param length Länge der Zeichenkette
	 *  \param attrib Farbattribut zur Darstellung
	 */
	void print (char* string, int length, Attribute attrib = Attribute());

	/*! \brief Löschen des Inhalts und Zurücksetzen des Cursors
	 *
	 *  \todo Methode implementieren
	 *
	 *  \param character Füllzeichen
	 *  \param attrib Zeichenattribut
	 */
	void reset(char character=' ', Attribute attrib = Attribute());

	/*! \brief Grundlegende Anzeige eines Zeichens mit Attribut an einer bestimmten
	 * Stelle auf dem kompletten CGA-Bildschirm.
	 *
	 *  Diese Methode gibt das Zeichen \p character an der absoluten Position
	 *  (\p x, \p y) mit dem angegebenen Farbattribut attrib aus. Dabei gibt \p x die
	 *  Spalte und \p y die Zeile der gewünschten Position an, wobei gilt:
	 *  0 <= \p x <= 79 und 0 <= \p y <= 24. Die Position (0,0) bezeichnet die linke
	 *  obere Ecke des Bildschirms. Mit \p attrib lassen sich Merkmale wie
	 *  Hintergrundfarbe, Vordergrundfarbe und Blinken festlegen.
	 *
	 *  \todo Methode implementieren
	 *  \opt Negative x- bzw y-Werte sollen den Cursor relativ vom Bildschirmrand
	 *       (links bzw unten) positionieren
	 *
	 *  \param x Bildschirmspalte, in der das Zeichen dargestellt werden soll
	 *  \param y Bildschirmzeile, in der das Zeichen dargestellt werden soll
	 *  \param character Zeichen, welches dargestellt werden soll
	 *  \param attrib Farbattribut zur Darstellung
	 */
	static void show (int x, int y, char character, Attribute attrib = Attribute());

};

