section .bss

global INTERRUPT_ARRAY
INTERRUPT_ARRAY:
	resb 2048

section	.text
extern	IRQHandle				;
extern	WaitForDebugger		;
extern	GetCurProc				;
extern	GetRealAddress			;

; RDI and RSI must already have been pushed in that order.
common_handler:
	sub		rsp, 16				;
	; 4 bytes for SS and CS		
	mov		[rsp + 4], ds		;
	mov		[rsp + 6], es		;
	mov		[rsp + 8], fs		;
	mov		[rsp + 10], gs		;

	push		rax					;
	push		rbx					;
	push		rcx					;
	push		rdx					;
	push		rbp					;
	push		rsp					; Ignored.
	push		r8						;
	push		r9						;
	push		r10					;
	push		r11					;
	push		r12					;
	push		r13					;
	push		r14					;
	push		r15					;

	; Add an extra 8 bytes for the process ID.
	sub		rsp, 8				;

	push rax;
	push rdi;

	; If the curProc is not null, save the context to it.
	cmp		qword [rsp], 0x70	;
	jne		.irq					;
	call		GetCurProc			;
	cmp		rax, 0				;
	je			.irq					;
		

	; Save the current process into rax.
	mov		rdi, [rsp + 0x08 + 16]	; R15
	mov		[rax + 0x08], rdi			;

	mov		rdi, [rsp + 0x10 + 16]	; R14
	mov		[rax + 0x10], rdi			;

	mov		rdi, [rsp + 0x18 + 16]	; R13
	mov		[rax + 0x18], rdi			;

	mov		rdi, [rsp + 0x20 + 16]	; R12
	mov		[rax + 0x20], rdi			;

	mov		rdi, [rsp + 0x28 + 16]	; R11
	mov		[rax + 0x28], rdi			;

	mov		rdi, [rsp + 0x30 + 16]	; R10
	mov		[rax + 0x30], rdi			;

	mov		rdi, [rsp + 0x38 + 16]	; R9
	mov		[rax + 0x38], rdi			;

	mov		rdi, [rsp + 0x40 + 16]	; R8
	mov		[rax + 0x40], rdi			;

	mov		rdi, [rsp + 0xB0 + 16]	; RSP
	mov		[rax + 0x48], rdi			;

	mov		rdi, [rsp + 0x50 + 16]	; RBP
	mov		[rax + 0x50], rdi			;

	mov		rdi, [rsp + 0x58 + 16]	; RDX
	mov		[rax + 0x58], rdi			;

	mov		rdi, [rsp + 0x60 + 16]	; RCX
	mov		[rax + 0x60], rdi			;

	mov		rdi, [rsp + 0x68 + 16]	; RBX
	mov		[rax + 0x68], rdi			;

	mov		rdi, [rsp + 0x70 + 16]	; RAX
	mov		[rax + 0x70], rdi			;

	mov		rdi, [rsp + 0xB8 + 16]	; SS
	mov		[rax + 0x78], rdi			;
			
	mov		rdi, [rsp + 0xA0 + 16]	; CS
	mov		[rax + 0x7A], rdi			;

	mov		rdi, [rsp + 0x7C + 16]	; DS
	mov		[rax + 0x7C], rdi			;

	mov		rdi, [rsp + 0x7E + 16]	; ES
	mov		[rax + 0x7E], rdi			;

	mov		rdi, [rsp + 0x80 + 16]	; FS
	mov		[rax + 0x80], rdi			;

	mov		rdi, [rsp + 0x82 + 16]	; GS
	mov		[rax + 0x82], rdi			;

	mov		rdi, [rsp + 0x88 + 16]	; RSI
	mov		[rax + 0x88], rdi			;

	mov		rdi, [rsp + 0x90 + 16]	; RDI
	mov		[rax + 0x90], rdi			;

	mov		rdi, [rsp + 0x98 + 16]	; RIP
	mov		[rax + 0x98], rdi	;

	mov		rdi, cr3				; CR3
	mov		[rax + 0xA0], rdi	;

	mov		rdi, [rsp + 0xA8 + 16]	; RFLAGS
	mov		[rax + 0xA8], rdi	;

	; MXCSR


	.irq:
	pop rdi							;
	pop rax							;
	push		rdi					;
	call		IRQHandle			;

	; Check if swapping to a new process...
	pop		rdi					;
	cmp		rdi, 0x70			;
	jne		.restore				;
	call		GetCurProc			;
	cmp		rax, 0				;
	je			.restore				;
	
	.overwrite:
	; RAX is the process to swap in.
	mov		rdi, [rax + 0x08]	; R15
	mov		[rsp + 0x08], rdi	;

	mov		rdi, [rax + 0x10]	; R14
	mov		[rsp + 0x10], rdi	;

	mov		rdi, [rax + 0x18]	; R13
	mov		[rsp + 0x18], rdi	;

	mov		rdi, [rax + 0x20]	; R12
	mov		[rsp + 0x20], rdi	;

	mov		rdi, [rax + 0x28]	; R11
	mov		[rsp + 0x28], rdi	;

	mov		rdi, [rax + 0x30]	; R10
	mov		[rsp + 0x30], rdi	;

	mov		rdi, [rax + 0x38]	; R9
	mov		[rsp + 0x38], rdi	;

	mov		rdi, [rax + 0x40]	; R8
	mov		[rsp + 0x40], rdi	;

	mov		rdi, [rax + 0x48]	; RSP
	mov		[rsp + 0xB0], rdi	;

	mov		rdi, [rax + 0x50]	; RBP
	mov		[rsp + 0x50], rdi	;

	mov		rdi, [rax + 0x58]	; RDX
	mov		[rsp + 0x58], rdi	;

	mov		rdi, [rax + 0x60]	; RCX
	mov		[rsp + 0x60], rdi	;

	mov		rdi, [rax + 0x68]	; RBX
	mov		[rsp + 0x68], rdi	;

	mov		rdi, [rax + 0x70]	; RAX
	mov		[rsp + 0x70], rdi	;

	mov		di, [rax + 0x78]	; SS
	mov		[rsp + 0xB8], di	;

	mov		di, [rax + 0x7A]	; CS
	mov		[rsp + 0xA0], di	;
	
	mov		di, [rax + 0x7C]	; DS
	mov		[rsp + 0x7C], di	;

	mov		di, [rax + 0x7E]	; ES
	mov		[rsp + 0x7E], di	;

	mov		di, [rax + 0x80]	; FS
	mov		[rsp + 0x80], di	;

	mov		di, [rax + 0x82]	; GS
	mov		[rsp + 0x82], di	;

	mov		di, 0x1111			;
	mov		[rsp + 0x84], di	;

	; 4 Bytes of padding

	mov		rdi, [rax + 0x88]	; RSI
	mov		[rsp + 0x88], rdi	;

	mov		rdi, [rax + 0x90]	; RDI
	mov		[rsp + 0x90], rdi	;

	mov		rdi, [rax + 0x98]	; RIP
	mov		[rsp + 0x98], rdi	;

	mov		rdi, [rax + 0xA0]	; CR3
	mov		cr3, rdi	;

	mov		rdi, [rax + 0xA8]	; RFLAGS
	mov		[rsp + 0xA8], rdi	;

	; MXCSR


	.restore:
	; Go over the Process ID.
	add		rsp, 8				;
	pop		r15					;
	pop		r14					;
	pop		r13					;
	pop		r12					;
	pop		r11					;
	pop		r10					;
	pop		r9						;
	pop		r8						;
	pop		rax					; Don't overwrite RSP. Trash RAX instead.
	pop		rbp					;
	pop		rdx					;
	pop		rcx					;
	pop		rbx					;
	pop		rax					;

	mov		ds, [rsp + 4]		; DS
	mov		es, [rsp + 6]		; ES
	mov		fs, [rsp + 8]		; FS
	mov		gs, [rsp + 10]		; GS
	add		rsp, 16				;

	pop		rsi					;
	pop		rdi					;
	iretq								;
	
