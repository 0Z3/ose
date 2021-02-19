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

#ifndef OSE_VM_H
#define OSE_VM_H

#include "ose_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/* number of 32-bit ints available in the cache message */
#define OSEVM_CACHE_SIZE 30
#define OSEVM_CACHE_MSG_SIZE OSE_CONTEXT_MESSAGE_OVERHEAD	\
	+ 4 + 4 + 32 + (OSEVM_CACHE_SIZE * 4)
#define OSEVM_CACHE_OFFSET_0 OSE_BUNDLE_HEADER_LEN	\
	+ OSE_CONTEXT_BUNDLE_OFFSET			\
	+ OSE_BUNDLE_HEADER_LEN				\
	+ 4 + 4 + 32 /* size, address, typetags */
#define OSEVM_CACHE_OFFSET_1 OSEVM_CACHE_OFFSET_0 + 4
#define OSEVM_CACHE_OFFSET_2 OSEVM_CACHE_OFFSET_1 + 4
#define OSEVM_CACHE_OFFSET_3 OSEVM_CACHE_OFFSET_2 + 4
#define OSEVM_CACHE_OFFSET_4 OSEVM_CACHE_OFFSET_3 + 4
#define OSEVM_CACHE_OFFSET_5 OSEVM_CACHE_OFFSET_4 + 4
#define OSEVM_CACHE_OFFSET_6 OSEVM_CACHE_OFFSET_5 + 4
#define OSEVM_CACHE_OFFSET_7 OSEVM_CACHE_OFFSET_6 + 4
#define OSEVM_CACHE_OFFSET_8 OSEVM_CACHE_OFFSET_7 + 4
#define OSEVM_CACHE_OFFSET_9 OSEVM_CACHE_OFFSET_8 + 4
#define OSEVM_CACHE_OFFSET_10 OSEVM_CACHE_OFFSET_9 + 4
#define OSEVM_CACHE_OFFSET_11 OSEVM_CACHE_OFFSET_10 + 4
#define OSEVM_CACHE_OFFSET_12 OSEVM_CACHE_OFFSET_11 + 4
#define OSEVM_CACHE_OFFSET_13 OSEVM_CACHE_OFFSET_12 + 4
#define OSEVM_CACHE_OFFSET_14 OSEVM_CACHE_OFFSET_13 + 4
#define OSEVM_CACHE_OFFSET_15 OSEVM_CACHE_OFFSET_14 + 4

#define OSEVM_CACHE_POINTER OSEVM_CACHE_OFFSET_0
#define OSEVM_CACHE_FLAGS OSEVM_CACHE_OFFSET_1
#define OSEVM_CACHE_OFFSET_INPUT OSEVM_CACHE_OFFSET_2
#define OSEVM_CACHE_OFFSET_STACK OSEVM_CACHE_OFFSET_3
#define OSEVM_CACHE_OFFSET_ENV OSEVM_CACHE_OFFSET_4
#define OSEVM_CACHE_OFFSET_CONTROL OSEVM_CACHE_OFFSET_5
#define OSEVM_CACHE_OFFSET_DUMP OSEVM_CACHE_OFFSET_6
#define OSEVM_CACHE_OFFSET_OUTPUT OSEVM_CACHE_OFFSET_7

#ifdef OSEVM_HAVE_SIZES

#define OSEVM_INPUT_CONTEXT_MESSAGE_OFFSET OSE_BUNDLE_HEADER_LEN	\
	+ OSEVM_CACHE_MSG_SIZE
#define OSEVM_INPUT_BUNDLE_OFFSET OSEVM_INPUT_CONTEXT_MESSAGE_OFFSET	\
	+ OSE_CONTEXT_BUNDLE_OFFSET

#define OSEVM_STACK_CONTEXT_MESSAGE_OFFSET OSEVM_INPUT_CONTEXT_MESSAGE_OFFSET \
	+ OSE_CONTEXT_MESSAGE_OVERHEAD + OSEVM_INPUT_SIZE
#define OSEVM_STACK_BUNDLE_OFFSET OSEVM_STACK_CONTEXT_MESSAGE_OFFSET	\
	+ OSE_CONTEXT_BUNDLE_OFFSET

