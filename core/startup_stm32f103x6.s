.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global Reset_Handler
.global _estack

.section .isr_vector, "a", %progbits
.type isr_vector, %object
isr_vector:
    .word _estack                // Initial stack pointer
    .word Reset_Handler          // Reset handler

.section .text.Reset_Handler
.type Reset_Handler, %function
Reset_Handler:
    // Copy data section
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_sidata
1:
    cmp r0, r1
    ittt lt
    ldrlt r3, [r2], #4
    strlt r3, [r0], #4
    blt 1b

    // Zero BSS
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0
2:
    cmp r0, r1
    itt lt
    strlt r2, [r0], #4
    blt 2b

    bl main
    b .
.size Reset_Handler, . - Reset_Handler

