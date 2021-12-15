.arch   armv7-a
.file   "./test.c"
.data
.text
	mov     'd0, #1
	.align  2
	.global main
	.type   main, %function
main:
	stmfd   SP!, {R4-R9, FP, LR}
	add     FP, SP, #28
	ldr     R4, =1348
	sub     SP, SP, R4
	mov     R5, #1
	mov     R4, #1
	mov     R9, #2
	mov     R7, #3
	mov     R8, #4
.L2:
	mov     R0, #-125
	cmp     R0, #0
	beq     .L1
.L6:
.L4:
	cmp     R4, #16
	bgt     .L3
.L5:
	ldr     R0, =-1324
	add     R0, FP, R0
	mov     R6, #72
	mla     R0, R5, R6, R0
	add     R0, R0, R4, lsl #2
	mov     R6, #2
	str     R6, [R0]
	add     R0, R4, #1
	mov     R4, R0
	ldr     R0, =-1324
	add     R0, FP, R0
	mov     R6, #72
	mla     R0, R5, R6, R0
	add     R0, R0, R4, lsl #2
	ldr     R0, [R0]
	bl      putint
	b       .L4
.L3:
	add     R0, R5, R9
	add     R0, R0, R7
	add     R0, R0, R8
	add     R0, R0, #1
	mov     R5, R0
	b       .L2
.L1:
	mov     R0, #1
.L0:
	sub     SP, FP, #28
	ldmfd   SP!, {R4-R9, FP, LR}
	bx      LR
	.size   main, .-main
