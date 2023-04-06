nop

addi $1, $0, 5  # r1 = 5
addi $2, $0, 3  # r2 = 3
j 5  # PC=10
nop
bne $2, $2, 1 # if($2 !=$1) PC=PC+1+N
nop
jal 11
addi $5,$31, 0
jr $2
blt $1, $2, 4
setx 0
addi $6,$30, 0
bex 8

