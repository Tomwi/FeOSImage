.global ARGB24_to_ARGB16
.hidden ARGB24_to_ARGB16
.type ARGB24_to_ARGB16, %function

.arm
.align 2

@ r0 in
@ r1 dest
@ r2 = len

ARGB24_to_ARGB16:

cmp r2, #0
bxeq lr

.fLoop:

bx lr