INTERRUPT_ENTRY_000:				;
	push		rdi					;
	mov		rdi, 000				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_001:				; For INT without error
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;
	
INTERRUPT_ENTRY_002:
	push		rdi					;
	mov		rdi, 002				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_003:
	push		rdi					;
	mov		rdi, 003				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_004:
	push		rdi					;
	mov		rdi, 004				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_005:
	push		rdi					;
	mov		rdi, 005				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_006:
	push		rdi					;
	mov		rdi, 006				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_007:
	push		rdi					;
	mov		rdi, 007				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

;Generates an Erorr
INTERRUPT_ENTRY_008:
	push		rdi					; Save old RDI
	mov		rdi, [rsp + 8]		; Put error code into rdi
	push		rdi					; Push error code
	
	mov		rdi, [rsp + 8]		; Move the old RDI down the stack.
	mov		[rsp + 16], rdi	;
	
	pop		rdi					; Pop the error code into rdi.
	mov		[rsp], rsi			; Save old RSI
	mov		rsi, rdi				; Move error code into rsi.
	
	mov		rdi, 008				; Put the INT number in RDI
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_009:
	push		rdi					;
	mov		rdi, 009				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

; Generates an Error
INTERRUPT_ENTRY_010:
	push		rdi					; Save old RDI
	mov		rdi, [rsp + 8]		; Put error code into rdi
	push		rdi					; Push error code
	
	mov		rdi, [rsp + 8]		; Move the old RDI down the stack.
	mov		[rsp + 16], rdi	;
	
	pop		rdi					; Pop the error code into rdi.
	mov		[rsp], rsi			; Save old RSI
	mov		rsi, rdi				; Move error code into rsi.
	
	mov		rdi, 010				; Put the INT number in RDI
	cld								;
	jmp		common_handler		;

; Generates an Error
INTERRUPT_ENTRY_011:
	push		rdi					; Save old RDI
	mov		rdi, [rsp + 8]		; Put error code into rdi
	push		rdi					; Push error code
	
	mov		rdi, [rsp + 8]		; Move the old RDI down the stack.
	mov		[rsp + 16], rdi	;
	
	pop		rdi					; Pop the error code into rdi.
	mov		[rsp], rsi			; Save old RSI
	mov		rsi, rdi				; Move error code into rsi.
	
	mov		rdi, 011				; Put the INT number in RDI
	cld								;
	jmp		common_handler		;

;Generates an Error
INTERRUPT_ENTRY_012:
		push		rdi					; Save old RDI
	mov		rdi, [rsp + 8]		; Put error code into rdi
	push		rdi					; Push error code
	
	mov		rdi, [rsp + 8]		; Move the old RDI down the stack.
	mov		[rsp + 16], rdi	;
	
	pop		rdi					; Pop the error code into rdi.
	mov		[rsp], rsi			; Save old RSI
	mov		rsi, rdi				; Move error code into rsi.
	
	mov		rdi, 012				; Put the INT number in RDI
	cld								;
	jmp		common_handler		;

