/* libpeos-compat: alloca (the compiler's). */
#pragma once
#include <stddef.h>
#ifndef alloca
#define alloca __builtin_alloca
#endif
