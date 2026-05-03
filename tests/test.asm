; test.asm — tests every opcode and addressing mode supported by rxm
; assemble with: nasm -f bin test.asm -o test.bin
; disassemble with: ./rxm test.bin

bits 32

; --- no operands ---
nop
hlt
int3
pushf
popf
leave
ret
retf
ret 4

; --- PUSH/POP all registers ---
push eax
push ecx
push edx
push ebx
push esp
push ebp
push esi
push edi

pop eax
pop ecx
pop edx
pop ebx
pop esp
pop ebp
pop esi
pop edi

; --- MOV reg, imm32 ---
mov eax, 0x00000001
mov ecx, 0x0000FFFF
mov edx, 0xDEADBEEF
mov ebx, 0x12345678
mov esp, 0x00000000
mov ebp, 0xCAFEBABE
mov esi, 0xAAAAAAAA
mov edi, 0xFFFFFFFF

; --- CALL and JMP with immediate ---
call next1
next1:
jmp  next2
next2:

; --- conditional jumps rel8 ---
jo   next3
jno  next3
jb   next3
jnb  next3
je   next3
jne  next3
jbe  next3
ja   next3
js   next3
jns  next3
jp   next3
jnp  next3
jl   next3
jge  next3
jle  next3
jg   next3
next3:

; --- INT ---
int 0x80
int 0x03

; --- MOV r/m32, r32  (0x89) — mod 3: register direct ---
mov eax, ebx
mov ecx, edx
mov esi, edi

; --- MOV r32, r/m32  (0x8B) — mod 3: register direct ---
mov ebx, eax
mov edx, ecx

; --- MOV r32, r/m32  (0x8B) — mod 0: [reg] ---
mov eax, [ebx]
mov ecx, [esi]
mov edx, [edi]

; --- MOV r32, r/m32  (0x8B) — mod 1: [reg + disp8] ---
mov eax, [ebp + 8]
mov ecx, [ebp - 4]
mov edx, [ebx + 16]

; --- MOV r32, r/m32  (0x8B) — mod 2: [reg + disp32] ---
mov eax, [ebp + 0x100]
mov ecx, [ebx + 0x1000]

; --- MOV r/m32, r32  (0x89) — mod 1: [reg + disp8] ---
mov [ebp - 4],  eax
mov [ebp - 8],  ecx
mov [ebp + 12], edx

; --- ADD r/m32, r32  (0x01) ---
add [ebx],       eax
add [ebp - 4],   ecx
add eax,         ecx

; --- SUB r/m32, r32  (0x29) ---
sub [esi],       edx
sub [ebp + 8],   eax
sub ecx,         edx

; --- CMP r/m32, r32  (0x39) ---
cmp [edi],       eax
cmp [ebp - 8],   ecx
cmp eax,         ebx

; --- SIB: [base + index*scale] ---
mov eax, [ebx + esi]
mov eax, [ebx + esi*2]
mov eax, [ebx + esi*4]
mov eax, [ebx + esi*8]

; --- SIB: [base + index*scale + disp8] ---
mov eax, [ebx + esi*4 + 0x10]
mov ecx, [edx + edi*2 + 0x08]

; --- SIB: [base + index*scale + disp32] ---
mov eax, [ebx + esi*4 + 0x1000]
mov ecx, [edx + edi*8 + 0x2000]

; --- SIB: no index (index == 4) ---
mov eax, [esp]
mov eax, [esp + 4]
mov eax, [esp + 0x100]
