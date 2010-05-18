#ifndef _COMPILER_H_
#define _COMPILER_H_

// by default NULL is undefined in C++
#include <stddef.h>

/// define the force inlining attribute for this compiler
#define __INLINE static __attribute__((__always_inline__)) inline

/// this is used to define the attribute value_in_regs to return structures as value
#define __VIR

// this is used to define the weak references
#define __WEAK

/// Align instantiated lvalue or struct member on 4 bytes
#define __ALIGN4 __attribute__((aligned(4)))

/// Likely to occur condition
#define likely(x) __builtin_expect(!!(x), 1)

/// Unlikely to occur condition
#define unlikely(x) __builtin_expect(!!(x), 0)

#endif // _COMPILER_H_
