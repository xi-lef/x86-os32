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
    mov eax, [esp + 4]
    mov ebx, [eax + ebx_offset]
    mov esi, [eax + esi_offset]
    mov edi, [eax + edi_offset]
    mov ebp, [eax + ebp_offset]
    mov esp, [eax + esp_offset]

    ret

; TOC_SWITCH : Threadumschaltung. Der aktuelle Registersatz wird     
;              gesichert und der Registersatz des neuen "thread of control"
;              wird in den Prozessor eingelesen.  
;
; C Prototyp: void toc_switch (struct toc *regs_now,
;                              struct toc *reg_then);

align 8
toc_switch:
    ; save current toc
    mov eax, [esp + 4]
    mov [eax + ebx_offset], ebx
    mov [eax + esi_offset], esi
    mov [eax + edi_offset], edi
    mov [eax + ebp_offset], ebp
    mov [eax + esp_offset], esp

    ; load new toc
    mov eax, [esp + 8]
    mov ebx, [eax + ebx_offset]
    mov esi, [eax + esi_offset]
    mov edi, [eax + edi_offset]
    mov ebp, [eax + ebp_offset]
    mov esp, [eax + esp_offset]

    ; jump to new thread
    ret
