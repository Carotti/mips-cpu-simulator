.text
  # $a0 contains the number to check if prime
  main:
    addiu $a0, $0, 7
    jal prime
end:    nop
        j end
        nop

prime:
    addi $t4, $0, 2
    divu $a0, $t4
    mflo $t3
    addi $v0, $0, 1 # Assume the number is prime
    addi $t0, $0, 1
loop:
    addi $t0, $t0, 1
    divu $a0, $t0
    mfhi $t1
    beq $t1, $0, notPrime
    nop
    bne $t0, $t3, loop
    nop
    j $ra
    nop

notPrime:
    addi $v0, $0, 0
    j $ra
    nop
