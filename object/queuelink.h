// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Enthält die Klasse QueueLink
 */

#pragma once

/* Forward declaration: */
template<typename T> class QueueLink;
template<typename T, QueueLink<T> T::*> class Queue;

/*! \brief Verkettungszeiger zum Einfügen eines Objektes in eine einfach
 *  verkettete Liste.
 *
 *  Die Klasse QueueLink stellt einen Verkettungszeiger auf ein Element mit dem
 *  Typ T zur Verfügung und ist damit Basis aller Klassen, deren Instanzen in
 *  Listen (Queue Objekten) verwaltet werden sollen.
 */
template <typename T>
class QueueLink
{
	/* Erlaube Klasse Queue Zugriff auf den privaten Member next. */
	template<typename X, QueueLink<X> X::* x> friend class Queue;

private:
	/*! \brief next Gibt das nächste Element der Liste an.
	 */
	T *next;

public:
	QueueLink<T>() : next(nullptr) {}

};

/* Here we do some template magic to get either the pointer-to-member
 *  field (if != nullptr) or a default QueueLink field with the name
 *  queue_link. As a BS student you're not supposed to understand this
 *  without hard thinking. */
// enable_if is copied from the standard C++ libr
template<bool B, class T = void>
struct enable_if {};

template<class T>
struct enable_if<true, T> { typedef T type; };

template<typename X, QueueLink<X> X::* next_field, class Enable = void>
struct queue_link_get {
	static QueueLink<X> * call(X* o) {
		return &(o->queue_link);
	}
};

template<typename X, QueueLink<X> X::* next_field>
struct queue_link_get<X, next_field, typename enable_if<next_field != nullptr>::type> {
	static QueueLink<X> * call(X* o) {
		return &(o->*next_field);
	}
};

