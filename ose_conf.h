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

/**
 * VM bundle sizes
 * 
 * These can be changed here or defined when invoking `make` if not building
 * a library to be linked against later, since these values are needed
 * in different header files. 
 * 
 * If these are commented out, the osevm will be built with a version
 * of #osevm_init() that takes 7 arguments in order to specify these 
 * sizes at runtime.
 */
/* #ifndef OSE_CONF_VM_INPUT_SIZE */
/* #define OSE_CONF_VM_INPUT_SIZE 8192 */
/* #endif */
/* #ifndef OSE_CONF_VM_STACK_SIZE */
/* #define OSE_CONF_VM_STACK_SIZE 8192 */
/* #endif */
/* #ifndef OSE_CONF_VM_ENV_SIZE */
/* #define OSE_CONF_VM_ENV_SIZE 8192 */
/* #endif */
/* #ifndef OSE_CONF_VM_CONTROL_SIZE */
/* #define OSE_CONF_VM_CONTROL_SIZE 8192 */
/* #endif */
/* #ifndef OSE_CONF_VM_DUMP_SIZE */
/* #define OSE_CONF_VM_DUMP_SIZE 8192 */
/* #endif */
/* #ifndef OSE_CONF_VM_OUTPUT_SIZE */
/* #define OSE_CONF_VM_OUTPUT_SIZE 8192 */
/* #endif */

/**
 * VM hooks
 *
 * Uncomment the three lines for whichever hooks you want to implement,
 * and change the name of the function from my_xxx to the name of your function.
 */
/* #ifndef OSEVM_ASSIGN */
/* #define OSEVM_ASSIGN my_assign */
/* #endif */

/* #ifndef OSEVM_LOOKUP */
/* #define OSEVM_LOOKUP my_lookup */
/* #endif */

/* #ifndef OSEVM_FUNCALL */
/* #define OSEVM_FUNCALL my_funcall */
/* #endif */

/* #ifndef OSEVM_QUOTE */
/* #define OSEVM_QUOTE my_quote */
/* #endif */

/* #ifndef OSEVM_COPYCONTEXTBUNDLE */
/* #define OSEVM_COPYCONTEXTBUNDLE my_copyContextBundle */
/* #endif */

/* #ifndef OSEVM_APPENDTOCONTEXTBUNDLE */
/* #define OSEVM_APPENDTOCONTEXTBUNDLE my_appendToContextBundle */
/* #endif */

/* #ifndef OSEVM_REPLACECONTEXTBUNDLE */
/* #define OSEVM_REPLACECONTEXTBUNDLE my_replaceContextBundle */
/* #endif */

/* #ifndef OSEVM_MOVEELEMTOCONTEXTBUNDLE */
/* #define OSEVM_MOVEELEMTOCONTEXTBUNDLE my_moveElemToContextBundle */
/* #endif */

/* #ifndef OSEVM_EXEC */
/* #define OSEVM_EXEC my_exec */
/* #endif */

/* #ifndef OSEVM_EXECINCURRENTCONTEXT */
/* #define OSEVM_EXECINCURRENTCONTEXT my_execInCurrentContext */
/* #endif */

/* #ifndef OSEVM_APPLY */
/* #define OSEVM_APPLY my_apply */
/* #endif */

/* #ifndef OSEVM_RETURN */
/* #define OSEVM_RETURN my_return */
/* #endif */

/* #ifndef OSEVM_DEFUN */
/* #define OSEVM_DEFUN my_defun */
/* #endif */

/* #ifndef OSEVM_ENDDEFUN */
/* #define OSEVM_ENDDEFUN my_endDefun */
/* #endif */

/* #ifndef OSEVM_TOINT32 */
/* #define OSEVM_TOINT32 my_toInt32 */
/* #endif */

/* #ifndef OSEVM_TOFLOAT */
/* #define OSEVM_TOFLOAT my_toFloat */
/* #endif */

/* #ifndef OSEVM_TOSTRING */
/* #define OSEVM_TOSTRING my_toString */
/* #endif */

/* #ifndef OSEVM_TOBLOB */
/* #define OSEVM_TOBLOB my_toBlob */
/* #endif */

/* #ifndef OSEVM_APPENDBYTE */
/* #define OSEVM_APPENDBYTE my_appendByte */
/* #endif */

/* #ifndef OSEVM_DEFAULT */
/* #define OSEVM_DEFAULT my_default */
/* #endif */

/* #ifndef OSEVM_PREINPUT */
/* #define OSEVM_PREINPUT my_preInput */
/* #endif */

/* #ifndef OSEVM_POSTINPUT */
/* #define OSEVM_POSTINPUT my_postInput */
/* #endif */

/* #ifndef OSEVM_POPINPUTTOCONTROL */
/* #define OSEVM_POPINPUTTOCONTROL my_popInputToControl */
/* #endif */

/* #ifndef OSEVM_POSTCONTROL */
/* #define OSEVM_POSTCONTROL my_postControl */
/* #endif */

/* #ifndef OSEVM_ISKNOWNADDRESS */
/* #define OSEVM_ISKNOWNADDRESS my_isKnownAddress */
/* #endif */

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
 * Debug is significantly slower, but provides a number of features 
 * that can help track down problems.
 */
/* #define OSE_CONF_DEBUG */

/* When not in debug mode, recoverable assertions are turned off. 
   In debug mode, the default behavior is for recoverable assertions to 
   behave like normal assertions and call abort(), however, by defining
   OSE_CONF_RASSERT_THROW, they can be made to longjmp back to a point
   before the assertion failed. 
*/
/* #define OSE_CONF_RASSERT_FAIL */
/* #define OSE_CONF_RASSERT_THROW */

/**
 * The address of anonymous values. When a value is pushed onto the stack
 * with a function like #ose_pushInt32(), it goes on as a message with a
 * default message, which can be set here. By default, the message is empty.
 * 
 * In order to define your own default, you must explicitly pad the address
 * with enough NULL bytes to make it a multiple of 4 (for example: "/x\0\0",
 * or "/foobar\0"). OSE_CONF_ADDRESS_ANONVAL_LEN refers to the number of 
 * non-NULL bytes, and OSE_CONF_ADDRESS_ANONVAL_SIZE is the full length
 * of the padded address (multiple of 4 bytes).
 */
/* #define OSE_CONF_ADDRESS_ANONVAL "/\0\0\0" */
/* #define OSE_CONF_ADDRESS_ANONVAL_LEN 1 */
/* #define OSE_CONF_ADDRESS_ANONVAL_SIZE 4 */


#ifdef __cplusplus
}
#endif

#endif
