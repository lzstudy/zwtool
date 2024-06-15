
#define FLAG_ADDR            0x40000000

.global _start  /* 全局标号 */

_start:
	MOV		x3, #1
	MOV		x4, #FLAG_ADDR
	STR		x3, [x4]

loop:
	b		loop
