# Tradução BPL to Assembly

.globl f1

f1:

pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movl -8(%rbp), %eax
jmp end_func

end_func:
leave
ret

.globl f2

f2:

pushq %rbp
movq %rsp, %rbp
subq $144, %rsp
movl $1, %eax
movl -12(%rbp), %ecx
movl %eax, -12(%rbp)

movl -12(%rbp), %edi
leaq -136(%rbp), %rsi
call f1
movl %eax, -16(%rbp)

movl $5, %edi
movq -8(%rbp), %rsi
call f1
movl %eax, -16(%rbp)

movl -16(%rbp), %eax
jmp end_func

end_func:
leave
ret