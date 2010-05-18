#ifndef _ARCH_H_
#define _ARCH_H_

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

/// use the standard assertion mechanism
#define ASSERT_WARN assert
#define ASSERT_ERR assert
#define ASSERT_ERR2(a,b,c) assert(a)

#endif // _ARCH_H_
