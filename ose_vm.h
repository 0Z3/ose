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

#define OSE_VM_STATUS_SIZE 16

#ifdef OSE_VM_SIZE

#define OSE_VM_STACK_SIZE_ (OSE_VM_SIZE					\
			    - (4 + OSE_BUNDLE_HEADER_LEN		\
			       + OSE_CONTEXT_MESSAGE_OVERHEAD		\
			       + OSE_CONTEXT_MESSAGE_OVERHEAD + OSE_VM_STATUS_SIZE \
			       + OSE_CONTEXT_MESSAGE_OVERHEAD))		\
	/ 6
#define OSE_VM_STACK_SIZE OSE_VM_STACK_SIZE_ - (OSE_VM_STACK_SIZE_ % 4)

#define OSEVM_INPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + OSE_BUNDLE_HEADER_LEN	\
					  + OSE_CONTEXT_BUNDLE_OFFSET)
#define OSEVM_STACK(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + OSE_BUNDLE_HEADER_LEN	\
					  + OSE_VM_STACK_SIZE		\
					  + OSE_CONTEXT_BUNDLE_OFFSET)
#define OSEVM_ENV(osevm) ose_makeBundle(ose_getBundlePtr(osevm)		\
					+ OSE_BUNDLE_HEADER_LEN		\
					+ (OSE_VM_STACK_SIZE		\
					   + OSE_VM_STACK_SIZE)		\
					+ OSE_CONTEXT_BUNDLE_OFFSET)
#define OSEVM_CONTROL(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					    + OSE_BUNDLE_HEADER_LEN	\
					    + (OSE_VM_STACK_SIZE	\
					       + OSE_VM_STACK_SIZE	\
					       + OSE_VM_STACK_SIZE)	\
					    + OSE_CONTEXT_BUNDLE_OFFSET)
#define OSEVM_DUMP(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					 + OSE_BUNDLE_HEADER_LEN	\
					 + (OSE_VM_STACK_SIZE		\
					    + OSE_VM_STACK_SIZE		\
					    + OSE_VM_STACK_SIZE		\
					    + OSE_VM_STACK_SIZE)	\
					 + OSE_CONTEXT_BUNDLE_OFFSET)
#define OSEVM_OUTPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					   + OSE_BUNDLE_HEADER_LEN	\
					   + (OSE_VM_STACK_SIZE		\
					      + OSE_VM_STACK_SIZE	\
					      + OSE_VM_STACK_SIZE	\
					      + OSE_VM_STACK_SIZE	\
					      + OSE_VM_STACK_SIZE)	\
					   + OSE_CONTEXT_BUNDLE_OFFSET)

#else

#define OSEVM_CACHE_OFFSET			\
	OSE_BUNDLE_HEADER_LEN			\
	+ OSE_CONTEXT_BUNDLE_OFFSET		\
	+ OSE_BUNDLE_HEADER_LEN			\
	+ 4 + 4 + 8 // size, address, typetags
	
#define OSEVM_INPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + ose_readInt32(osevm,	\
							  OSEVM_CACHE_OFFSET))
#define OSEVM_STACK(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					  + ose_readInt32(osevm,	\
							  OSEVM_CACHE_OFFSET + 4))
#define OSEVM_ENV(osevm) ose_makeBundle(ose_getBundlePtr(osevm)		\
					+ ose_readInt32(osevm,		\
							OSEVM_CACHE_OFFSET + 8))
#define OSEVM_CONTROL(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					    + ose_readInt32(osevm,	\
							    OSEVM_CACHE_OFFSET + 12))
#define OSEVM_DUMP(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					 + ose_readInt32(osevm,		\
							 OSEVM_CACHE_OFFSET + 16))
#define OSEVM_OUTPUT(osevm) ose_makeBundle(ose_getBundlePtr(osevm)	\
					   + ose_readInt32(osevm,	\
							   OSEVM_CACHE_OFFSET + 20))

#endif

void osevm_assign(ose_bundle osevm);
#ifndef OSEVM_ASSIGN
#define OSEVM_ASSIGN osevm_assign
#else
extern void OSEVM_ASSIGN (ose_bundle osevm);
#endif

void osevm_lookup(ose_bundle osevm);
#ifndef OSEVM_LOOKUP
#define OSEVM_LOOKUP osevm_lookup
#else
extern void OSEVM_LOOKUP (ose_bundle osevm);
#endif

void osevm_toInt32(ose_bundle osevm);
#ifndef OSEVM_TOINT32
#define OSEVM_TOINT32 osevm_toInt32
#else
extern void OSEVM_TOINT32 (ose_bundle osevm);
#endif

void osevm_toFloat(ose_bundle osevm);
#ifndef OSEVM_TOFLOAT
#define OSEVM_TOFLOAT osevm_toFloat
#else
extern void OSEVM_TOFLOAT (ose_bundle osevm);
#endif

void osevm_toString(ose_bundle osevm);
#ifndef OSEVM_TOSTRING
#define OSEVM_TOSTRING osevm_toString
#else
extern void OSEVM_TOSTRING (ose_bundle osevm);
#endif

void osevm_toBlob(ose_bundle osevm);
#ifndef OSEVM_TOBLOB
#define OSEVM_TOBLOB osevm_toBlob
#else
extern void OSEVM_TOBLOB (ose_bundle osevm);
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

void osevm_preControl(ose_bundle osevm);
#ifndef OSEVM_PRECONTROL
#define OSEVM_PRECONTROL osevm_preControl
#else
extern void OSEVM_PRECONTROL (ose_bundle osevm);
#endif

void osevm_postControl(ose_bundle osevm);
#ifndef OSEVM_POSTCONTROL
#define OSEVM_POSTCONTROL osevm_postControl
#else
extern void OSEVM_POSTCONTROL (ose_bundle osevm);
#endif

ose_bundle osevm_init(ose_bundle bundle);
void osevm_run(ose_bundle bundle);
char osevm_step(ose_bundle osevm);

#ifdef __cplusplus
}
#endif

#endif
