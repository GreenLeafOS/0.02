.code32
.section 	.text
.global		_start
.global		loop

.extern		arch_init
.extern		page_init
.extern		memory_init

_start:
	mov		$0x0f00+'K',%ax
	mov		%ax,%gs:640

	call	memory_init
	mov		$0x0f00+'M',%ax
	mov		%ax,%gs:642

	call	arch_init
	mov		$0x0f00+'A',%ax
	mov		%ax,%gs:644

	call	page_init
	mov		$0x0f00+'P',%ax
	mov		%ax,%gs:646

	hlt
loop:	jmp		loop


