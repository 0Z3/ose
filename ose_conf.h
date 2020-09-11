/*
Copyright (c) 2019-20 John MacCallum
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * @file ose_conf.h
 * @brief Configuration options.
 *
 * This file provides support for a variety of options including
 * extended types, and debugging. They can be enabled by passing the 
 * -D option to gcc-like compilers, for example 
 * @code{.txt} clang ... -DOSE_DEBUG ... @endcode
 */

#ifndef OSE_CONF_H
#define OSE_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sys/ose_endian.h"

/**
 * The Arduino IDE doesn't let you predefine with a -D switch, so
 * we require and include a header for Arduino projects.
 */
#if defined(ARDUINO) && !defined(OSE_CONF_IGNORE_ARDUINO)
#include "ose_defs.h"
#endif

#ifdef __DOXYGEN__
/** 
 * @brief Debug mode: enables assertions, and enforces the type of 
 * #osc_bundle by wrapping it in a struct.
*/
#define OSE_CONF_DEBUG
/**
 * @brief Define the behavior of the #ose_rassert macro. These definitons
 * are mutually exclusive. Defining OSE_CONF_RASSERT_FAIL causes
 * #ose_rassert to behave identically to #ose_assert. If no OSE_CONF_RASSERT
 * definition is provided, the default is OSE_CONF_RASSERT_FAIL.
 */
#define OSE_CONF_RASSERT_FAIL
/**
 * @brief Define the behavior of the #ose_rassert macro. These definitons
 * are mutually exclusive. Defining OSE_CONF_RASSERT_THROW causes
 * a longjmp call to happen, and must be preceeded by a call to setjmp.
 * This can be done using the #OSE_TRY and #OSE_CATCH macros in #ose_assert.h.
 * If no OSE_CONF_RASSERT definition is provided, 
 * the default is OSE_CONF_RASSERT_FAIL.
 */
#define OSE_CONF_RASSERT_THROW
/** @brief Provide support for the OSC symbol extended type. */
#define OSE_CONF_PROVIDE_TYPE_SYMBOL
/** @brief Provide support for the OSC double extended type. */
#define OSE_CONF_PROVIDE_TYPE_DOUBLE
/** @brief Provide support for the OSC int8 extended type. */
#define OSE_CONF_PROVIDE_TYPE_INT8
/** @brief Provide support for the OSC uint8 extended type. */
#define OSE_CONF_PROVIDE_TYPE_UINT8
/** @brief Provide support for the OSC uint32 extended type. */
#define OSE_CONF_PROVIDE_TYPE_UINT32
/** @brief Provide support for the OSC int64 extended type. */
#define OSE_CONF_PROVIDE_TYPE_INT64
/** @brief Provide support for the OSC uint64 extended type. */
#define OSE_CONF_PROVIDE_TYPE_UINT64
/** @brief Provide support for the OSC timetag extended type. */
#define OSE_CONF_PROVIDE_TYPE_TIMETAG
/** @brief Provide support for the OSC true extended type. */
#define OSE_CONF_PROVIDE_TYPE_TRUE
/** @brief Provide support for the OSC false extended type. */
#define OSE_CONF_PROVIDE_TYPE_FALSE
/** @brief Provide support for the OSC null extended type. */
#define OSE_CONF_PROVIDE_TYPE_NULL
/** @brief Provide support for the OSC infinitum extended type. */
#define OSE_CONF_PROVIDE_TYPE_INFINITUM

#else // __DOXYGEN__

#ifdef OSE_CONF_DEBUG
#define OSE_DEBUG
#define OSE_SAFE_BUNDLE
#else
//#define NDEBUG
#endif

#if !defined(OSE_CONF_RASSERT_FAIL) && !defined(OSE_CONF_RASSERT_THROW)
#define OSE_RASSERT_FAIL
#elif defined(OSE_CONF_RASSERT_FAIL)
#ifdef OSE_CONF_RASSERT_THROW
#error "Only one OSE_CONF_RASSERT definition may be provided. "\
	"Found both OSE_CONF_RASSERT_FAIL and OSE_CONF_RASSERT_THROW."
#endif
#define OSE_RASSERT_FAIL
#elif defined(OSE_CONF_RASSERT_THROW)
#ifdef OSE_CONF_RASSERT_FAIL
#error "Only one OSE_CONF_RASSERT definition may be provided. "\
	"Found both OSE_CONF_RASSERT_FAIL and OSE_CONF_RASSERT_THROW."
#endif
#define OSE_RASSERT_THROW
#endif

