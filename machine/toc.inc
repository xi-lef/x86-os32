; vim: set et ts=4 sw=4:

; Die folgenden Angaben belegen ebx_offset mit dem Wert 0, esi_offset mit 4
; edi_offset mit 8 usw. Dies entspricht den Abständen der Elemente ebx, esi,
; edi usw. der Struktur toc zum Anfang der Struktur (in Bytes).
; Die Reihenfolge der Registerbezeichnungen muss unbedingt mit der von
; der Struktur toc übereinstimmen.

[ABSOLUTE 0]
ebx_offset:	resd 1
esi_offset:	resd 1
edi_offset:	resd 1
ebp_offset:	resd 1
esp_offset:	resd 1
