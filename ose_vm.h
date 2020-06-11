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

void ose_assignStackToEnv(ose_bundle osevm);
#ifndef OSEVM_ASSIGN
#define OSEVM_ASSIGN ose_assignStackToEnv
#else
extern void OSEVM_ASSIGN (ose_bundle osevm);
#endif

void ose_lookupStackItemInEnv(ose_bundle osevm);
#ifndef OSEVM_LOOKUP
#define OSEVM_LOOKUP ose_lookupStackItemInEnv
#else
extern void OSEVM_LOOKUP (ose_bundle osevm);
#endif

void ose_preInput(ose_bundle osevm);
#ifndef OSEVM_PREINPUT
#define OSEVM_PREINPUT ose_preInput
#else
extern void OSEVM_PREINPUT (ose_bundle osevm);
#endif

void ose_postInput(ose_bundle osevm);
#ifndef OSEVM_POSTINPUT
#define OSEVM_POSTINPUT ose_postInput
#else
extern void OSEVM_POSTINPUT (ose_bundle osevm);
#endif

void ose_preControl(ose_bundle osevm);
#ifndef OSEVM_PRECONTROL
#define OSEVM_PRECONTROL ose_preControl
#else
extern void OSEVM_PRECONTROL (ose_bundle osevm);
#endif

void ose_postControl(ose_bundle osevm);
#ifndef OSEVM_POSTCONTROL
#define OSEVM_POSTCONTROL ose_postControl
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
