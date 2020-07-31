	.arch   armv7-a
	.file   "test.c"
	.data
	.text
	.align  2
	.global main
	.type   main, %function
main:
	stmfd   sp!, {fp, lr}
	add     FP, SP, #4
	sub     SP, SP, #8
	mov     R9, #10
	str     R9, [FP, #-8]
.L1:
	ldr     R9, [FP, #-8]
	mov     R4, #0
	cmp     R9, R4
	bgt     .L4
	b       .L0
.L0:
	ldr     R9, [FP, #-8]
	mov     R0, R9
	b       .L5
.L4:
	ldr     R9, [FP, #-8]
	mov     R4, #5
	cmp     R9, R4
	bgt     .L2
	b       .L3
.L3:
	ldr     R9, [FP, #-8]
	mov     R0, R9
	b       .L1
.L2:
	ldr     R9, [FP, #-8]
	sub     R9, R9, #1
	str     R9, [FP, #-8]
	b       .L1
.L5:
NOP
	sub     sp, fp, #4
	ldmfd   sp!, {fp, lr}
	bx      lr
	.size   main, .-main