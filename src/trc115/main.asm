.data
  op1: .word 400100
  op2: .word 0xFFFFFF12

.text
main:
  add $t0, $t3, $t4
  lw $t0, op1

  addi $t3, $zero, 0xAB

  addiu  $t2, $t2, 4  # We'll now iterate over [$2+4, $10] instead of [$2, $10[
  LOOP:       addi         $t1, $t1, 1
              bne          $t1, $t4, LOOP
              addiu        $t2, $t2, 4  # Use the delay slot to increase $2
