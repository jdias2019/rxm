bits 32

; MOV reg, imm32
mov eax, 0x00000001
mov ebx, 0x00000002
mov ecx, 0x00000003

; PUSH / POP
push eax
push ebx
pop ecx
pop edx

; CALL e JMP
call 0x100
jmp 0x200
jmp short $+2

; conditional jumps rel8
je short $+2
jne short $+2
jl short $+2
jge short $+2

; INT
int 0x80

; misc
nop
ret
hlt
int3
pushf
popf
leave
