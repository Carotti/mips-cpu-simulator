.data
  val: .word 0xFFFFFF12
.text
main:

  lw $9, val
  sltiu $8, $9, 0xFFFF