#define OSEVM_ENV_CONTEXT_MESSAGE_OFFSET OSEVM_STACK_CONTEXT_MESSAGE_OFFSET \
	+ OSE_CONTEXT_MESSAGE_OVERHEAD + OSEVM_STACK_SIZE
#define OSEVM_ENV_BUNDLE_OFFSET OSEVM_ENV_CONTEXT_MESSAGE_OFFSET	\
	+ OSE_CONTEXT_BUNDLE_OFFSET

#define OSEVM_CONTROL_CONTEXT_MESSAGE_OFFSET OSEVM_ENV_CONTEXT_MESSAGE_OFFSET \
	+ OSE_CONTEXT_MESSAGE_OVERHEAD + OSEVM_ENV_SIZE
#define OSEVM_CONTROL_BUNDLE_OFFSET OSEVM_CONTROL_CONTEXT_MESSAGE_OFFSET	\
	+ OSE_CONTEXT_BUNDLE_OFFSET

#define OSEVM_DUMP_CONTEXT_MESSAGE_OFFSET OSEVM_CONTROL_CONTEXT_MESSAGE_OFFSET \
	+ OSE_CONTEXT_MESSAGE_OVERHEAD + OSEVM_CONTROL_SIZE
#define OSEVM_DUMP_BUNDLE_OFFSET OSEVM_DUMP_CONTEXT_MESSAGE_OFFSET	\
	+ OSE_CONTEXT_BUNDLE_OFFSET

#define OSEVM_OUTPUT_CONTEXT_MESSAGE_OFFSET OSEVM_DUMP_CONTEXT_MESSAGE_OFFSET \
	+ OSE_CONTEXT_MESSAGE_OVERHEAD + OSEVM_DUMP_SIZE
#define OSEVM_OUTPUT_BUNDLE_OFFSET OSEVM_OUTPUT_CONTEXT_MESSAGE_OFFSET	\
	+ OSE_CONTEXT_BUNDLE_OFFSET

#define OSEVM_INPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + OSEVM_INPUT_BUNDLE_OFFSET)
#define OSEVM_STACK(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + OSEVM_STACK_BUNDLE_OFFSET)
#define OSEVM_ENV(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					+ OSEVM_ENV_BUNDLE_OFFSET)
#define OSEVM_CONTROL(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					    + OSEVM_CONTROL_BUNDLE_OFFSET)
#define OSEVM_DUMP(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					 + OSEVM_DUMP_BUNDLE_OFFSET)
#define OSEVM_OUTPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					   + OSEVM_OUTPUT_BUNDLE_OFFSET)

ose_bundle osevm_init(ose_bundle bundle);

#else
	
#define OSEVM_INPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + ose_readInt32(osevm,	\
							  OSEVM_CACHE_OFFSET_INPUT))
#define OSEVM_STACK(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + ose_readInt32(osevm,	\
							  OSEVM_CACHE_OFFSET_STACK))
#define OSEVM_ENV(osevm) ose_makeBundle(ose_getBundlePtr(osevm)		\
					+ ose_readInt32(osevm,		\
							OSEVM_CACHE_OFFSET_ENV))
#define OSEVM_CONTROL(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					    + ose_readInt32(osevm,	\
							    OSEVM_CACHE_OFFSET_CONTROL))
#define OSEVM_DUMP(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					 + ose_readInt32(osevm,		\
							 OSEVM_CACHE_OFFSET_DUMP))
#define OSEVM_OUTPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					   + ose_readInt32(osevm,	\
							   OSEVM_CACHE_OFFSET_OUTPUT))

ose_bundle osevm_init(ose_bundle bundle,
		      int32_t input_size,
		      int32_t stack_size,
		      int32_t env_size,
		      int32_t control_size,
		      int32_t dump_size,
		      int32_t output_size);
#endif

#define OSEVM_FLAG_COMPILE 1
#define OSEVM_GET_FLAGS(osevm) ose_readInt32(osevm, OSEVM_CACHE_FLAGS)
#define OSEVM_SET_FLAGS(osevm, flags) \
	ose_writeInt32(osevm, OSEVM_CACHE_FLAGS, flags)

#define OSEVM_SET_FLAG_COMPILE(osevm) \
	OSEVM_SET_FLAGS(osevm, \
		       OSEVM_GET_FLAGS(osevm) | OSEVM_FLAG_COMPILE)
