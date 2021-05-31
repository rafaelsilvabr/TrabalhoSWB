# Tradução BPL to Assembly

.globl f1

f1:

pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movl $3, -12(%rbp)

movl -12(%rbp), %ecx
cmpl $0, %ecx
je d_if1

movl $0, -12(%rbp)

d_if1:
movl -12(%rbp), %ecx
cmpl $0, %ecx
je d_if2

jmp end_func

d_if2:
movl $-1, %eax
jmp end_func

end_func:
leave
ret