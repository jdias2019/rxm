
bits 32

nop
hlt
int3
pushf
popf
leave
ret
retf
ret 4

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

mov eax, 0x00000001
mov ecx, 0x0000FFFF
mov edx, 0xDEADBEEF
mov ebx, 0x12345678
mov esp, 0x00000000
mov ebp, 0xCAFEBABE
mov esi, 0xAAAAAAAA
mov edi, 0xFFFFFFFF

call next1
next1:
jmp  next2
next2:

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

int 0x80
int 0x03

mov eax, ebx
mov ecx, edx
mov esi, edi

mov ebx, eax
mov edx, ecx

mov eax, [ebx]
mov ecx, [esi]
mov edx, [edi]

mov eax, [ebp + 8]
mov ecx, [ebp - 4]
mov edx, [ebx + 16]

mov eax, [ebp + 0x100]
mov ecx, [ebx + 0x1000]

mov [ebp - 4],  eax
mov [ebp - 8],  ecx
mov [ebp + 12], edx

add [ebx],       eax
add [ebp - 4],   ecx
add eax,         ecx

sub [esi],       edx
sub [ebp + 8],   eax
sub ecx,         edx

cmp [edi],       eax
cmp [ebp - 8],   ecx
cmp eax,         ebx

mov eax, [ebx + esi]
mov eax, [ebx + esi*2]
mov eax, [ebx + esi*4]
mov eax, [ebx + esi*8]

mov eax, [ebx + esi*4 + 0x10]
mov ecx, [edx + edi*2 + 0x08]

mov eax, [ebx + esi*4 + 0x1000]
mov ecx, [edx + edi*8 + 0x2000]

mov eax, [esp]
mov eax, [esp + 4]
mov eax, [esp + 0x100]