#define OSEVM_UNSET_FLAG_COMPILE(osevm) \
	OSEVM_SET_FLAGS(osevm, \
		       OSEVM_GET_FLAGS(osevm) & ~(0UL | OSEVM_FLAG_COMPILE))

#define OSEVM_GET_CACHE_VALUE(osevm, idx) \
	(ose_assert((idx) < OSEVM_CACHE_SIZE),				\
	 ose_readInt32(osevm, OSEVM_CACHE_OFFSET_0 + (4 * (idx))))
#define OSEVM_SET_CACHE_VALUE(osevm, idx, val) \
	(ose_assert((idx) < OSEVM_CACHE_SIZE),				\
	 ose_writeInt32(osevm, OSEVM_CACHE_OFFSET_0 + (4 * (idx)), val))

/**********************************************************************
 * Hooks
 **********************************************************************/
void osevm_assign(ose_bundle osevm, char *address);
#ifndef OSEVM_ASSIGN
#define OSEVM_ASSIGN osevm_assign
#else
extern void OSEVM_ASSIGN (ose_bundle osevm, char *address);
#endif

void osevm_lookup(ose_bundle osevm, char *address);
#ifndef OSEVM_LOOKUP
#define OSEVM_LOOKUP osevm_lookup
#else
extern void OSEVM_LOOKUP (ose_bundle osevm, char *address);
#endif

void osevm_funcall(ose_bundle osevm, char *address);
#ifndef OSEVM_FUNCALL
#define OSEVM_FUNCALL osevm_funcall
#else
extern void OSEVM_FUNCALL (ose_bundle osevm, char *address);
#endif

void osevm_quote(ose_bundle osevm, char *address);
#ifndef OSEVM_QUOTE
#define OSEVM_QUOTE osevm_quote
#else
extern void OSEVM_QUOTE (ose_bundle osevm, char *address);
#endif

void osevm_copyContextBundle(ose_bundle osevm, char *address);
#ifndef OSEVM_COPYCONTEXTBUNDLE
#define OSEVM_COPYCONTEXTBUNDLE osevm_copyContextBundle
#else
extern void OSEVM_COPYCONTEXTBUNDLE (ose_bundle osevm, char *address);
#endif

void osevm_appendToContextBundle(ose_bundle osevm, char *address);
#ifndef OSEVM_APPENDTOCONTEXTBUNDLE
#define OSEVM_APPENDTOCONTEXTBUNDLE osevm_appendToContextBundle
#else
extern void OSEVM_APPENDTOCONTEXTBUNDLE (ose_bundle osevm, char *address);
#endif

void osevm_replaceContextBundle(ose_bundle osevm, char *address);
#ifndef OSEVM_REPLACECONTEXTBUNDLE
#define OSEVM_REPLACECONTEXTBUNDLE osevm_replaceContextBundle
#else
extern void OSEVM_REPLACECONTEXTBUNDLE (ose_bundle osevm, char *address);
#endif

void osevm_moveElemToContextBundle(ose_bundle osevm, char *address);
#ifndef OSEVM_MOVEELEMTOCONTEXTBUNDLE
#define OSEVM_MOVEELEMTOCONTEXTBUNDLE osevm_moveElemToContextBundle
#else
extern void OSEVM_MOVEELEMTOCONTEXTBUNDLE (ose_bundle osevm, char *address);
#endif

void osevm_exec(ose_bundle osevm, char *address);
#ifndef OSEVM_EXEC
#define OSEVM_EXEC osevm_exec
#else
extern void OSEVM_EXEC (ose_bundle osevm, char *address);
#endif

void osevm_execInCurrentContext(ose_bundle osevm, char *address);
#ifndef OSEVM_EXECINCURRENTCONTEXT
#define OSEVM_EXECINCURRENTCONTEXT osevm_execInCurrentContext
#else
extern void OSEVM_EXECINCURRENTCONTEXT (ose_bundle osevm, char *address);
#endif

void osevm_apply(ose_bundle osevm, char *address);
#ifndef OSEVM_APPLY
#define OSEVM_APPLY osevm_apply
#else
extern void OSEVM_APPLY (ose_bundle osevm, char *address);
#endif

