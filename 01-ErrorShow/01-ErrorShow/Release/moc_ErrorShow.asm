; Listing generated by Microsoft (R) Optimizing Compiler Version 19.23.28105.4 

	TITLE	C:\Users\lfish\Projects\WinViaCppStudy\01-ErrorShow\01-ErrorShow\GeneratedFiles\Release\moc_ErrorShow.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB MSVCRT
INCLUDELIB OLDNAMES

EXTRN	__imp_?staticMetaObject@QWidget@@2UQMetaObject@@B:BYTE
CONST	SEGMENT
?qt_meta_stringdata_ErrorShow@@3Uqt_meta_stringdata_ErrorShow_t@@B DD 0ffffffffH ; qt_meta_stringdata_ErrorShow
	DD	09H
	DD	00H
	DD	010H
	DB	045H
	DB	072H
	DB	072H
	DB	06fH
	DB	072H
	DB	053H
	DB	068H
	DB	06fH
	DB	077H
	DB	00H
	ORG $+2
?qt_meta_data_ErrorShow@@3QBIB DD 08H			; qt_meta_data_ErrorShow
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
	DD	00H
CONST	ENDS
PUBLIC	?metaObject@ErrorShow@@UBEPBUQMetaObject@@XZ	; ErrorShow::metaObject
PUBLIC	?qt_metacast@ErrorShow@@UAEPAXPBD@Z		; ErrorShow::qt_metacast
PUBLIC	?qt_metacall@ErrorShow@@UAEHW4Call@QMetaObject@@HPAPAX@Z ; ErrorShow::qt_metacall
PUBLIC	?qt_static_metacall@ErrorShow@@CAXPAVQObject@@W4Call@QMetaObject@@HPAPAX@Z ; ErrorShow::qt_static_metacall
PUBLIC	?staticMetaObject@ErrorShow@@2UQMetaObject@@B	; ErrorShow::staticMetaObject
EXTRN	__imp_?dynamicMetaObject@QObjectData@@QBEPAUQMetaObject@@XZ:PROC
EXTRN	__imp_?qt_metacast@QWidget@@UAEPAXPBD@Z:PROC
EXTRN	__imp_?qt_metacall@QWidget@@UAEHW4Call@QMetaObject@@HPAPAX@Z:PROC
CRT$XCU	SEGMENT
??staticMetaObject$initializer$@ErrorShow@@2P6AXXZA@@3P6AXXZA DD FLAT:??__E?staticMetaObject@ErrorShow@@2UQMetaObject@@B@@YAXXZ ; ??staticMetaObject$initializer$@ErrorShow@@2P6AXXZA@@3P6AXXZA
CRT$XCU	ENDS
_DATA	SEGMENT
?staticMetaObject@ErrorShow@@2UQMetaObject@@B DB 4 DUP(00H) ; ErrorShow::staticMetaObject
	DD	FLAT:?qt_meta_stringdata_ErrorShow@@3Uqt_meta_stringdata_ErrorShow_t@@B
	DD	FLAT:?qt_meta_data_ErrorShow@@3QBIB
	DD	FLAT:?qt_static_metacall@ErrorShow@@CAXPAVQObject@@W4Call@QMetaObject@@HPAPAX@Z
	DD	00H
	DD	00H
_DATA	ENDS
; Function compile flags: /Ogtp
;	COMDAT ??__E?staticMetaObject@ErrorShow@@2UQMetaObject@@B@@YAXXZ
text$di	SEGMENT
??__E?staticMetaObject@ErrorShow@@2UQMetaObject@@B@@YAXXZ PROC ; `dynamic initializer for 'ErrorShow::staticMetaObject'', COMDAT
; File C:\Users\lfish\Projects\WinViaCppStudy\01-ErrorShow\01-ErrorShow\GeneratedFiles\Release\moc_ErrorShow.cpp
; Line 67
	mov	eax, DWORD PTR __imp_?staticMetaObject@QWidget@@2UQMetaObject@@B
	mov	DWORD PTR ?staticMetaObject@ErrorShow@@2UQMetaObject@@B, eax
; Line 72
	ret	0
??__E?staticMetaObject@ErrorShow@@2UQMetaObject@@B@@YAXXZ ENDP ; `dynamic initializer for 'ErrorShow::staticMetaObject''
text$di	ENDS
; Function compile flags: /Ogtp
;	COMDAT ?qt_static_metacall@ErrorShow@@CAXPAVQObject@@W4Call@QMetaObject@@HPAPAX@Z
_TEXT	SEGMENT
__o$ = 8						; size = 4
__c$ = 12						; size = 4
__id$ = 16						; size = 4
__a$ = 20						; size = 4
?qt_static_metacall@ErrorShow@@CAXPAVQObject@@W4Call@QMetaObject@@HPAPAX@Z PROC ; ErrorShow::qt_static_metacall, COMDAT
; File C:\Users\lfish\Projects\WinViaCppStudy\01-ErrorShow\01-ErrorShow\GeneratedFiles\Release\moc_ErrorShow.cpp
; Line 64
	ret	0
