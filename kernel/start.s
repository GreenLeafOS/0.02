.code32
.section 	.text
.global		_start

_start:
	mov		$0x0f00+'k',%ax
	mov		%ax,%gs:640

loop:	jmp		loop