void osevm_return(ose_bundle osevm, char *address);
#ifndef OSEVM_RETURN
#define OSEVM_RETURN osevm_return
#else
extern void OSEVM_RETURN (ose_bundle osevm, char *address);
#endif

void osevm_defun(ose_bundle osevm, char *address);
#ifndef OSEVM_DEFUN
#define OSEVM_DEFUN osevm_defun
#else
extern void OSEVM_DEFUN (ose_bundle osevm, char *address);
#endif

void osevm_endDefun(ose_bundle osevm, char *address);
#ifndef OSEVM_ENDDEFUN
#define OSEVM_ENDDEFUN osevm_endDefun
#else
extern void OSEVM_ENDDEFUN (ose_bundle osevm, char *address);
#endif

void osevm_toInt32(ose_bundle osevm, char *address);
#ifndef OSEVM_TOINT32
#define OSEVM_TOINT32 osevm_toInt32
#else
extern void OSEVM_TOINT32 (ose_bundle osevm, char *address);
#endif

void osevm_toFloat(ose_bundle osevm, char *address);
#ifndef OSEVM_TOFLOAT
#define OSEVM_TOFLOAT osevm_toFloat
#else
extern void OSEVM_TOFLOAT (ose_bundle osevm, char *address);
#endif

void osevm_toString(ose_bundle osevm, char *address);
#ifndef OSEVM_TOSTRING
#define OSEVM_TOSTRING osevm_toString
#else
extern void OSEVM_TOSTRING (ose_bundle osevm, char *address);
#endif

void osevm_toBlob(ose_bundle osevm, char *address);
#ifndef OSEVM_TOBLOB
#define OSEVM_TOBLOB osevm_toBlob
#else
extern void OSEVM_TOBLOB (ose_bundle osevm, char *address);
#endif

void osevm_appendByte(ose_bundle osevm, char *address);
#ifndef OSEVM_APPENDBYTE
#define OSEVM_APPENDBYTE osevm_appendByte
#else
extern void OSEVM_APPENDBYTE (ose_bundle osevm, char *address);
#endif

void osevm_default(ose_bundle osevm, char *address);
#ifndef OSEVM_DEFAULT
#define OSEVM_DEFAULT osevm_default
#else
extern void OSEVM_DEFAULT (ose_bundle osevm, char *address);
#endif

void osevm_preInput(ose_bundle osevm);
#ifndef OSEVM_PREINPUT
#define OSEVM_PREINPUT osevm_preInput
#else
extern void OSEVM_PREINPUT (ose_bundle osevm);
#endif

void osevm_postInput(ose_bundle osevm);
#ifndef OSEVM_POSTINPUT
#define OSEVM_POSTINPUT osevm_postInput
#else
extern void OSEVM_POSTINPUT (ose_bundle osevm);
#endif

void osevm_popInputToControl(ose_bundle osevm);
#ifndef OSEVM_POPINPUTTOCONTROL
#define OSEVM_POPINPUTTOCONTROL osevm_popInputToControl
#else
extern void OSEVM_POPINPUTTOCONTROL (ose_bundle osevm);
#endif

void osevm_postControl(ose_bundle osevm);
#ifndef OSEVM_POSTCONTROL
#define OSEVM_POSTCONTROL osevm_postControl
#else
extern void OSEVM_POSTCONTROL (ose_bundle osevm);
#endif

#define osevm_isKnownAddress(str) 0
#ifndef OSEVM_ISKNOWNADDRESS
#define OSEVM_ISKNOWNADDRESS osevm_isKnownAddress
#else
extern int OSEVM_ISKNOWNADDRESS (const char * const address);
#endif

void osevm_inputMessages(ose_bundle osevm,
			 int32_t size, const char * const bundle);
void osevm_inputMessage(ose_bundle osevm,
			int32_t size, const char * const message);
void osevm_run(ose_bundle bundle);
char osevm_step(ose_bundle osevm);
#ifdef OSEVM_HAVE_SIZES
int32_t osevm_computeSizeReqs(int n, ...);
#else
int32_t osevm_computeSizeReqs(int32_t input_size,
			      int32_t stack_size,
			      int32_t env_size,
			      int32_t control_size,
			      int32_t dump_size,
			      int32_t output_size,
			      int n, ...);
#endif

#ifdef __cplusplus
}
#endif

#endif