;Generates an Error
INTERRUPT_ENTRY_013:
	push		rdi					; Save old RDI
	mov		rdi, [rsp + 8]		; Put error code into rdi
	push		rdi					; Push error code
	
	mov		rdi, [rsp + 8]		; Move the old RDI down the stack.
	mov		[rsp + 16], rdi	;
	
	pop		rdi					; Pop the error code into rdi.
	mov		[rsp], rsi			; Save old RSI
	mov		rsi, rdi				; Move error code into rsi.
	
	mov		rdi, 013				; Put the INT number in RDI
	cld								;
	jmp		common_handler		;

;Generates an Error
INTERRUPT_ENTRY_014:
	push		rdi					; Save old RDI
	mov		rdi, [rsp + 8]		; Put error code into rdi
	push		rdi					; Push error code
	
	mov		rdi, [rsp + 8]		; Move the old RDI down the stack.
	mov		[rsp + 16], rdi	;
	
	pop		rdi					; Pop the error code into rdi.
	mov		[rsp], rsi			; Save old RSI
	mov		rsi, rdi				; Move error code into rsi.
	

	mov		rdi, 014				; Put the INT number in RDI
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_015:
	push		rdi					;
	mov		rdi, 015				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_016:
	push		rdi					;
	mov		rdi, 016				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

;Generates an Error
INTERRUPT_ENTRY_017:
	push		rdi					; Save old RDI
	mov		rdi, [rsp + 8]		; Put error code into rdi
	push		rdi					; Push error code
	
	mov		rdi, [rsp + 8]		; Move the old RDI down the stack.
	mov		[rsp + 16], rdi	;
	
	pop		rdi					; Pop the error code into rdi.
	mov		[rsp], rsi			; Save old RSI
	mov		rsi, rdi				; Move error code into rsi.
	
	mov		rdi, 017				; Put the INT number in RDI
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_018:
	push		rdi					;
	mov		rdi, 018				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_019:
	push		rdi					;
	mov		rdi, 019				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_020:
	push		rdi					;
	mov		rdi, 020				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_021:
	push		rdi					;
	mov		rdi, 022				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_022:
	push		rdi					;
	mov		rdi, 0022			;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_023:
	push		rdi					;
	mov		rdi, 023				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_024:
	push		rdi					;
	mov		rdi, 024				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_025:
	push		rdi					;
	mov		rdi, 025				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_026:
	push		rdi					;
	mov		rdi, 026				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_027:
	push		rdi					;
	mov		rdi, 027				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_028:
	push		rdi					;
	mov		rdi, 028				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_029:
	push		rdi					;
	mov		rdi, 029				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_030:
	push		rdi					;
	mov		rdi, 030				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_031:
	push		rdi					;
	mov		rdi, 031				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_032:
	push		rdi					;
	mov		rdi, 032				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_033:
	push		rdi					;
	mov		rdi, 033				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_034:
	push		rdi					;
	mov		rdi, 034				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_035:
	push		rdi					;
	mov		rdi, 035				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_036:
	push		rdi					;
	mov		rdi, 036				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_037:
	push		rdi					;
	mov		rdi, 037				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_038:
	push		rdi					;
	mov		rdi, 038				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_039:
	push		rdi					;
	mov		rdi, 039				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_040:
	push		rdi					;
	mov		rdi, 040				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_041:
	push		rdi					;
	mov		rdi, 041				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_042:
	push		rdi					;
	mov		rdi, 042				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_043:
	push		rdi					;
	mov		rdi, 043				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_044:
	push		rdi					;
	mov		rdi, 044				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_045:
	push		rdi					;
	mov		rdi, 045				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_046:
	push		rdi					;
	mov		rdi, 046				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_047:
	push		rdi					;
	mov		rdi, 047				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_048:
	push		rdi					;
	mov		rdi, 048				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_049:
	push		rdi					;
	mov		rdi, 049				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_050:
	push		rdi					;
	mov		rdi, 050				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_051:
	push		rdi					;
	mov		rdi, 051				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_052:
	push		rdi					;
	mov		rdi, 052				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_053:
	push		rdi					;
	mov		rdi, 053				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_054:
	push		rdi					;
	mov		rdi, 054				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_055:
	push		rdi					;
	mov		rdi, 055				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_056:
	push		rdi					;
	mov		rdi, 056				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_057:
	push		rdi					;
	mov		rdi, 057				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_058:
	push		rdi					;
	mov		rdi, 058				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_059:
	push		rdi					;
	mov		rdi, 059				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_060:
	push		rdi					;
	mov		rdi, 060				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_061:
	push		rdi					;
	mov		rdi, 061				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_062:
	push		rdi					;
	mov		rdi, 062				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_063:
	push		rdi					;
	mov		rdi, 063				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_064:
	push		rdi					;
	mov		rdi, 064				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_065:
	push		rdi					;
	mov		rdi, 065				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_066:
	push		rdi					;
	mov		rdi, 066				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_067:
	push		rdi					;
	mov		rdi, 067				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_068:
	push		rdi					;
	mov		rdi, 068				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_069:
	push		rdi					;
	mov		rdi, 069				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_070:
	push		rdi					;
	mov		rdi, 070				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_071:
	push		rdi					;
	mov		rdi, 071				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_072:
	push		rdi					;
	mov		rdi, 072				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_073:
	push		rdi					;
	mov		rdi, 073				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_074:
	push		rdi					;
	mov		rdi, 074				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_075:
	push		rdi					;
	mov		rdi, 075				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_076:
	push		rdi					;
	mov		rdi, 076				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_077:
	push		rdi					;
	mov		rdi, 077				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_078:
	push		rdi					;
	mov		rdi, 078				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_079:
	push		rdi					;
	mov		rdi, 079				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_080:
	push		rdi					;
	mov		rdi, 080				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_081:
	push		rdi					;
	mov		rdi, 081				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_082:
	push		rdi					;
	mov		rdi, 082				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_083:
	push		rdi					;
	mov		rdi, 083				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_084:
	push		rdi					;
	mov		rdi, 084				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_085:
	push		rdi					;
	mov		rdi, 085				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_086:
	push		rdi					;
	mov		rdi, 086				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_087:
	push		rdi					;
	mov		rdi, 087				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_088:
	push		rdi					;
	mov		rdi, 088				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_089:
	push		rdi					;
	mov		rdi, 089				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_090:
	push		rdi					;
	mov		rdi, 090				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_091:
	push		rdi					;
	mov		rdi, 091				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_092:
	push		rdi					;
	mov		rdi, 092				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_093:
	push		rdi					;
	mov		rdi, 093				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_094:
	push		rdi					;
	mov		rdi, 094				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_095:
	push		rdi					;
	mov		rdi, 095				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_096:
	push		rdi					;
	mov		rdi, 096				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_097:
	push		rdi					;
	mov		rdi, 097				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_098:
	push		rdi					;
	mov		rdi, 098				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_099:
	push		rdi					;
	mov		rdi, 099				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_100:
	push		rdi					;
	mov		rdi, 100				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_101:
	push		rdi					;
	mov		rdi, 101				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_102:
	push		rdi					;
	mov		rdi, 102				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_103:
	push		rdi					;
	mov		rdi, 103				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_104:
	push		rdi					;
	mov		rdi, 104				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_105:
	push		rdi					;
	mov		rdi, 105				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_106:
	push		rdi					;
	mov		rdi, 106				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_107:
	push		rdi					;
	mov		rdi, 107				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_108:
	push		rdi					;
	mov		rdi, 108				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_109:
	push		rdi					;
	mov		rdi, 109				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;


