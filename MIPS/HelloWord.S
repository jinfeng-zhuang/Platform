###########################################

    .text

main:
    
    la $a0, message
    li $a1, 0x11223344
    sw $a1, ($a0)
    li $v0, 4
    syscall
    
    jr $ra

###########################################
    
    .data

message:
    .asciiz "Hello World!\n"