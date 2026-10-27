.intel_syntax noprefix
.section .text
.global _start

// Minimal C runtime entry:
// - assumes the kernel already set up a valid user stack
// - calls main()
// - exits via int 0x80
.extern main

_start:
    // argc at [rsp], argv at [rsp+8].
    mov rdi, [rsp]        // argc
    lea rsi, [rsp+8]      // argv

    // SysV ABI: before a call, RSP must be 16-byte aligned.
    // call then pushes return address, so main() sees RSP%16==8.
    and rsp, -16

    call main

    // rax = return code from main
    mov rdi, rax
    mov rax, 3          // PEOS_SYS_exit
    int 0x80

    hlt
    jmp .

.section .note.GNU-stack,"",@progbits