INTERRUPT_ENTRY_110:	
	push		rdi					;
	mov		rdi, 110				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_111:
	push		rdi					;
	mov		rdi, 111				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_112:
	push		rdi					;
	mov		rdi, 112				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_113:
	push		rdi					;
	mov		rdi, 113				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_114:
	push		rdi					;
	mov		rdi, 114				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_115:
	push		rdi					;
	mov		rdi, 115				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_116:
	push		rdi					;
	mov		rdi, 116				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_117:
	push		rdi					;
	mov		rdi, 117				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_118:
	push		rdi					;
	mov		rdi, 118				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_119:
	push		rdi					;
	mov		rdi, 119				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_120:
	push		rdi					;
	mov		rdi, 120				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_121:
	push		rdi					;
	mov		rdi, 121				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_122:
	push		rdi					;
	mov		rdi, 122				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_123:
	push		rdi					;
	mov		rdi, 123				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_124:
	push		rdi					;
	mov		rdi, 124				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_125:
	push		rdi					;
	mov		rdi, 125				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_126:
	push		rdi					;
	mov		rdi, 126				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_127:
	push		rdi					;
	mov		rdi, 127				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_128:
	push		rdi					;
	mov		rdi, 128				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_129:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_130:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_131:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_132:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_133:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_134:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_135:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_136:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_137:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_138:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_139:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_140:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_141:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_142:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_143:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_144:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_145:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_146:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_147:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_148:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_149:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_150:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_151:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_152:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_153:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_154:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_155:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_156:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_157:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_158:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_159:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_160:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_161:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_162:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_163:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_164:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_165:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_166:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_167:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_168:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_169:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_170:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_171:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_172:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_173:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_174:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_175:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_176:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_177:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_178:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_179:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_180:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_181:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_182:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_183:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_184:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_185:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_186:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_187:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_188:
	push		rdi					;
	mov		rdi, 188				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_189:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_190:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_191:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_192:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_193:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_194:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_195:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_196:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_197:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_198:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_199:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_200:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_201:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_202:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_203:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_204:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_205:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_206:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_207:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_208:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_209:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_210:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_211:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_212:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_213:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_214:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_215:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_216:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_217:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_218:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_219:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_220:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_221:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_222:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_223:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_224:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_225:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_226:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_227:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_228:
	push		rdi					;
	mov		rdi, 001				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_229:
	push		rdi					;
	mov		rdi, 229				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_230:
	push		rdi					;
	mov		rdi, 230				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_231:
	push		rdi					;
	mov		rdi, 231				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_232:
	push		rdi					;
	mov		rdi, 232				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_233:
	push		rdi					;
	mov		rdi, 233				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_234:
	push		rdi					;
	mov		rdi, 234				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_235:
	push		rdi					;
	mov		rdi, 235				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_236:
	push		rdi					;
	mov		rdi, 236				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_237:
	push		rdi					;
	mov		rdi, 237				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_238:
	push		rdi					;
	mov		rdi, 238				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_239:
	push		rdi					;
	mov		rdi, 239				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_240:
	push		rdi					;
	mov		rdi, 240				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_241:
	push		rdi					;
	mov		rdi, 241				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_242:
	push		rdi					;
	mov		rdi, 242				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_243:
	push		rdi					;
	mov		rdi, 243				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_244:
	push		rdi					;
	mov		rdi, 244				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_245:
	push		rdi					;
	mov		rdi, 245				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_246:
	push		rdi					;
	mov		rdi, 246				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_247:
	push		rdi					;
	mov		rdi, 247				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_248:
	push		rdi					;
	mov		rdi, 248				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_249:
	push		rdi					;
	mov		rdi, 249				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_250:
	push		rdi					;
	mov		rdi, 250				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_251:
	push		rdi					;
	mov		rdi, 251				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_252:
	push		rdi					;
	mov		rdi, 252				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_253:
	push		rdi					;
	mov		rdi, 253				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_254:
	push		rdi					;
	mov		rdi, 254				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;

