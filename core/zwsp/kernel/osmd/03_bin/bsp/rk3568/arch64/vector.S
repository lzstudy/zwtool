    .section .os.vector.text, "ax"

    .global  OsVectorTable

    .global  OsHwiDispatcher
    .type    OsHwiDispatcher, function

    .global  OsVectorTable
    .type    OsVectorTable,function

    .global  OsResetVector
    .type    OsResetVector, function

OsExcDispatch:
loop:
    b loop


.macro EXC_HANDLE vecId
    stp x1, x0, [sp,#-16]!
    mov x1, #\vecId
    b   OsExcDispatch
.endm

    .align 13

OsVectorTable:
.set    VBAR, OsVectorTable
.org VBAR                                // Synchronous, Current EL with SP_EL0
    EXC_HANDLE  0

.org (VBAR + 0x80)                       // IRQ/vIRQ, Current EL with SP_EL0
    EXC_HANDLE  1

.org (VBAR + 0x100)                      // FIQ/vFIQ, Current EL with SP_EL0
    EXC_HANDLE  2

.org (VBAR + 0x180)                      // SERROR, Current EL with SP_EL0
    EXC_HANDLE  3

.org (VBAR + 0x200)                      // Synchronous, Current EL with SP_ELx
    EXC_HANDLE  4

.org (VBAR + 0x280)                      // IRQ/vIRQ, Current EL with SP_ELx
    EXC_HANDLE  5
#    stp x1, x0, [sp,#-16]!
#    mov x1, #5
#    b   OsHwiDispatcher

.org (VBAR + 0x300)                      // FIQ/vFIQ, Current EL with SP_ELx
    EXC_HANDLE  6

.org (VBAR + 0x380)                      // SERROR, Current EL with SP_ELx
    EXC_HANDLE  7

.org (VBAR + 0x400)                      // Synchronous, EL changes and the target EL is using AArch64
    EXC_HANDLE  8

.org (VBAR + 0x480)                      // IRQ/vIRQ, EL changes and the target EL is using AArch64
    EXC_HANDLE  9

.org (VBAR + 0x500)                      // FIQ/vFIQ, EL changes and the target EL is using AArch64
    EXC_HANDLE  10
.org (VBAR + 0x580)                      // SERROR, EL changes and the target EL is using AArch64
    EXC_HANDLE  11
.org (VBAR + 0x600)                      // Synchronous, L changes and the target EL is using AArch32
    EXC_HANDLE  12

.org (VBAR + 0x680)                      // IRQ/vIRQ, EL changes and the target EL is using AArch32
    EXC_HANDLE  13

.org (VBAR + 0x700)                      // FIQ/vFIQ, EL changes and the target EL is using AArch32
    EXC_HANDLE  14
.org (VBAR + 0x780)                      // SERROR, EL changes and the target EL is using AArch32
    EXC_HANDLE  15

    .text


OsResetVector:
    LDR x0, =OsVectorTable
    MSR VBAR_EL1, X0
    ret