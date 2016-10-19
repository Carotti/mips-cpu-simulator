.data
  op1: .word 0x0BF9F147
  op2: .word 0xFFFFFF12

.text
main:
  lw $t3, op1
  lw $t4, op2
  add $t0, $t3, $t4
