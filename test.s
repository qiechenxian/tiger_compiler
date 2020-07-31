	.arch   armv7-a
	.file   "0_my_test.c"
	.data
	.text
	.align  2
	.global main
	.type   main, %function
main:
	stmfd   sp!, {fp, lr}
	add     FP, SP, #4
	sub     SP, SP, #12
	ldr     R9, [FP, #-8]
	str     r0, [SP, #0]
	ldr     r0, [R9]
	bl      putint
	ldr     r0, [SP, #0]
	ldr     r1, [SP, #-4]
	ldr     r2, [SP, #-8]
	ldr     r3, [SP, #-12]
	ldr     R9, [FP, #-8]
	mov     R0, R9
	b       .L0
.L0:
NOP
	sub     sp, fp, #4
	ldmfd   sp!, {fp, lr}
	bx      lr
	.size   main, .-main
