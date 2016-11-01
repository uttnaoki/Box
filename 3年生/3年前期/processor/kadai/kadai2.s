#
# sum2.s
#
        INITIAL_GP = 0x10008000
        INITIAL_SP = 0x7ffffffc
        # system call service number
        stop_service = 99

        .text
        # е€ќжњџеЊ–гѓ«гѓјгѓЃгѓі
init:
        # initialize $gp (global pointer) and $sp (stack pointer)
        la      $gp, INITIAL_GP         # $gp <- 0x10008000 (INITIAL_GP)
        la      $sp, INITIAL_SP         # $sp <- 0x7ffffffc (INITIAL_SP)
        jal     main                    # jump to `main'
        nop                             #   (delay slot)
        li      $v0, stop_service       # $v0 <- 99 (stop_service)
        syscall                         # stop
        nop
        # not reach here
stop:                                   # if syscall return 
        j stop                          # infinite loop...
        nop

        .text   0x00001000
	# mainгѓ«гѓјгѓЃгѓі
main:
        sw      $ra, 0($sp)             # $raгЃ®еЂ¤г‚’г‚№г‚їгѓѓг‚ЇгЃ«дїќе­�гЃ—гЃ¦гЃЉгЃЏ
        lw      $a0, 0xc004($gp)        # $a0 <- mem[0x10004004]
                                        # гѓ‡гѓјг‚їгЃ®е…€й ­г‚ўгѓ‰гѓ¬г‚№
        lw      $a1, 0xc008($gp)        # $a1 <- mem[0x10004008]
                                        # гѓ‡гѓјг‚їгЃ®еЂ‹ж•°
	jal     sum                     # г‚µгѓ–гѓ«гѓјгѓЃгѓі`sum'г‚’е‘јгЃіе‡єгЃ™
        nop                             # (йЃ…е»¶г‚№гѓ­гѓѓгѓ€)
        sw      $v0, 0xc000($gp)        # mem[0x10004000] <- $v0
                                        # зµђжћњг‚’0x10004000з•Єењ°гЃ«ж јзґЌ
        lw      $ra, 0($sp)             # $raгЃ®еЂ¤г‚’г‚№г‚їгѓѓг‚ЇгЃ‹г‚‰еѕ©е…ѓ
        nop                             # (йЃ…е»¶г‚№гѓ­гѓѓгѓ€)гЂЂгЂЂгЂЂгЂЂгЂЂгЂЂ
        jr      $ra                     # гѓЄг‚їгѓјгѓі
        nop                             # (йЃ…е»¶г‚№гѓ­гѓѓгѓ€)
							     
sum:
	ori	$v0, $zero, 0
	nop
_loop:
	beq	$a1, $zero, _end
	lw	$v1, 0($a0)
	nop
	add	$v0, $v0, $v1
	addi	$a0, $a0, 4
	addi	$a1, $a1, -1
	j	_loop
	nop
_end:	
	jr      $ra                     # гѓЄг‚їгѓјгѓі
        nop                             # (йЃ…е»¶г‚№гѓ­гѓѓгѓ€)

        #
        # data segment
        #
        .data   0x10004000
RESULT: .word   0xffffffff
BASE:   .word   0x10004100
N:      .word   5

        .data 0x10004100
DATA:   .word   1, 2, 3, 4, 5
        
# End of file (sum2.s)
