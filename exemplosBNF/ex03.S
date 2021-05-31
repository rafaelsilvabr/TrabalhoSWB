# Tradução BPL to Assembly

.globl f1

f1:

pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movl $1, -12(%rbp)

movl -12(%rbp), %eax
movl %eax, -16(%rbp)

movl %edi, %eax
movl -16(%rbp), %ecx
addl %ecx, %eax
movl %eax, -12(%rbp)

movl -12(%rbp), %eax
movl $-5, %ecx
imull %ecx, %eax
movl %eax, -16(%rbp)

movl -12(%rbp), %eax
jmp end_func

end_func:
leave
ret