INTERRUPT_ENTRY_255:
	push		rdi					;
	mov		rdi, 255				;
	push		rsi					;
	mov		rsi, 0				;
	cld								;
	jmp		common_handler		;


global FillInterruptArray

FillInterruptArray:
	push		rax					;
	mov		rax, qword INTERRUPT_ENTRY_000;
	mov		qword [INTERRUPT_ARRAY + 0], rax;

	mov		rax, qword INTERRUPT_ENTRY_001;
	mov		qword [INTERRUPT_ARRAY + 8], rax;

	mov		rax, qword INTERRUPT_ENTRY_002;
	mov		qword [INTERRUPT_ARRAY + 16], rax;

	mov		rax, qword INTERRUPT_ENTRY_003;
	mov		qword [INTERRUPT_ARRAY + 24], rax;

	mov		rax, qword INTERRUPT_ENTRY_004;
	mov		qword [INTERRUPT_ARRAY + 32], rax;

	mov		rax, qword INTERRUPT_ENTRY_005;
	mov		qword [INTERRUPT_ARRAY + 40], rax;

	mov		rax, qword INTERRUPT_ENTRY_006;
	mov		qword [INTERRUPT_ARRAY + 48], rax;

	mov		rax, qword INTERRUPT_ENTRY_007;
	mov		qword [INTERRUPT_ARRAY + 56], rax;

	mov		rax, qword INTERRUPT_ENTRY_008;
	mov		qword [INTERRUPT_ARRAY + 64], rax;

	mov		rax, qword INTERRUPT_ENTRY_009;
	mov		qword [INTERRUPT_ARRAY + 72], rax;

	mov		rax, qword INTERRUPT_ENTRY_010;
	mov		qword [INTERRUPT_ARRAY + 80], rax;

	mov		rax, qword INTERRUPT_ENTRY_011;
	mov		qword [INTERRUPT_ARRAY + 88], rax;

	mov		rax, qword INTERRUPT_ENTRY_012;
	mov		qword [INTERRUPT_ARRAY + 96], rax;

	mov		rax, qword INTERRUPT_ENTRY_013;
	mov		qword [INTERRUPT_ARRAY + 104], rax;

	mov		rax, qword INTERRUPT_ENTRY_014;
	mov		qword [INTERRUPT_ARRAY + 112], rax;

	mov		rax, qword INTERRUPT_ENTRY_015;
	mov		qword [INTERRUPT_ARRAY + 120], rax;

	mov		rax, qword INTERRUPT_ENTRY_016;
	mov		qword [INTERRUPT_ARRAY + 128], rax;

	mov		rax, qword INTERRUPT_ENTRY_017;
	mov		qword [INTERRUPT_ARRAY + 136], rax;

	mov		rax, qword INTERRUPT_ENTRY_018;
	mov		qword [INTERRUPT_ARRAY + 144], rax;

	mov		rax, qword INTERRUPT_ENTRY_019;
	mov		qword [INTERRUPT_ARRAY + 152], rax;

	mov		rax, qword INTERRUPT_ENTRY_020;
	mov		qword [INTERRUPT_ARRAY + 160], rax;

	mov		rax, qword INTERRUPT_ENTRY_021;
	mov		qword [INTERRUPT_ARRAY + 168], rax;

	mov		rax, qword INTERRUPT_ENTRY_022;
	mov		qword [INTERRUPT_ARRAY + 176], rax;

	mov		rax, qword INTERRUPT_ENTRY_023;
	mov		qword [INTERRUPT_ARRAY + 184], rax;

	mov		rax, qword INTERRUPT_ENTRY_024;
	mov		qword [INTERRUPT_ARRAY + 192], rax;

	mov		rax, qword INTERRUPT_ENTRY_025;
	mov		qword [INTERRUPT_ARRAY + 200], rax;

	mov		rax, qword INTERRUPT_ENTRY_026;
	mov		qword [INTERRUPT_ARRAY + 208], rax;

	mov		rax, qword INTERRUPT_ENTRY_027;
	mov		qword [INTERRUPT_ARRAY + 216], rax;

	mov		rax, qword INTERRUPT_ENTRY_028;
	mov		qword [INTERRUPT_ARRAY + 224], rax;

	mov		rax, qword INTERRUPT_ENTRY_029;
	mov		qword [INTERRUPT_ARRAY + 232], rax;

	mov		rax, qword INTERRUPT_ENTRY_030;
	mov		qword [INTERRUPT_ARRAY + 240], rax;

	mov		rax, qword INTERRUPT_ENTRY_031;
	mov		qword [INTERRUPT_ARRAY + 248], rax;

	mov		rax, qword INTERRUPT_ENTRY_032;
	mov		qword [INTERRUPT_ARRAY + 256], rax;

	mov		rax, qword INTERRUPT_ENTRY_033;
	mov		qword [INTERRUPT_ARRAY + 264], rax;

	mov		rax, qword INTERRUPT_ENTRY_034;
	mov		qword [INTERRUPT_ARRAY + 272], rax;

	mov		rax, qword INTERRUPT_ENTRY_035;
	mov		qword [INTERRUPT_ARRAY + 280], rax;

	mov		rax, qword INTERRUPT_ENTRY_036;
	mov		qword [INTERRUPT_ARRAY + 288], rax;

	mov		rax, qword INTERRUPT_ENTRY_037;
	mov		qword [INTERRUPT_ARRAY + 296], rax;

	mov		rax, qword INTERRUPT_ENTRY_038;
	mov		qword [INTERRUPT_ARRAY + 304], rax;

	mov		rax, qword INTERRUPT_ENTRY_039;
	mov		qword [INTERRUPT_ARRAY + 312], rax;

	mov		rax, qword INTERRUPT_ENTRY_040;
	mov		qword [INTERRUPT_ARRAY + 320], rax;

	mov		rax, qword INTERRUPT_ENTRY_041;
	mov		qword [INTERRUPT_ARRAY + 0x148], rax;

	mov		rax, qword INTERRUPT_ENTRY_042;
	mov		qword [INTERRUPT_ARRAY + 0x150], rax;

	mov		rax, qword INTERRUPT_ENTRY_043;
	mov		qword [INTERRUPT_ARRAY + 0x158], rax;

	mov		rax, qword INTERRUPT_ENTRY_044;
	mov		qword [INTERRUPT_ARRAY + 0x160], rax;

	mov		rax, qword INTERRUPT_ENTRY_045;
	mov		qword [INTERRUPT_ARRAY + 0x168], rax;

	mov		rax, qword INTERRUPT_ENTRY_046;
	mov		qword [INTERRUPT_ARRAY + 0x170], rax;

	mov		rax, qword INTERRUPT_ENTRY_047;
	mov		qword [INTERRUPT_ARRAY + 0x178], rax;

	mov		rax, qword INTERRUPT_ENTRY_048;
	mov		qword [INTERRUPT_ARRAY + 0x180], rax;

	mov		rax, qword INTERRUPT_ENTRY_049;
	mov		qword [INTERRUPT_ARRAY + 0x188], rax;

	mov		rax, qword INTERRUPT_ENTRY_050;
	mov		qword [INTERRUPT_ARRAY + 0x190], rax;

	mov		rax, qword INTERRUPT_ENTRY_051;
	mov		qword [INTERRUPT_ARRAY + 0x198], rax;

	mov		rax, qword INTERRUPT_ENTRY_052;
	mov		qword [INTERRUPT_ARRAY + 0x1A0], rax;

	mov		rax, qword INTERRUPT_ENTRY_053;
	mov		qword [INTERRUPT_ARRAY + 0x1A8], rax;

	mov		rax, qword INTERRUPT_ENTRY_054;
	mov		qword [INTERRUPT_ARRAY + 0x1B0], rax;

	mov		rax, qword INTERRUPT_ENTRY_055;
	mov		qword [INTERRUPT_ARRAY + 0x1B8], rax;

	mov		rax, qword INTERRUPT_ENTRY_056;
	mov		qword [INTERRUPT_ARRAY + 0x1C0], rax;

	mov		rax, qword INTERRUPT_ENTRY_057;
	mov		qword [INTERRUPT_ARRAY + 0x1C8], rax;

	mov		rax, qword INTERRUPT_ENTRY_058;
	mov		qword [INTERRUPT_ARRAY + 0x1D0], rax;

	mov		rax, qword INTERRUPT_ENTRY_059;
	mov		qword [INTERRUPT_ARRAY + 0x1D8], rax;

	mov		rax, qword INTERRUPT_ENTRY_060;
	mov		qword [INTERRUPT_ARRAY + 0x1E0], rax;

	mov		rax, qword INTERRUPT_ENTRY_061;
	mov		qword [INTERRUPT_ARRAY + 0x1E8], rax;

	mov		rax, qword INTERRUPT_ENTRY_062;
	mov		qword [INTERRUPT_ARRAY + 0x1F0], rax;

	mov		rax, qword INTERRUPT_ENTRY_063;
	mov		qword [INTERRUPT_ARRAY + 0x1F8], rax;

	mov		rax, qword INTERRUPT_ENTRY_064;
	mov		qword [INTERRUPT_ARRAY + 0x200], rax;

	mov		rax, qword INTERRUPT_ENTRY_065;
	mov		qword [INTERRUPT_ARRAY + 0x208], rax;

	mov		rax, qword INTERRUPT_ENTRY_066;
	mov		qword [INTERRUPT_ARRAY + 0x210], rax;

	mov		rax, qword INTERRUPT_ENTRY_067;
	mov		qword [INTERRUPT_ARRAY + 0x218], rax;

	mov		rax, qword INTERRUPT_ENTRY_068;
	mov		qword [INTERRUPT_ARRAY + 0x220], rax;

	mov		rax, qword INTERRUPT_ENTRY_069;
	mov		qword [INTERRUPT_ARRAY + 0x228], rax;

	mov		rax, qword INTERRUPT_ENTRY_070;
	mov		qword [INTERRUPT_ARRAY + 0x230], rax;

	mov		rax, qword INTERRUPT_ENTRY_071;
	mov		qword [INTERRUPT_ARRAY + 0x238], rax;

	mov		rax, qword INTERRUPT_ENTRY_072;
	mov		qword [INTERRUPT_ARRAY + 0x240], rax;

	mov		rax, qword INTERRUPT_ENTRY_073;
	mov		qword [INTERRUPT_ARRAY + 0x248], rax;

	mov		rax, qword INTERRUPT_ENTRY_074;
	mov		qword [INTERRUPT_ARRAY + 0x250], rax;

	mov		rax, qword INTERRUPT_ENTRY_075;
	mov		qword [INTERRUPT_ARRAY + 0x258], rax;

	mov		rax, qword INTERRUPT_ENTRY_076;
	mov		qword [INTERRUPT_ARRAY + 0x260], rax;

	mov		rax, qword INTERRUPT_ENTRY_077;
	mov		qword [INTERRUPT_ARRAY + 0x268], rax;

	mov		rax, qword INTERRUPT_ENTRY_078;
	mov		qword [INTERRUPT_ARRAY + 0x270], rax;

	mov		rax, qword INTERRUPT_ENTRY_079;
	mov		qword [INTERRUPT_ARRAY + 0x278], rax;

	mov		rax, qword INTERRUPT_ENTRY_080;
	mov		qword [INTERRUPT_ARRAY + 0x280], rax;

	mov		rax, qword INTERRUPT_ENTRY_081;
	mov		qword [INTERRUPT_ARRAY + 0x288], rax;

	mov		rax, qword INTERRUPT_ENTRY_082;
	mov		qword [INTERRUPT_ARRAY + 0x290], rax;

	mov		rax, qword INTERRUPT_ENTRY_083;
	mov		qword [INTERRUPT_ARRAY + 0x298], rax;

	mov		rax, qword INTERRUPT_ENTRY_084;
	mov		qword [INTERRUPT_ARRAY + 0x2A0], rax;

	mov		rax, qword INTERRUPT_ENTRY_085;
	mov		qword [INTERRUPT_ARRAY + 0x2A8], rax;

	mov		rax, qword INTERRUPT_ENTRY_086;
	mov		qword [INTERRUPT_ARRAY + 0x2B0], rax;

	mov		rax, qword INTERRUPT_ENTRY_087;
	mov		qword [INTERRUPT_ARRAY + 0x2B8], rax;

	mov		rax, qword INTERRUPT_ENTRY_088;
	mov		qword [INTERRUPT_ARRAY + 0x2C0], rax;

	mov		rax, qword INTERRUPT_ENTRY_089;
	mov		qword [INTERRUPT_ARRAY + 0x2C8], rax;

	mov		rax, qword INTERRUPT_ENTRY_090;
	mov		qword [INTERRUPT_ARRAY + 0x2D0], rax;

	mov		rax, qword INTERRUPT_ENTRY_091;
	mov		qword [INTERRUPT_ARRAY + 0x2D8], rax;

	mov		rax, qword INTERRUPT_ENTRY_092;
	mov		qword [INTERRUPT_ARRAY + 0x2E0], rax;

	mov		rax, qword INTERRUPT_ENTRY_093;
	mov		qword [INTERRUPT_ARRAY + 0x2E8], rax;

	mov		rax, qword INTERRUPT_ENTRY_094;
	mov		qword [INTERRUPT_ARRAY + 0x2F0], rax;

	mov		rax, qword INTERRUPT_ENTRY_095;
	mov		qword [INTERRUPT_ARRAY + 0x2F8], rax;

	mov		rax, qword INTERRUPT_ENTRY_096;
	mov		qword [INTERRUPT_ARRAY + 0x300], rax;

	mov		rax, qword INTERRUPT_ENTRY_097;
	mov		qword [INTERRUPT_ARRAY + 0x308], rax;

	mov		rax, qword INTERRUPT_ENTRY_098;
	mov		qword [INTERRUPT_ARRAY + 0x310], rax;

	mov		rax, qword INTERRUPT_ENTRY_099;
	mov		qword [INTERRUPT_ARRAY + 0x318], rax;

	mov		rax, qword INTERRUPT_ENTRY_100;
	mov		qword [INTERRUPT_ARRAY + 0x320], rax;

	mov		rax, qword INTERRUPT_ENTRY_101;
	mov		qword [INTERRUPT_ARRAY + 0x328], rax;

	mov		rax, qword INTERRUPT_ENTRY_102;
	mov		qword [INTERRUPT_ARRAY + 0x330], rax;

	mov		rax, qword INTERRUPT_ENTRY_103;
	mov		qword [INTERRUPT_ARRAY + 0x338], rax;

	mov		rax, qword INTERRUPT_ENTRY_104;
	mov		qword [INTERRUPT_ARRAY + 0x340], rax;

	mov		rax, qword INTERRUPT_ENTRY_105;
	mov		qword [INTERRUPT_ARRAY + 0x348], rax;

	mov		rax, qword INTERRUPT_ENTRY_106;
	mov		qword [INTERRUPT_ARRAY + 0x350], rax;

	mov		rax, qword INTERRUPT_ENTRY_107;
	mov		qword [INTERRUPT_ARRAY + 0x358], rax;

	mov		rax, qword INTERRUPT_ENTRY_108;
	mov		qword [INTERRUPT_ARRAY + 0x360], rax;

	mov		rax, qword INTERRUPT_ENTRY_109;
	mov		qword [INTERRUPT_ARRAY + 0x368], rax;

	mov		rax, qword INTERRUPT_ENTRY_110;
	mov		qword [INTERRUPT_ARRAY + 0x370], rax;

	mov		rax, qword INTERRUPT_ENTRY_111;
	mov		qword [INTERRUPT_ARRAY + 0x378], rax;

	mov		rax, qword INTERRUPT_ENTRY_112;
	mov		qword [INTERRUPT_ARRAY + 0x380], rax;

	mov		rax, qword INTERRUPT_ENTRY_113;
	mov		qword [INTERRUPT_ARRAY + 0x388], rax;

	mov		rax, qword INTERRUPT_ENTRY_114;
	mov		qword [INTERRUPT_ARRAY + 0x390], rax;

	mov		rax, qword INTERRUPT_ENTRY_115;
	mov		qword [INTERRUPT_ARRAY + 0x398], rax;

	mov		rax, qword INTERRUPT_ENTRY_116;
	mov		qword [INTERRUPT_ARRAY + 0x3A0], rax;

	mov		rax, qword INTERRUPT_ENTRY_117;
	mov		qword [INTERRUPT_ARRAY + 0x3A8], rax;

	mov		rax, qword INTERRUPT_ENTRY_118;
	mov		qword [INTERRUPT_ARRAY + 0x3B0], rax;

	mov		rax, qword INTERRUPT_ENTRY_119;
	mov		qword [INTERRUPT_ARRAY + 0x3B8], rax;

	mov		rax, qword INTERRUPT_ENTRY_120;
	mov		qword [INTERRUPT_ARRAY + 0x3C0], rax;

	mov		rax, qword INTERRUPT_ENTRY_121;
	mov		qword [INTERRUPT_ARRAY + 0x3C8], rax;

	mov		rax, qword INTERRUPT_ENTRY_122;
	mov		qword [INTERRUPT_ARRAY + 0x3D0], rax;

	mov		rax, qword INTERRUPT_ENTRY_123;
	mov		qword [INTERRUPT_ARRAY + 0x3D8], rax;

	mov		rax, qword INTERRUPT_ENTRY_124;
	mov		qword [INTERRUPT_ARRAY + 0x3E0], rax;

	mov		rax, qword INTERRUPT_ENTRY_125;
	mov		qword [INTERRUPT_ARRAY + 0x3E8], rax;

	mov		rax, qword INTERRUPT_ENTRY_126;
	mov		qword [INTERRUPT_ARRAY + 0x3F0], rax;

	mov		rax, qword INTERRUPT_ENTRY_127;
	mov		qword [INTERRUPT_ARRAY + 0x3F8], rax;

	mov		rax, qword INTERRUPT_ENTRY_128;
	mov		qword [INTERRUPT_ARRAY + 0x400], rax;

	mov		rax, qword INTERRUPT_ENTRY_129;
	mov		qword [INTERRUPT_ARRAY + 0x408], rax;

	mov		rax, qword INTERRUPT_ENTRY_130;
	mov		qword [INTERRUPT_ARRAY + 0x410], rax;

	mov		rax, qword INTERRUPT_ENTRY_131;
	mov		qword [INTERRUPT_ARRAY + 0x418], rax;

	mov		rax, qword INTERRUPT_ENTRY_132;
	mov		qword [INTERRUPT_ARRAY + 0x420], rax;

	mov		rax, qword INTERRUPT_ENTRY_133;
	mov		qword [INTERRUPT_ARRAY + 0x428], rax;

	mov		rax, qword INTERRUPT_ENTRY_134;
	mov		qword [INTERRUPT_ARRAY + 0x430], rax;

	mov		rax, qword INTERRUPT_ENTRY_135;
	mov		qword [INTERRUPT_ARRAY + 0x438], rax;

	mov		rax, qword INTERRUPT_ENTRY_136;
	mov		qword [INTERRUPT_ARRAY + 0x440], rax;

	mov		rax, qword INTERRUPT_ENTRY_137;
	mov		qword [INTERRUPT_ARRAY + 0x448], rax;

	mov		rax, qword INTERRUPT_ENTRY_138;
	mov		qword [INTERRUPT_ARRAY + 0x450], rax;

	mov		rax, qword INTERRUPT_ENTRY_139;
	mov		qword [INTERRUPT_ARRAY + 0x458], rax;

	mov		rax, qword INTERRUPT_ENTRY_140;
	mov		qword [INTERRUPT_ARRAY + 0x460], rax;

	mov		rax, qword INTERRUPT_ENTRY_141;
	mov		qword [INTERRUPT_ARRAY + 0x468], rax;

	mov		rax, qword INTERRUPT_ENTRY_142;
	mov		qword [INTERRUPT_ARRAY + 0x470], rax;


	
	pop		rax					;
	ret