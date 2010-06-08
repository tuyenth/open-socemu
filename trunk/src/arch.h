/** @file arch.h
 * @brief Definition of some architecture dependent macros
 */

#ifndef _ARCH_H_
#define _ARCH_H_

#include <stdint.h>
#include <assert.h>
#include <stdio.h>

/// use the standard assertion mechanism
#define ASSERT_WARN assert
#define ASSERT_ERR assert

#endif // _ARCH_H_
