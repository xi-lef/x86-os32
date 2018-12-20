; vim: set et ts=4 sw=4:

%include "machine/toc.inc"

; EXPORTIERTE FUNKTIONEN
[GLOBAL toc_switch]
[GLOBAL toc_go]

[SECTION .text]

; IMPLEMENTIERUNG DER FUNKTIONEN

; TOC_GO : Startet den ersten Thread ueberhaupt.
;
; C Prototyp: void toc_go (struct toc *regs);

align 8
toc_go:

; TOC_SWITCH : Threadumschaltung. Der aktuelle Registersatz wird     
;              gesichert und der Registersatz des neuen "thread of control"
;              wird in den Prozessor eingelesen.  
;
; C Prototyp: void toc_switch (struct toc *regs_now,
;                              struct toc *reg_then);

align 8
toc_switch:
