.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

.global Reset_Handler
.global _estack
.global xPortSysTickHandler
.global PendSV_Handler
.global SVC_Handler

.section .isr_vector, "a", %progbits
.type isr_vector, %object
isr_vector:
    .word _estack                // 0: Initial Stack Pointer
    .word Reset_Handler          // 1: Reset Handler
    .word Default_Handler        // 2: NMI
    .word Default_Handler        // 3: Hard Fault
    .word Default_Handler        // 4: MemManage
    .word Default_Handler        // 5: BusFault
    .word Default_Handler        // 6: UsageFault
    .word 0                      // 7: Reserved
    .word 0                      // 8: Reserved
    .word 0                      // 9: Reserved
    .word 0                      // 10: Reserved
    .word SVC_Handler            // 11: SVCall
    .word Default_Handler        // 12: Debug Monitor
    .word 0                      // 13: Reserved
    .word PendSV_Handler         // 14: PendSV
    .word xPortSysTickHandler    // 15: SysTick

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

// Default weak handlers
.section .text.Default_Handler, "ax", %progbits
.type Default_Handler, %function
Default_Handler:
    b .

.weak SVC_Handler
.set SVC_Handler, Default_Handler

.weak PendSV_Handler
.set PendSV_Handler, Default_Handler

.weak xPortSysTickHandler
.set xPortSysTickHandler, Default_Handler
