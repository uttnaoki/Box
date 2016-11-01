	INITIAL_GP = 0x10008000
	INITIAL_SP = 0x7ffffffc
	framesize = 32
	ra_loc = 28
	fp_loc = 24
	# system call service number
	stop_service = 99

init:
	# initialize $gp (global pointer) and $sp (stack pointer)
	la      $gp, INITIAL_GP         # $gp <- 0x10008000 (INITIAL_GP)
	la      $sp, INITIAL_SP         # $sp <- 0x7ffffffc (INITIAL_SP)
	jal     main                    # jump to `main'
	nop                             #   (delay slot)
	li      $v0, stop_service       # $v0 <- 99 (stop_service)
	syscall                         # halt
	nop
	# not reach here
stop:                                   #if syscall return
	j stop                          #infinite loop...
	nop


main:
	addi    $sp, $sp, -framesize    # allocate new stack frame
	sw      $ra, ra_loc($sp)        # save $ra to stack frame
	sw      $fp, fp_loc($sp)        # save $fp to stack frame
	add     $fp, $sp, $zero             # $fp <- $sp

	la $t2, a       #変数のアドレスを格納
	li $v0, 10	#整数
	addi $sp,$sp,-4
	sw $v0,0($sp)
	sw $v0, 0($t2)    #変数の値を更新
	addi $sp, $sp, 4
$if1:
	la $v0, a
	lw $v0, 0($v0)
	nop
	addi $sp,$sp,-4
	sw $v0,0($sp)
	add $t8, $v0, $zero
	addi $sp, $sp, 4
	li $v0, 10	#整数
	addi $sp,$sp,-4
	sw $v0,0($sp)
	addi $sp, $sp, 4
	slt $t3, $t8, $v0
	beq $t3, $zero, $skipif1
	nop
	la $t2, a       #変数のアドレスを格納
	li $v0, 1	#整数
	addi $sp,$sp,-4
	sw $v0,0($sp)
	sw $v0, 0($t2)    #変数の値を更新
	addi $sp, $sp, 4
$skipif1:
$ifend1:
	add $sp, $fp, $zero
	nop
	lw $fp, fp_loc($sp)
	lw $ra, ra_loc($sp)
	nop
	addi $sp, $sp, framesize
	jr $ra
	nop
	.data 0x10004000
result: .word 0
a: .word 0
