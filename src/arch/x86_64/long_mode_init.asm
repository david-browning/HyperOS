global long_mode_start

section .text
bits 64
long_mode_start:
	; load 0 into all data segment registers
	mov ax, 0
	mov ss, ax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	; Put the address to the multiboot info in arg 0.
	mov edi, ebx;

	extern kernel_main
	call kernel_main
	
	;Write shutdown to screen.
	mov r8, 0x2f742f752f682f53
	mov r9, 0x2f6E2f772f6F2f64
	mov qword [0xb8000], r8
	mov qword [0xb8008], r9

	;Write the kernel return code to the screen.
	extern PrintResult
	mov rdi, rax;
	call PrintResult;

	hlt