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
 * @file ose_assert.h
 * @brief Provides an assertion macro.
 */

#ifndef OSE_ASSERT_H
#define OSE_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

/** @brief Assertion macro. Can be turned off by defining NDEBUG 
 */
/* #define ose_assert(t) assert(t) */
#ifdef NDEBUG
#define ose_assert(t, ...) do {} while(0)
#define ose_assertf(t, fmt, ...) do {} while(0)
#else
/* TODO: make ose_assert take a mandatory string arg. */
/* This is only here until we transition to the new sig. */
#define ose_assert(t, ...)						\
	!(t)								\
	? ((void)fprintf(stderr,					\
		   "Assertion failed: %s, function %s, file %s, line %d.\n", \
		   #t, __func__, __FILE__, __LINE__),			\
	   fprintf(stderr,						\
		   "%s\n",						\
		   #__VA_ARGS__),						\
	   abort())							\
	: (void)0

#define ose_assertf(t, fmt, ...)					\
	!(t)								\
	? ((void)fprintf(stderr,					\
		   "Assertion failed: %s, function %s, file %s, line %d.\n", \
		   #t, __func__, __FILE__, __LINE__),			\
	   fprintf(stderr,						\
		   fmt,							\
		   ##__VA_ARGS__),					\
	   abort())							\
	: (void)0
#endif

/* to be implemented. this should put a value in the status message
   of the main context bundle */
#define ose_setErrno(e) do{}while(0)
#define ose_debug(b) 

#if defined(OSE_RASSERT_FAIL)

#define ose_rassert(t, e) ose_assert(t)
#define ose_try do {} while(0);
#define ose_catch(x) while(0)
#define ose_finally do {} while(0);
#define ose_end_try do {} while(0);

#elif defined(OSE_RASSERT_THROW)

#define ose_rassert(t, e)				\
	if(!(t))					\
	{						\
		extern int ose_setjmp_called;		\
		extern jmp_buf ose_jmp_buf;		\
		ose_setErrno(e);			\
		if(ose_setjmp_called){			\
			longjmp(ose_jmp_buf, e);	\
		}					\
	}
#define ose_try do{				\
	extern jmp_buf ose_jmp_buf;		\
	extern int ose_setjmp_called;		\
	ose_setjmp_called = 1;			\
	switch(setjmp(ose_jmp_buf)){		\
	case 0: while(1){
#define ose_catch(x)				\
	break;					\
	case x:
#define ose_finally				\
	break; }				\
	default: {
#define ose_end_try break; } } ose_setjmp_called = 0; } while(0);
	
#endif

#ifdef __cplusplus
}
#endif

#endif
