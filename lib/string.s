/*
 * string.s
 *
 *  Created on: 2012-8-16
 *      Author: greenleaf
 */
.global memset

.section	.text
memset:
	push %ebp
	mov %esp,%ebp
	mov 8(%ebp),%edi
    mov 12(%ebp),%eax
    mov 16(%ebp),%ecx
    rep
    stosb
    mov 8(%ebp),%eax
    mov %ebp,%esp
    pop %ebp
    ret
