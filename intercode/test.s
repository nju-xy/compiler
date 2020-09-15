.data
_prompt: .asciiz "Enter an integer:"
_ret: .asciiz "\n"
.globl main
.text
read:
  li $v0, 4
  la $a0, _prompt
  syscall
  li $v0, 5
  syscall
  jr $ra

write:
  li $v0, 1
  syscall
  li $v0, 4
  la $a0, _ret
  syscall
  move $v0, $0
  jr $ra

func_fact:
  addi $fp, $sp, 8
  li $t1, -24
  add $sp, $fp, $t1
  lw $t1, 0($fp)
  sw $t1, -12($fp)
  lw $t1, -12($fp)
  li $t2, 1
  bne $t1, $t2, label2
  lw $t1, -12($fp)
  move $v0, $t1
  addi $sp, $fp, -8
  li $v1, 4
  jr $ra
  j label1
label2:
  lw $t1, -12($fp)
  li $t2, 1
  sub $t1, $t1, $t2
  sw $t1, -16($fp)
  lw $t1, -16($fp)
  addi $sp, $sp, -4
  sw $t1, 0($sp)
  addi $sp, $sp, -8
  sw $ra, 0($sp)
  sw $fp, 4($sp)
  jal func_fact
  lw $fp, 4($sp)
  lw $ra, 0($sp)
  addi $sp, $sp, 8
  add $sp, $sp, $v1
  move $t1, $v0
  sw $t1, -20($fp)
  lw $t1, -12($fp)
  lw $t2, -20($fp)
  mul $t1, $t1, $t2
  sw $t1, -24($fp)
  lw $t1, -24($fp)
  move $v0, $t1
  addi $sp, $fp, -8
  li $v1, 4
  jr $ra
label1:

main:
  addi $fp, $sp, 8
  li $t1, -24
  add $sp, $fp, $t1
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal read
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $t1, $v0
  sw $t1, -12($fp)
  lw $t1, -12($fp)
  sw $t1, -16($fp)
  lw $t1, -16($fp)
  li $t2, 1
  ble $t1, $t2, label5
  lw $t1, -16($fp)
  addi $sp, $sp, -4
  sw $t1, 0($sp)
  addi $sp, $sp, -8
  sw $ra, 0($sp)
  sw $fp, 4($sp)
  jal func_fact
  lw $fp, 4($sp)
  lw $ra, 0($sp)
  addi $sp, $sp, 8
  add $sp, $sp, $v1
  move $t1, $v0
  sw $t1, -20($fp)
  lw $t1, -20($fp)
  sw $t1, -24($fp)
  j label4
label5:
  li $t1, 1
  sw $t1, -24($fp)
label4:
  lw $t1, -24($fp)
  move $a0, $t1
  addi $sp, $sp, -4
  sw $ra, 0($sp)
  jal write
  lw $ra, 0($sp)
  addi $sp, $sp, 4
  move $v0, $0
  li $t1, 0
  move $v0, $t1
  addi $sp, $fp, -8
  li $v1, 0
  jr $ra
