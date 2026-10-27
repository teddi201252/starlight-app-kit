#pragma once

#include <stdint.h>

typedef uint64_t jmp_buf[8];

int setjmp(jmp_buf env) __attribute__((returns_twice));
__attribute__((noreturn)) void longjmp(jmp_buf env, int value);

