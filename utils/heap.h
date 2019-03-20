#pragma once

/*! \file
 *  \brief Enthält einen einfachen (SP) Allokator
 */

#include "types.h"

static const size_t HEAP_SIZE = 1024 * 1024 * 32; // 32 MiB

struct HeapStats
{
	size_t free;
	size_t free_blocks;
	size_t used;
	size_t used_blocks;
};

/*! \brief Allokiere Speicher und gib einen Zeiger darauf zurück.
 *  Der Speicher ist nicht bereinigt (genullt).
 *
 * \param size Größe in Bytes, die allokiert werden soll
 * \return Zeiger auf Speicher oder nullptr bei Fehler
 */
extern "C" void *malloc(size_t size);

/*! \brief Einen zuvor allokierten Speicher freigeben
 *
 * \param mem Zeiger auf allokierten Speicher
 */
extern "C" void free(void *mem);

/*! \brief Allokiere Speicher für ein Array und gib einen Zeiger darauf zurück.
 *  Der Speicher ist bereinigt (genullt).
 *
 * \param nmemb Anzahl der zu allokierenden Elemente
 * \param size Größe eines Elements in Bytes
 * \return Zeiger auf Speicher oder nullptr bei Fehler
 */
extern "C" void *calloc(size_t nmemb, size_t size);

/*! \brief Größe des allokierten Speichers ändern.
 *  Der Inhalt des Speichers wird bis zur angegebenen Grüße nicht verändert.
 *
 * \param ptr Pointer auf allokierten Speicher (oder nullptr für zu malloc()
 *            äquivalentes Verhalten)
 * \param size neue Größe des Speichers
 * \return Zeiger auf neuen Speicher oder nullptr bei Fehler
 */
extern "C" void *realloc(void *ptr, size_t size);

void *operator new(size_t size);