#ifdef OSE_CONF_PROVIDE_TYPE_SYMBOL
#define OSE_PROVIDE_TYPE_SYMBOL
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_DOUBLE
#define OSE_PROVIDE_TYPE_DOUBLE
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_INT8
#define OSE_PROVIDE_TYPE_INT8
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_UINT8
#define OSE_PROVIDE_TYPE_UINT8
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_UINT32
#define OSE_PROVIDE_TYPE_UINT32
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_INT64
#define OSE_PROVIDE_TYPE_INT64
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_UINT64
#define OSE_PROVIDE_TYPE_UINT64
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_TIMETAG
#define OSE_PROVIDE_TYPE_TIMETAG
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_TRUE
#define OSE_PROVIDE_TYPE_TRUE
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_FALSE
#define OSE_PROVIDE_TYPE_FALSE
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_NULL
#define OSE_PROVIDE_TYPE_NULL
#endif
#ifdef OSE_CONF_PROVIDE_TYPE_INFINITUM
#define OSE_PROVIDE_TYPE_INFINITUM
#endif

#ifdef OSE_CONF_ADDRESS_ANONVAL
#define OSE_USER_ADDRESS_ANONVAL

#ifdef OSE_DEBUG
#include <stdint.h>
static const char * const OSE_ADDRESS_ANONVAL = OSE_CONF_ADDRESS_ANONVAL "\0\0\0\0";
static const int32_t OSE_ADDRESS_ANONVAL_NBYTES = sizeof(OSE_CONF_ADDRESS_ANONVAL) - 1;
static const int32_t OSE_ADDRESS_ANONVAL_LEN = OSE_ADDRESS_ANONVAL_NBYTES;
static const int32_t OSE_ADDRESS_ANONVAL_SIZE = OSE_ADDRESS_ANONVAL_NBYTES
	+ (4 - ((OSE_ADDRESS_ANONVAL_NBYTES) % 4));

#else

#define OSE_ADDRESS_ANONVAL OSE_CONF_ADDRESS_ANONVAL "\0\0\0\0"
#define OSE_ADDRESS_ANONVAL_NBYTES sizeof(OSE_CONF_ADDRESS_ANONVAL) - 1
#define OSE_ADDRESS_ANONVAL_LEN OSE_ADDRESS_ANONVAL_NBYTES
#define OSE_ADDRESS_ANONVAL_SIZE OSE_ADDRESS_ANONVAL_NBYTES \
	+ (4 - ((OSE_ADDRESS_ANONVAL_NBYTES) % 4))

#endif // OSE_DEBUG
#endif // OSE_CONF_ADDRESS_ANONVAL

#if defined(OSE_CONF_VM_INPUT_SIZE)		\
	&& defined(OSE_CONF_VM_STACK_SIZE)	\
	&& defined(OSE_CONF_VM_ENV_SIZE)	\
	&& defined(OSE_CONF_VM_CONTROL_SIZE)	\
	&& defined(OSE_CONF_VM_DUMP_SIZE)	\
	&& defined(OSE_CONF_VM_OUTPUT_SIZE)
#define OSEVM_HAVE_SIZES
#define OSEVM_INPUT_SIZE OSE_CONF_VM_INPUT_SIZE
#define OSEVM_STACK_SIZE OSE_CONF_VM_STACK_SIZE
#define OSEVM_ENV_SIZE OSE_CONF_VM_ENV_SIZE
#define OSEVM_CONTROL_SIZE OSE_CONF_VM_CONTROL_SIZE
#define OSEVM_DUMP_SIZE OSE_CONF_VM_DUMP_SIZE
#define OSEVM_OUTPUT_SIZE OSE_CONF_VM_OUTPUT_SIZE

#elif !defined(OSE_CONF_VM_INPUT_SIZE)		\
	&& !defined(OSE_CONF_VM_STACK_SIZE)	\
	&& !defined(OSE_CONF_VM_ENV_SIZE)	\
	&& !defined(OSE_CONF_VM_CONTROL_SIZE)	\
	&& !defined(OSE_CONF_VM_DUMP_SIZE)	\
	&& !defined(OSE_CONF_VM_OUTPUT_SIZE)

#else
#warning Sizes were specified for some, but not all of the VM stacks.	\
	Sizes specified will be ignored during compilation,		\
	and must be passed to osevm_init() at runtime.
#endif

#ifdef OSE_CONF_SYMTAB_FNSYMS
#define OSE_SYMTAB_FNSYMS
#endif

#ifdef OSE_CONF_USE_OPTIMIZED_CODE
#define OSE_USE_OPTIMIZED_CODE
#endif
	
#endif // __DOXYGEN__

#ifdef __cplusplus
}
#endif

#endif
