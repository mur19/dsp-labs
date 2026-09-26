;
; Branch Instruction to Program Start 
;

	.ref  init_code_start

	.sect "codestart"

	.global BOOTLOAD

BOOTLOAD:	LB init_code_start
