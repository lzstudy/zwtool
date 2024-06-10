
#define PHY_ADDR            0x70000000

.global _start  /* 全局标号 */

_start:
	MOV		x3, #66
	MOV		x4, #PHY_ADDR
	STR		x3, [x4]

loop:
	b		loop
