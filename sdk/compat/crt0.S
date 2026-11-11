// libpeos-compat's program entry (ports, docs/d2decomped-port-plan.md):
// the stack as the kernel leaves it (argc, argv), then C: the first thread's
// TLS, the constructors, main, exit (atexit, C++'s destructors).
.intel_syntax noprefix
.section .text
.global _start
.extern __peos_compat_start

_start:
    mov rdi, [rsp]        // argc
    lea rsi, [rsp+8]      // argv
    xor rbp, rbp          // the end of the frame chain, for unwinders
    and rsp, -16
    call __peos_compat_start
    hlt
    jmp .

.section .note.GNU-stack,"",@progbits