?qt_static_metacall@ErrorShow@@CAXPAVQObject@@W4Call@QMetaObject@@HPAPAX@Z ENDP ; ErrorShow::qt_static_metacall
_TEXT	ENDS
; Function compile flags: /Ogtp
;	COMDAT ?qt_metacall@ErrorShow@@UAEHW4Call@QMetaObject@@HPAPAX@Z
_TEXT	SEGMENT
__c$ = 8						; size = 4
__id$ = 12						; size = 4
__a$ = 16						; size = 4
?qt_metacall@ErrorShow@@UAEHW4Call@QMetaObject@@HPAPAX@Z PROC ; ErrorShow::qt_metacall, COMDAT
; _this$ = ecx
; File C:\Users\lfish\Projects\WinViaCppStudy\01-ErrorShow\01-ErrorShow\GeneratedFiles\Release\moc_ErrorShow.cpp
; Line 90
	push	ebp
	mov	ebp, esp
; Line 93
	pop	ebp
; Line 91
	jmp	DWORD PTR __imp_?qt_metacall@QWidget@@UAEHW4Call@QMetaObject@@HPAPAX@Z
?qt_metacall@ErrorShow@@UAEHW4Call@QMetaObject@@HPAPAX@Z ENDP ; ErrorShow::qt_metacall
_TEXT	ENDS
; Function compile flags: /Ogtp
;	COMDAT ?qt_metacast@ErrorShow@@UAEPAXPBD@Z
_TEXT	SEGMENT
__clname$ = 8						; size = 4
?qt_metacast@ErrorShow@@UAEPAXPBD@Z PROC		; ErrorShow::qt_metacast, COMDAT
; _this$ = ecx
; File C:\Users\lfish\Projects\WinViaCppStudy\01-ErrorShow\01-ErrorShow\GeneratedFiles\Release\moc_ErrorShow.cpp
; Line 82
	push	ebp
	mov	ebp, esp
	push	esi
; Line 83
	mov	esi, DWORD PTR __clname$[ebp]
	push	edi
	mov	edi, ecx
	test	esi, esi
	jne	SHORT $LN2@qt_metacas
	pop	edi
	xor	eax, eax
; Line 87
	pop	esi
	pop	ebp
	ret	4
$LN2@qt_metacas:
; Line 84
	mov	eax, OFFSET ?qt_meta_stringdata_ErrorShow@@3Uqt_meta_stringdata_ErrorShow_t@@B+16
	mov	ecx, esi
	npad	3
$LL5@qt_metacas:
	mov	dl, BYTE PTR [ecx]
	cmp	dl, BYTE PTR [eax]
	jne	SHORT $LN6@qt_metacas
	test	dl, dl
	je	SHORT $LN7@qt_metacas
	mov	dl, BYTE PTR [ecx+1]
	cmp	dl, BYTE PTR [eax+1]
	jne	SHORT $LN6@qt_metacas
	add	ecx, 2
	add	eax, 2
	test	dl, dl
	jne	SHORT $LL5@qt_metacas
$LN7@qt_metacas:
	xor	eax, eax
	jmp	SHORT $LN8@qt_metacas
$LN6@qt_metacas:
	sbb	eax, eax
	or	eax, 1
$LN8@qt_metacas:
	test	eax, eax
	jne	SHORT $LN3@qt_metacas
; Line 85
	mov	eax, edi
	pop	edi
; Line 87
	pop	esi
	pop	ebp
	ret	4
$LN3@qt_metacas:
; Line 86
	push	esi
	mov	ecx, edi
	call	DWORD PTR __imp_?qt_metacast@QWidget@@UAEPAXPBD@Z
	pop	edi
; Line 87
	pop	esi
	pop	ebp
	ret	4
?qt_metacast@ErrorShow@@UAEPAXPBD@Z ENDP		; ErrorShow::qt_metacast
_TEXT	ENDS
; Function compile flags: /Ogtp
;	COMDAT ?metaObject@ErrorShow@@UBEPBUQMetaObject@@XZ
_TEXT	SEGMENT
?metaObject@ErrorShow@@UBEPBUQMetaObject@@XZ PROC	; ErrorShow::metaObject, COMDAT
; _this$ = ecx
; File C:\Users\lfish\Projects\WinViaCppStudy\01-ErrorShow\01-ErrorShow\GeneratedFiles\Release\moc_ErrorShow.cpp
; Line 78
	mov	ecx, DWORD PTR [ecx+4]
	cmp	DWORD PTR [ecx+24], 0
	je	SHORT $LN3@metaObject
	jmp	DWORD PTR __imp_?dynamicMetaObject@QObjectData@@QBEPAUQMetaObject@@XZ
$LN3@metaObject:
	mov	eax, OFFSET ?staticMetaObject@ErrorShow@@2UQMetaObject@@B ; ErrorShow::staticMetaObject
; Line 79
	ret	0
?metaObject@ErrorShow@@UBEPBUQMetaObject@@XZ ENDP	; ErrorShow::metaObject
_TEXT	ENDS
END