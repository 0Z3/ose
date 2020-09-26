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
 * These can be changed here or defined when invoking `make` if not building
 * a library to be linked against later, since these values are needed
 * in different header files. 
 * 
 * If these are commented out, the osevm will be built with a version
 * of #osevm_init() that takes 7 arguments in order to specify these 
 * sizes at runtime.
 */
#ifndef OSE_CONF_VM_INPUT_SIZE
#define OSE_CONF_VM_INPUT_SIZE 131072
#endif
#ifndef OSE_CONF_VM_STACK_SIZE
#define OSE_CONF_VM_STACK_SIZE 131072
#endif
#ifndef OSE_CONF_VM_ENV_SIZE
#define OSE_CONF_VM_ENV_SIZE 131072
#endif
#ifndef OSE_CONF_VM_CONTROL_SIZE
#define OSE_CONF_VM_CONTROL_SIZE 131072
#endif
#ifndef OSE_CONF_VM_DUMP_SIZE
#define OSE_CONF_VM_DUMP_SIZE 131072
#endif
#ifndef OSE_CONF_VM_OUTPUT_SIZE
#define OSE_CONF_VM_OUTPUT_SIZE 131072
#endif

/**
 * Uncomment these to provide support for different nonstandard OSC types
 */
/* define OSE_CONF_PROVIDE_TYPE_SYMBOL */
/* define OSE_CONF_PROVIDE_TYPE_DOUBLE */
/* define OSE_CONF_PROVIDE_TYPE_INT8 */
/* define OSE_CONF_PROVIDE_TYPE_UINT8 */
/* define OSE_CONF_PROVIDE_TYPE_UINT32 */
/* define OSE_CONF_PROVIDE_TYPE_INT64 */
/* define OSE_CONF_PROVIDE_TYPE_UINT64 */
/* define OSE_CONF_PROVIDE_TYPE_TYPETAG */
/* define OSE_CONF_PROVIDE_TYPE_TRUE */
/* define OSE_CONF_PROVIDE_TYPE_FALSE */
/* define OSE_CONF_PROVIDE_TYPE_NULL */
/* define OSE_CONF_PROVIDE_TYPE_INFINITUM */

/**
 * These can be safely defined when calling `make`. Debug is significantly
 * slower, but provides a number of features that can help track down problems.
 */
#ifdef OSE_CONF_DEBUG
#define OSE_DEBUG
#define OSE_SAFE_BUNDLE
#else
#define NDEBUG
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

/**********************************************************************
 * End configuration vars
 **********************************************************************/

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

#endif /* OSE_DEBUG */
#endif /* OSE_CONF_ADDRESS_ANONVAL */

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
	
#ifdef __cplusplus
}
#endif

#endif
