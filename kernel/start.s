.code32
.section 	.text
.global		_start

.extern		arch_init
.extern		page_init


_start:
	call	mem_set

	mov		$0x0f00+'K',%ax
	mov		%ax,%gs:640

	call	arch_init
	call	page_init

	mov		$0x0f00+'P',%ax
	mov		%ax,%gs:642

loop:	jmp		loop

mem_set:
	movl	$_bss,%eax
	movl	$_end,%ecx
	subl	%eax,%ecx
_mem_set:
	movl	$0,%es:(%eax,%ecx)
	subl	$4,%ecx
	jnz		_mem_set

	ret
