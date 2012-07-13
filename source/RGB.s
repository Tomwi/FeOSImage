.global RGB24_to_ARGB16
.hidden RGB24_to_ARGB16
.type RGB24_to_ARGB16, %function

.arm
.align 2

@ r0 in
@ r1 dest
@ r2 = len


RGB24_to_ARGB16:

cmp r2, #0
bxeq lr

push { r4-r8 }
and r8, r2, #0x3
sub r2, r8
mov r6, #(0x1f<<27)
mov r7,#(1<<15)
orr r7, r7, #(1<<31)

.loop:

ldmia r0!,{r4-r5, ip}
@ extract pixel 1
and r3, r6, r4, lsl #24 @ red in upper five bits
orr r3, r3, r4, lsr #11 @ green (and other stuff too) in the lower bits
ror r3, #10				@ put green left of red (and five bits with possible garbage before ) in top bits
bic r3, r3, r6			@ clear garbage left of green
lsr r4, r4, #19			@ discard red and green
orr r3, r3, r4, lsl #27 @ put blue in the cleared bits left of green

@ extract pixel 2
and r4, r6, r4, lsl #19
orr r3, r4, r3, lsr #6
and r4, r6, r5, lsl #24
orr r3, r4, r3, lsr #5
and r4, r6, r5, lsl #16
orr r3, r4, r3, lsr #5

orr r3, r7, r3, lsr #1

and r4, r6, r5, lsl #8	@ red
and r5, r6, r5			@ green
orr r4, r5, r4, lsr #5
and r5, r6, ip, lsl #24	@ blue
orr r4, r5, r4, lsr #5

and r5, r6, ip, lsl #16	@ red
orr r4, r5, r4, lsr #6
and r5, r6, ip, lsl #8 @ green
orr r4, r5, r4, lsr #5
and r5, r6, ip @ blue
orr r4, r5, r4, lsr #5

orr r4, r7, r4, lsr #1

stmia r1!,{r3-r4}
subs r2,#4
bne .loop

cmp r8, #0
beq .end

.slowLoop:
	ldrb r3,[r0],#1
	subs r8,#1
	ldrb r4,[r0],#1
	orr  r3, r7, r3, lsr #3
	ldrb r5,[r0],#1
	lsr  r4, #3
	orr  r3, r3, r4, lsl #5
	lsr  r5, #3
	orr  r3, r3, r5, lsl #10
	strh r3,[r1],#2
	bne .slowLoop
.end:
pop {r4-r8}

bx lr





