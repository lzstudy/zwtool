
#define FLAG_ADDR            0x50000000
// #defien PHY_ADDR			 0x60000000

.global _start  /* 全局标号 */

_start:
	MOV		x20, #1
	MOV		x21, #FLAG_ADDR
	STR		x20, [x21]

	# 设置堆栈
	LDR     x1, =0x61000000
	BIC     sp, x1, #0xF

	# 设置中断向量表
	BL		OsResetVector

	# 清bss
	LDR 	x0, =__bss_start
	LDR 	x1, =__bss_end
	MOV 	x2, x0
clear_bss:
	CMP 	x0, x1
	BEQ 	done_clearing_bss
	STR 	xzr, [x0], #8
	B 		clear_bss

done_clearing_bss:
	# 跳转到main函数
	bl 		main

exit_loop:
	b		exit_loop
