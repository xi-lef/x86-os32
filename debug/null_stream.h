// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Hier ist die Klasse Null_Stream implementiert.
 *  Sie kann statt der Klasse O_Stream verwendet werden, wenn -- zum Zwecke
 *  der Entkäferung -- alle Ausgaben ignoriert werden sollen.
 *  \ingroup io
 */

#pragma once

/*! \brief Die Klasse Null_Stream ignoriert alle mittels Streamoperator
 *  übergebenen Daten.
 *
 *  Sie kann statt der Klasse O_Stream verwendet werden, wenn -- zum Zwecke
 *  der Entkäferung -- alle Ausgaben ignoriert werden sollen, wie z.B. bei der
 *  VERBOSE-Ausgabe
 *
 *  Durch die Verwendung von Template-Programmierung reichen hier zwei
 *  generische Methoden aus (welche einfach alles verwerfen),
 *
 *  \ingroup debug
 */
class Null_Stream {
public:

	/*! \brief Konstruktor
	 */
	Null_Stream(){};

	/*! \brief Übergabe eines beliebigen Typs
	 *  Verwendet Templates für eine generische & kurze Lösung
	 *  \param value zu ignorierendes Datum
	 *  \tparam T Typ des zu ignorierenden Datums
	 *  \return Referenz auf ein Null_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	template <typename T>
	Null_Stream& operator << (__attribute__((unused)) T value){
		return *this;
	};

	/*! \brief Aufruf einer [O_Stream] Manipulatorfunktion
	 *  \param f ignorierte Manipulatorfunktion
	 *  \return Referenz auf ein Null_Stream Objekt, um Operatoren konkatenieren zu können.
	 */
	template <typename T>
	Null_Stream& operator <<(__attribute__((unused)) T& (*f) (T&)){
		return *this;
	}
};

extern Null_Stream nullstream;
