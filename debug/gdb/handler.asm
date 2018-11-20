;
; Copyright (C) 2016  Matt Borgerson
; Copyright (C) 2018  Bernhard Heinloth
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License along
; with this program; if not, write to the Free Software Foundation, Inc.,
; 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
;

bits 32

%define NUM_HANDLERS 32

section .data
global dbg_irq_entries

; Erstelle eine Tabelle (Array) mit den Adressen der spezifischen Unterbrechungsbehandlungen
dbg_irq_entries:
%macro dbg_irq_addr 1
	dd dbg_irq_entry_%1
%endmacro
%assign i 0
%rep NUM_HANDLERS
	dbg_irq_addr i
	%assign i i+1
%endrep

section .text
; Die generische Unterbrechungsbehandlung ist (analog zum guardian) eine C Funktion
extern debug_handler

; Debug-Unterbrechungsbehandlungen
; Analog zu den normalen Unterbrechungsbehandlungen in der startup.asm,
; jedoch müssen hier mehr (alle) Register gesichert werden.
;; DBGIRQ <irq-num> <error-code?>
%macro DBGIRQ 2
align 8
dbg_irq_entry_%1:
	; CPU sichert
	; - EFLAGS
	; - CS
	; - EIP
	; - ERROR CODE, falls vorhanden (bei Vektor 8, 10-14 und 17)
	%if %2 == 0
		; Falls es keinen ERROR CODE gibt, setzen wir einen Platzhalter
		; (damit der Stack identisch aufgebaut ist)
		push 0
	%endif

	; - VECTOR (Interrupt-Nummer)
	push %1

	pushad
	; sichert alle General-Purpose Register:
	; - EAX
	; - ECX
	; - EDX
	; - EBX
	; - ESP
	; - EBP
	; - ESI
	; - EDI

	push ds
	push es
	push fs
	push gs
	push ss
	; damit haben wir auch die Segment-Register:
	; - DS
	; - ES
	; - FS
	; - GS
	; - SS

	mov ebp, esp
	push ebp
	; statt guardian rufen wir unseren debug-Interrupthandler auf
	call debug_handler

	; CPU Kontext vom Stack nehmen
	mov esp, ebp
	pop ss
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp, 8 ; wegen ERROR CODE und VECTOR
	iret
%endmacro

; Macro für jeden Trap aufrufen (kann aber auch händisch gemacht werden, wie in der startup.asm)
%assign i 0
%rep NUM_HANDLERS
	%if (i == 8) || ((i >= 10) && (i <= 14)) || (i == 17)
		; mit ERROR CODE
		DBGIRQ i, 1
	%else
		; ohne ERROR CODE
		DBGIRQ i, 0
	%endif
	%assign i i+1
%endrep

