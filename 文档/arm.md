# 能够使用的指令集（待完善）
## MOV
    MOV R1,R0
    MOV R0,#5

## ADD
    ADD R0,R1,R2
    ADD RO,R1,#5

## SUB
    SUB R0,R1,R2
    SUB R0,R1,#5
    
## MUL
    MUL RO,R1,R2
    #无立即数乘
    
## LDR和STR   
    LDR R5,[R6,#0X08]
    LDR R5,[R6,R3]
    STR R5,[R6,#0X08]
    STR R5,[R6,R3]

## CMP
    CMP R1,R2
    CMP R1,#5
     
## B和BL
    B LABEL
    BEQ LABEL
    BL LABEL

    
    
    
    
    
