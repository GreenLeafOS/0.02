.global		_start

.extern		gdt_init
.extern		idt_init
.extern		tss_init
.extern		init_8259A
.extern		buddy_init

.extern		kernel_main

.section 	.text
_start:
	call	set_bss

	call	gdt_init

	mov		$0x0f61,%ax
	mov		%ax,%gs:480

	mov		$16,%ax
	mov		%ax,%ds
	mov		%ax,%es
	mov		%ax,%fs
	mov		%ax,%ss

	mov		$40,%ax
	mov		%ax,%gs

	movl	$0x80000,%esp

	call	init_8259A

	call	idt_init
	call	tss_init

	call	buddy_init
	call	page_init

	mov		$0x0f62,%ax
	mov		%ax,%gs:640

	jmp		kernel_main

set_bss:
	movl	$_bss,%eax
	movl	$_end,%ecx
	subl	%eax,%ecx
_set_bss:
	movl	$0,%ds:(%eax,%ecx)
	subl	$4,%ecx
	jnz		_set_bss

	ret



