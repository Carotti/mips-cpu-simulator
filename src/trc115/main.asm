.data
  val: .word 0xFFFFFF12
.text
main:

  lw $9, val
  slti $8, $9, 0x8000
