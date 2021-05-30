.globl f1

f1:

pushq %rbp
movq %rsp, %rbp
subq $64, %rsp
movl %edi, %eax
movl $1, %ecx
addl %ecx, %eax
movl %eax, -20(%rbp)

movq $5, %rax
imulq $4, %rax
leaq -60(%rbp), %rcx
addq %rcx, %rax
movl $2, %ecx
movl %ecx, (%rax)

movq $3, %rax
imulq $4, %rax
leaq -16(%rbp), %rcx
addq %rcx, %rax
movl -20(%rbp), %ecx
movl %ecx, (%rax)

movq $5, %rax
imulq $4, %rax
leaq -60(%rbp), %rcx
addq %rcx, %rax
movl (%rax), %ecx
movl %ecx, -20(%rbp)

movl -8(%rbp), %eax
jmp end_func

end_func:
leave
ret