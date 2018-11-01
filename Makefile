# vim: set et ts=4 sw=4:
# -----------------------------------------------------------------------------
#
#                              M A K E F I L E
#
# Zum Uebungsbetriebssystem OO/MP-StuBS der Lehrveranstaltung Betriebssysteme.
# -----------------------------------------------------------------------------
#
# Durch Eingabe von 'make' werden die Quelltexte des Systems kompiliert.
# Es gibt Targets in mehreren Varianten, <name>, <name>-noopt sowie
# <name>-verbose.
# Targets mit dem Suffix -noopt werden ohne Optimierungen kompiliert. Dies
# kann sich z.B. für das Debugging mit gdb eignen.
# Bei dem Suffix -verbose werden umfangreiche Ausgaben (über den Ausgabestrom
# DBG_VERBOSE) angezeigt, was beim printf-Debugging hilfreich sein kann.
#
# Folgende Targets sind verfügbar (jeweils auch mit -noopt):
# all:      Das System wird gebaut und liegt anschließend als ELF-Binary vor.
#
# kvm:      KVM wird gestartet und führt euer System in einer
#           Hardware-Beschleunigten virtuellen Maschine aus. Dieses Target ruft
#           aus Sicherhietsgründen ein CIP-Pool spezifisches Script auf.
#
# netboot:  Das System wird in das Netzwerkverzeichnis kopiert und kann auf den
#           Testrechner gestartet werden
#
# iso:      Erstellt ein bootfähiges Abbild für den Start auf Testrechner
#           mittels externe Datenträger
#
# qemu:     QEMU wird gestartet und führt euer System aus. Da QEMU langsamer
#           ist als KVM treten hier u.U. andere Fehler hervor als in KVM oder
#           auf der Hardware.
#
# qemu-iso: QEMU start und führt euer System von der ISO (als CD eingelegt) aus
#
# qemu-gdb: QEMU wird mit aktiviertem GDB-Stub gestartet und eine GDB-Instanz
#           angehängt, damit lässt sich euer System Schritt für Schritt
#           debuggen
#
# qemu-ddd: Wie qemu-gdb nur dient hier DDD als Frontend.
#
# help:     Zeigt eine umfangreiche Hilfe an
#
# -----------------------------------------------------------------------------
# Rewrite 02/2014: gabor
# -----------------------------------------------------------------------------

# --------------------------------------------------------------------------
# Default target

.DEFAULT_GOAL = all

# --------------------------------------------------------------------------
# Zu uebersetzende Quelldateien (Wichtig: vor common.mk definieren!)
CC_SOURCES = $(shell find . -name "*.cc" -and ! -name '.*' -and ! -path './test*' -and ! -path './fs/tool/*')
ASM_SOURCES = $(shell find . -name "*.asm"  -and ! -name '.*' -and ! -name "startup.asm")
STARTUP_SOURCE = startup.asm
STARTUP_OBJECT = $(OBJDIR)/_startup.o
# Pfad zum Ordner für Dateien für die initiale Ramdisk (für Aufgabe 7)
INITRD_DIR ?= initrd/
INITRD_TOOL ?= fs/tool/fstool
INITRD_DEP =
# 1MB Freier Speicher
INITRD_FREE ?= 1048576

# -----------------------------------------------------------------------------
# Parameter:
KERNEL = $(OBJDIR)/system
ISOFILE = $(OBJDIR)/stubs.iso

# --------------------------------------------------------------------------
# Globale Variablen und Standard-Rezepte einbinden
include common.mk

# -----------------------------------------------------------------------------
# Initiale Ramdisk
ifneq ($(wildcard $(INITRD_DIR)*),)
INITRD = $(OBJDIR)/initrd.img
INITRD_DEP += $(shell find $(INITRD_DIR) -type f )
# zusätzliche Abhängigkeit für Kernel definieren
$(KERNEL): $(INITRD)
endif

all: $(KERNEL)

# Rezept zum Linken des Systemimages
$(KERNEL): $(STARTUP_OBJECT) $(OBJPRE) $(MAKEFILE_LIST)
	@echo "LD		$@"
	@if test \( ! \( -d $(@D) \) \) ;then mkdir -p $(@D);fi
	$(VERBOSE) $(LD) -e startup -T boot/sections.ld -o $(KERNEL) $(LDFLAGS) $(STARTUP_OBJECT) $(LDHEAD) $(OBJPRE) $(LDTAIL) $(LDLIBS)

# Tool zum Bearbeiten eines Minix-v3 Abbildes (Aufgabe 7)
$(INITRD_TOOL): $(shell test -d $(dir $(INITRD_TOOL)) && find $(dir $(INITRD_TOOL)) -name "*.cc" -or -name '*.h')
	@echo "Make		$@"
	@make -C $(dir $(INITRD_TOOL))

# Ramdisk mit Minix v3 Dateisystem
$(INITRD): $(INITRD_TOOL) $(INITRD_DEP)
	@echo "INITRD		$@"
	@dd if=/dev/zero of=$@ bs=$(shell du -s $(INITRD_DIR) | cut -f1 | xargs expr $(INITRD_FREE) + ) count=1
	@/sbin/mkfs.minix -3 $@  # optional --inodes <number>
	@./$(INITRD_TOOL) put "$(INITRD_DIR)" $@
