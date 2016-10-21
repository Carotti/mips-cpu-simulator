.data
  op1: .word 400100
  op2: .word 0xFFFFFF12

.text
main:
  nop
  nop
  nop
  nop
  nop
  addi $t0, $zero, 54
  bltzal $t1, main
