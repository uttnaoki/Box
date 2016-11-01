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
	ori     $fp, $sp, 0             # $fp <- $sp

	la $t2, i       #変数のアドレスを格納
	li $v0, 1	#整数
	addi $sp,$sp,-4
	sw $v0,0($sp)
	sw $v0, 0($t2)    #変数の値を更新
	addi $sp, $sp, 4

	la $t8, i
	li $v0, 11	#整数
	addi $sp,$sp,-4
	sw $v0,0($sp)
	add $t9, $v0, $zero
	la $t8, i
	lw $t8, 0($t8)
	nop
	slt $t3, $t8, $t9

	beq $t3, $zero, LoopE1
	nop
LoopS1:
	la $t2, sum       #変数のアドレスを格納
	la $v0, i        #変数
	lw $v0, 0($v0)
	nop
	addi $sp,$sp,-4
	sw $v0,0($sp)
	la $v0, sum        #変数
	lw $v0, 0($v0)
	nop
	addi $sp,$sp,-4
	sw $v0,0($sp)
#pop
	lw $t0, 0($sp)
	lw $t1, 4($sp)
	nop
	addi $sp,$sp,8

	add $v0, $t0, $t1

	addi $sp,$sp,-4
	sw $v0,0($sp)
	sw $v0, 0($t2)    #変数の値を更新
	addi $sp, $sp, 4

	la $t2, i       #変数のアドレスを格納
	li $v0, 1	#整数
	addi $sp,$sp,-4
	sw $v0,0($sp)
	la $v0, i        #変数
	lw $v0, 0($v0)
	nop
	addi $sp,$sp,-4
	sw $v0,0($sp)
#pop
	lw $t0, 0($sp)
	lw $t1, 4($sp)
	nop
	addi $sp,$sp,8

	add $v0, $t0, $t1

	addi $sp,$sp,-4
	sw $v0,0($sp)
	sw $v0, 0($t2)    #変数の値を更新
	addi $sp, $sp, 4

	la $t8, i
	lw $t8, 0($t8)
	nop
	slt $t3, $t8, $t9
	beq $t3, $zero, LoopE1
	nop
	j   LoopS1
LoopE1:

	la $t2, result       #変数のアドレスを格納
	la $v0, sum        #変数
	lw $v0, 0($v0)
	nop
	addi $sp,$sp,-4
	sw $v0,0($sp)
	sw $v0, 0($t2)    #変数の値を更新
	addi $sp, $sp, 4

	lw $fp, fp_loc($sp)
	lw $ra, ra_loc($sp)
	nop
	addi $sp, $sp, framesize
	lw $ra, 0($sp)
	nop
	jr $ra
	nop
	.data 0x10004000
result: .word 0
i: .word 0
sum: .word 0
