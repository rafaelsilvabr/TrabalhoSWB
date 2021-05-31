# Tradução BPL to Assembly

.globl f1

f1:

pushq %rbp
movq %rsp, %rbp
subq $48, %rsp
movl %edi, %eax
movl $1, %ecx
cltd
idivl %ecx
movl %eax, -20(%rbp)

movl $5, -40(%rbp)

movq $2, %rax
imulq $4, %rax
leaq -36(%rbp), %rcx
addq %rcx, %rax
movl -40(%rbp), %ecx
movl %ecx, (%rax)

movl -40(%rbp), %ecx
cmpl $0, %ecx
je d_if1

movl -40(%rbp), %eax
jmp end_func

d_if1:
movq $5, %rax
imulq $4, %rax
leaq -36(%rbp), %rcx
addq %rcx, %rax
movl (%rax), %ecx
movl %ecx, -20(%rbp)

movl -20(%rbp), %eax
jmp end_func

end_func:
leave
ret