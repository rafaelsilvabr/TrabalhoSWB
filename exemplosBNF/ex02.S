# Tradução BPL to Assembly

.globl f1

f1:

pushq %rbp
movq %rsp, %rbp
movl $0, %eax
jmp end_func

end_func:
leave
ret

.globl f2

f2:

pushq %rbp
movq %rsp, %rbp
subq $144, %rsp
movl $-1, %eax
jmp end_func

end_func:
leave
ret

.globl f3

f3:

pushq %rbp
movq %rsp, %rbp
subq $144, %rsp
movl $5, %eax
jmp end_func

end_func:
leave
ret