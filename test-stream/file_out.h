// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse FileOut für die C++ Übungsaufgabe
 */

#pragma once

#include "o_stream.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/*! \brief Ausgabe in eine Datei
 *
 *  Die Klasse FileOut ermöglicht eine komfortable Ausgabe in eine Datei nur unter
 *  zu Hilfenahme der elementaren Systemaufrufe `open()` / `write()` / `close()` .
 *  Diese Klasse ist von O_Stream abgeleitet.
 */
class FileOut : public O_Stream
{
	// Verhindere Kopien und Zuweisungen
	FileOut(const FileOut&)            = delete;
	FileOut& operator=(const FileOut&) = delete;
    const char *path;
    int file;
    static int counter;

    public:
	/*! \brief Konstruktor
	 *  Öffnet die Datei mittels Syscall `open()` zum schreiben.
	 *  \param path Pfad zur Ausgabedatei
	 */
	
    FileOut(const char * path) {
        this->path = path;
        file = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        counter++;
    }

	/*! \brief Destruktor
	 *  Schließt die Datei (mittels `close()`)
	 */
	virtual ~FileOut() {
        close(file);
        counter--;
    }

	/*! \brief Rückgabe des Pfades der Ausgabedatei
	 *  \return Pfad der Ausgabedatei (wie im Konstruktor übergeben)
	 */
	const char * getPath() {
        return path;
    }

	/*! \brief Auflisten aller derzeit (mittels dieser Klasse) geöffneten Dateien
	 */
	static int count() {
        return counter;
    }

	/*! \brief Schreiben der Zeichenkette in die geöffnete Datei.
	 *  Die Implementierung soll ausschliesslich den Syscall `write()` verwenden.
	 */
	virtual void flush() {
        write(file, buffer, pos);
        pos = 0;
    }
};

