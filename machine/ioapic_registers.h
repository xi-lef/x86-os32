// vim: set et ts=4 sw=4:

/*! \file
 *  \brief Strukturen und Makros zum Zugriff auf den IO-APIC
 */

#pragma once

/*! \brief Einstellungsmöglichkeiten und Modi für die IOREDTBL_H und IOREDTBL_L
 *  Register.
 *
 *  siehe: IO-APIC manual, p. 11-13
 */
#define DESTINATION_MODE_PHYSICAL	0x0
/// \copydoc DESTINATION_MODE_PHYSICAL
#define DESTINATION_MODE_LOGICAL	0x1
/// \copydoc DESTINATION_MODE_PHYSICAL
#define TRIGGER_MODE_EDGE			0x0
/// \copydoc DESTINATION_MODE_PHYSICAL
#define TRIGGER_MODE_LEVEL			0x1
/// \copydoc DESTINATION_MODE_PHYSICAL
#define POLARITY_HIGH				0x0
/// \copydoc DESTINATION_MODE_PHYSICAL
#define POLARITY_LOW				0x1
/// \copydoc DESTINATION_MODE_PHYSICAL
#define MASK_ENABLED				0x0
/// \copydoc DESTINATION_MODE_PHYSICAL
#define MASK_DISABLED				0x1
/// \copydoc DESTINATION_MODE_PHYSICAL
#define DELIVERY_MODE_FIXED			0x0
/// \copydoc DESTINATION_MODE_PHYSICAL
#define DELIVERY_MODE_LOWESTPRI		0x1
/// \copydoc DESTINATION_MODE_PHYSICAL
#define DELIVERY_MODE_SMI			0x2
// Reserved			 				0x3
/// \copydoc DESTINATION_MODE_PHYSICAL
#define DELIVERY_MODE_NMI			0x4
/// \copydoc DESTINATION_MODE_PHYSICAL
#define DELIVERY_MODE_INIT			0x5
// Reserved							0x6
/// \copydoc DESTINATION_MODE_PHYSICAL
#define DELIVERY_MODE_EXTINT		0x7

/*! \brief ID des IO-APICs
 *
 *  Das IOAPICID Register liegt an Registernummer 0x0. Die IOAPICID wird
 *  während des Bootvorgangs aus den Systemkonfigurationstabellen des BIOS
 *  gelesen. Sie ist über APICSystem::getIOAPICID() abfragbar. Diese Information
 *  muss bei der Initialisierung des IOAPICs ins IOAPICID Register eingetragen
 *  werden.
 *
 *  siehe: IO-APIC manual, p. 9
 */
struct IOAPICID {
	unsigned int reserved_2:24,
			 ID:4, ///< IOAPIC Identification, R/W
			 reserved_1:4;
} __attribute__((packed));

/*! \brief Eintrag in der IO Redirection Table
 *
 *  Jeder Eintrag in der IO Redirection Table symbolisiert eine externe
 *  Interruptquelle. Für jeden externen Interrupt lassen sich hier folgende
 *  Einstellungen vornehmen:
 *      - \b vector gibt an, welcher Vektor in der Interruptvektortabelle für
 *        den entsprechenden Interrupt aktiviert werden soll.
 *      - \b delivery_mode gibt an, wie die adressierten CPUs (bzw. ihre local
 *        APICs) auf das Interruptsignal reagieren sollen. In OO/MPStubs soll
 *        `DELIVERY_MODE_LOWESTPRI` verwendet werden. Da die einzelnen CPUs alle
 *        dieselbe Priorität besitzen, werden so die auftretenden Interrupts
 *        gleich über alle CPUs verteilt.
 *      - \b destination_mode legt fest, wie \b logical_destination interpretiert
 *        werden soll. In OO/MPStubs soll hier `DESTINATION_MODE_LOGICAL`
 *        verwendet werden.
 *      - \b polarity gibt an, wann ein Interrupt signalisiert werden soll.
 *        OO/MPStubs soll hier `POLARITY_HIGH` verwenden.
 *      - \b trigger_mode gibt an, ob die Interruptsignalisierung pegel- oder
 *        flankengesteuert ist. OO/MPStubs verwendet hier `TRIGGER_MODE_EDGE`
 *        für die Tastatur und den Timer, die (optionale) serielle
 *        Schnittstelle jedoch `TRIGGER_MODE_LEVEL`
 *      - \b mask gibt an, ob für die entsprechende Quelle Interrupts zugelassen
 *        werden sollen oder nicht. `1` maskiert die dem Eintrag entsprechende
 *        Interruptquelle aus, `0` lässt den Interrupt zu.
 *      - \b logical_destination beschreibt als Bitmaske eine Menge von
 *        Prozessoren, die potentiell als Empfänger der Interruptnachricht in
 *        Frage kommen. Für OOStuBS ist dies logischerweise `1`, für MPStuBS
 *        müssen in der Bitmaske die untersten `n` Bits auf `1` gesetzt sein,
 *        wobei `n` die Anzahl der im System vorhandenen CPUs ist (siehe
 *        APICSystem::getNumberOfCPUs()). Damit kommen alle CPUs als Empfänger
 *        infrage und die Interruptlast wird so gleichmäßig über alle CPUs
 *        verteilt.
 *        *Hinweis:* In QEMU funktioniert dies nicht. Dort bekommt immer
 *        `CPU 0`, also der BSP alle Interrupts.
 *
 *  Ein Tabelleneintrag besteht dabei aus zwei 32bit breiten Wörtern IOREDTBL_H
 *  bzw. IOREDTBL_L
 *
 *  Die Redirection Tabelle beginnt bei IOAPIC-Register `0x10` und endet bei
 *  `0x3f`.
 *  Jeder Eintrag umfasst 64 bit, also 2 Register des IOAPICs. IOREDTBL_L ist
 *  niederwertige Wort, IOREDTBL_H das höherwertige. Eintrag 0 in der Tabelle
 *  wären dann die Register `0x10` (entspricht IOREDTBL_L) und
 *  `0x11` (entspricht IOREDTBL_H).
 *
 *  \see [IO-APIC manual](intel_ioapic.pdf), p. 11-13
 */
struct IOREDTBL_L {
	unsigned int vector:8,           ///< Interrupt Vector, R/W
			 delivery_mode:3,    ///< Delivery Mode, R/W
			 destination_mode:1, ///< Destination Mode, R/W
			 delivery_status:1,  ///< Delivery Status, RO
			 polarity:1,         ///< Interrupt Input Pin Polarity, R/W
			 remote_irr:1,       ///< Remote IRR (for level-triggered interrupts only), RO
			 trigger_mode:1,     ///< Trigger Mode, R/W
			 mask:1,             ///< Interrupt Mask, R/W
			 reserved:15;
} __attribute__((packed));

/// \copydoc IOREDTBL_L
struct IOREDTBL_H {
	unsigned int reserved:24,
			 logical_destination:8; ///< Menge von Zielprozessoren
} __attribute__((packed));

/// Union über die einzelnen Registertypen des IO-APICs
union IOAPICRegister {
	struct IOAPICID		IOAPICID;
	struct IOREDTBL_L   IOREDTBL_L;
	struct IOREDTBL_H   IOREDTBL_H;
	unsigned int        value;
} __attribute__((packed));

typedef union IOAPICRegister IOAPICRegister_t;

