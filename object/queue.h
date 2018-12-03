// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse Queue
 */

#pragma once

#include "object/queuelink.h"

/*! \brief Die Klasse Queue realisiert eine einfach verkettete Liste von
 *  Objekten mit next Zeiger.
 *
 *  Objekte, die verkettet werden sollen, enthalten ein QueueLink<T>-Member,
 *  welches diese Liste identifiziert.
 *  Beispiel:
\verbatim
class Foo {
   QueueLink<Foo> next_foo;
}
 \endverbatim
 *
 *  Das next_foo Feld enthaelt dabei den Verkettungszeiger. Es lassen
 *  sich nun Listen mit dem next_foo Verkettungszeiger generieren und
 *  verwenden. Insbesondere können diese Queues in C++11 Range
 *  Ausdrücken verwendet werden.
 *
 \verbatim
Queue<Foo, &Foo::next_foo> list;
Foo a, b, c;

list.enqueue(&a);
list.enqueue(&b);
list.enqueue(&c);

for(Foo * elem : list) {
  // use elem
}

 \endverbatim
 *  Wird eine Queue ohne Angabe eines expliziten Verkettungszeigers
 *  instanziiert, so muss die zu verkettende Klasse ein QueueLink<T>-Member mit
 *  dem Namen "queue_link" enthalten:
 \verbatim
class Bar {
   QueueLink<Bar> queue_link;
}
Queue<Bar> list;

 \endverbatim
 *
 *  Zur Implementierung: tail verweist nicht, wie oft üblich, auf das letzte
 *  Element der Liste, sondern
 *  auf den next Zeiger des letzten Elements, bzw., solange die Liste noch leer
 *  ist, auf den head Zeiger der Liste. Dadurch muss beim Einfügen eines
 *  Elements an die Liste nicht überprüft werden, ob bereits Elemente in ihr
 *  enthalten sind. Beim Entfernen von Elementen kann auf die Fallunterscheidung
 *  allerdings nicht verzichtet werden.
 */
template<typename T, QueueLink<T> T::* next_field = nullptr>
class Queue
{
	/* Wir erlauben es nicht Warteschlangen zuzuweisen, da ansonsten
	 * der "tail Zeiger auf das "head" Feld einer anderen
	 * Warteschlange zeigt.*/

	Queue(const Queue<T, next_field>&)            = delete;
	Queue& operator=(const Queue<T, next_field>&) = delete;

private:
	T *head;
	T **tail;

public:
	/*! \brief Der Konstruktor initialisiert die Liste als leere Liste
	 */
	Queue() : head(0), tail(&head)  { }

	/*! \brief Gibt einen Zeiger auf das QueueLink-Element
	 *  innerhalb des Argument-Objektes zurück.
	 *
	 * Wir verwenden hier die "ungewöhnliche" Pointer-to-Member
	 * Funktionalität von C++. Ein PtM kann mit dem "->*" Operator auf
	 * ein Objekt der passenden Klasse angewendet werden, um Zugriff
	 * auf ein bestimmtes Feld zu bekommen.
	 */
	static QueueLink<T> *get_node(T *o) {
		return queue_link_get<T, next_field>::call(o);
	}

	/*! \brief Das Listenelement item wird an das Ende der Liste angefügt.
	 *  \param item Listenelement, welches angefügt werden soll.
	 */
	void enqueue(T *item) {
		QueueLink<T> *node = get_node(item); // Verkettungszeiger für das Objekt holen.
		node->next = 0;                      // Das neue Element besitzt noch keinen Nachfolger.
		*tail = item;                        // Das Element an das Ende der Liste anfuegen
		tail = &(node->next);                // und den tail Zeiger aktualisieren.
	}

	/*! \brief Liefert das erste Element der Liste und entfernt es gleichzeitig
	 *  aus dieser.
	 *  \return entferntes Kopfelement oder 0 wenn die Liste leer ist.
	 */
	T* dequeue() {
		T *out;
		if((out = head)) { // Es gibt ein Element

			//  Es ist das letzte Element der Liste
			if (!(head = get_node(out)->next))
				tail = &head;

			// Markiere das Element als Entfernt
			get_node(out)->next = nullptr;
		}
		return out;
	}

	/*! \brief Ein Queue Iterator

	 * Wir implementieren eine Iterator-Wrapper-Klasse, die den
	 * aktuellen Zustand einfängt, wenn wir über eine Queue iterieren.
	 * Dies geschieht automatisch, wenn C++11-Range-Loops verwendet
	 * werden. Queue-Iteratoren sind als Vorwärts-Iteratoren
	 * implementiert. Mehr Details zu Vorwärts-Iteratoren gibt es <a
	 * href="http://en.cppreference.com/w/cpp/concept/ForwardIterator">hier</a>.
	 */
	class Iterator {
		T * first;
	public:
		Iterator() : first(nullptr) {};

		Iterator(Queue<T, next_field> * queue) : first(queue->head) {};

		bool operator!=(const Iterator & other) {
			return first != other.first;
		}

		T * operator*() { return first; }

		Iterator & operator++() {
			first = get_node(first)->next;
			return *this;
		}
	};

	/*! Liefert einen Iterator auf den Beginn der Queue
	 */
	Iterator begin() {
		return Iterator(this);
	}

	/*! Liefert einen End Iterator
	 */
	Iterator end() {
		return Iterator();
	}

	/*! \brief Mit dieser Methode kann das angegebene Element item aus der
	 *  Liste entfernt werden, unabhängig von seiner Position dort.
	 *  Verglichen werden die Pointer-Adressen, optional kann eine
	 *  Vergleichsfunktion angegeben werden. Die
	 *  Standardvergleichsfunktion ist ein C++11 Lambda Ausdruck, der
	 *  auf Pointer-Gleichheit prüft.
	 *
	 *  \param item Element, welches entfernt werden soll.
	 *  \param cmp Vergleichsfunktion.
	 *  \return Gibt das entfernte Element zurück, \b 0 wenn keins entfernt
	 *  wurde.
	 */
	T* remove(T *item, bool (*cmp)(T*,T*) = [] (T* a, T* b) {return a == b;}) {
		T **prev = &head;
		for (T * current : *this) {
			if(cmp(item, current)) {
				*prev = get_node(current)->next;
				if(&get_node(current)->next == tail) tail = prev;
				return current;
			}
			prev = &get_node(current)->next;
		}
		return 0;
	}

	/*! \brief Fügt das \b item am Anfang der Liste ein.
	 *  \param item Einzufügendes Element.
	 */
	void insert_first(T *item) {
		get_node(item)->next = head;
		head = item;

		if(tail == &head) tail = &get_node(item)->next;
	}

	/*! \brief Fügt das Element new_item hinter dem Element old_item in die
	 *  Liste ein.
	 *  \param old_item Element, nach dem eingefügt werden soll.
	 *  \param new_item Einzufügendes Element.
	 */
	void insert_after(T *old_item, T *new_item) {
		get_node(new_item)->next = get_node(old_item)->next;
		get_node(old_item)->next = new_item;

		if((*tail) == new_item)
			tail = &(get_node(new_item)->next);
	}

	/*! \brief Liefert das erste Element der Liste, ohne es zu entfernen.
	 *  \return Erstes Element der Liste.
	 */
	T* first() {
		T *out;
		out = head;
		return out;
	}

	/*! \brief Liefert das nächste Element der Liste für ein gegebenes Element.
	 */
	T* next(T* o) {
		return get_node(o)->next;
	}
};

