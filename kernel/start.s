.global		_start

.code32
.section 	.text
_start:
	mov		$0x0f61,%ax
	mov		%ax,%gs:480

loop:	jmp		loop
