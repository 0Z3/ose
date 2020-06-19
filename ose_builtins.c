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

#include <stdio.h>
#include <string.h>
#include "ose_conf.h"
#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"

#define OSE_BUILTIN_DEFN(name)				\
	void ose_builtin_##name(ose_bundle bundle)	\
	{						\
		ose_##name(OSEVM_STACK(bundle));        \
        }

#define OSE_BUILTIN_DEFPRED(name)				\
	void ose_builtin_##name(ose_bundle bundle)		\
	{							\
		ose_bundle vm_s = OSEVM_STACK(bundle);		\
		char t = ose_peekType(vm_s);			\
		ose_assert(ose_isIntegerType(t) == OSETT_TRUE);	\
		int32_t i = ose_popInt32(vm_s);			\
		ose_##name(i) == OSETT_TRUE			\
			? ose_pushInt32(vm_s, 1)		\
			: ose_pushInt32(vm_s, 0);		\
	}

OSE_BUILTIN_DEFN(2drop)
OSE_BUILTIN_DEFN(2dup)
OSE_BUILTIN_DEFN(2over)
OSE_BUILTIN_DEFN(2swap)
OSE_BUILTIN_DEFN(drop)
OSE_BUILTIN_DEFN(dup)
OSE_BUILTIN_DEFN(nip)
OSE_BUILTIN_DEFN(notrot)
OSE_BUILTIN_DEFN(over)
OSE_BUILTIN_DEFN(pick)
OSE_BUILTIN_DEFN(pickBottom)
OSE_BUILTIN_DEFN(pickMatch)
OSE_BUILTIN_DEFN(roll)
OSE_BUILTIN_DEFN(rollBottom)
OSE_BUILTIN_DEFN(rollMatch)
OSE_BUILTIN_DEFN(rot)
OSE_BUILTIN_DEFN(swap)
OSE_BUILTIN_DEFN(tuck)

OSE_BUILTIN_DEFN(bundleAll)
OSE_BUILTIN_DEFN(bundleFromBottom)
OSE_BUILTIN_DEFN(bundleFromTop)
OSE_BUILTIN_DEFN(clear)
OSE_BUILTIN_DEFN(clearPayload)
OSE_BUILTIN_DEFN(pop)
OSE_BUILTIN_DEFN(popAll)
OSE_BUILTIN_DEFN(popAllDrop)
OSE_BUILTIN_DEFN(popAllBundle)
OSE_BUILTIN_DEFN(popAllDropBundle)
OSE_BUILTIN_DEFN(push)
OSE_BUILTIN_DEFN(unpack)
OSE_BUILTIN_DEFN(unpackDrop)
OSE_BUILTIN_DEFN(unpackBundle)
OSE_BUILTIN_DEFN(unpackDropBundle)

OSE_BUILTIN_DEFN(countElems)
OSE_BUILTIN_DEFN(countItems)
OSE_BUILTIN_DEFN(lengthAddress)
OSE_BUILTIN_DEFN(lengthTT)
OSE_BUILTIN_DEFN(lengthItem)
OSE_BUILTIN_DEFN(lengthsItems)
OSE_BUILTIN_DEFN(sizeAddress)
OSE_BUILTIN_DEFN(sizeElem)
OSE_BUILTIN_DEFN(sizeItem)
OSE_BUILTIN_DEFN(sizePayload)
OSE_BUILTIN_DEFN(sizesElems)
OSE_BUILTIN_DEFN(sizesItems)
OSE_BUILTIN_DEFN(sizeTT)

OSE_BUILTIN_DEFN(blobToElem)
OSE_BUILTIN_DEFN(blobToType)
OSE_BUILTIN_DEFN(concatenateBlobs)
OSE_BUILTIN_DEFN(concatenateStrings)
OSE_BUILTIN_DEFN(copyAddressToString)
OSE_BUILTIN_DEFN(copyPayloadToBlob)
OSE_BUILTIN_DEFN(swapStringToAddress)
OSE_BUILTIN_DEFN(copyTTToBlob)
OSE_BUILTIN_DEFN(decatenateBlob)
OSE_BUILTIN_DEFN(decatenateString)
OSE_BUILTIN_DEFN(elemToBlob)
OSE_BUILTIN_DEFN(itemToBlob)
OSE_BUILTIN_DEFN(joinBlobs)
OSE_BUILTIN_DEFN(joinStrings)
OSE_BUILTIN_DEFN(moveStringToAddress)
OSE_BUILTIN_DEFN(splitBlobFromEnd)
OSE_BUILTIN_DEFN(splitBlobFromStart)
OSE_BUILTIN_DEFN(splitStringFromEnd)
OSE_BUILTIN_DEFN(splitStringFromStart)
OSE_BUILTIN_DEFN(swap4Bytes)
OSE_BUILTIN_DEFN(swap8Bytes)
OSE_BUILTIN_DEFN(swapNBytes)
OSE_BUILTIN_DEFN(trimBlob)
OSE_BUILTIN_DEFN(trimString)
OSE_BUILTIN_DEFN(match)
OSE_BUILTIN_DEFN(pmatch)

OSE_BUILTIN_DEFN(makeBlob)
OSE_BUILTIN_DEFN(pushBundle)

OSE_BUILTIN_DEFN(add)
OSE_BUILTIN_DEFN(sub)
OSE_BUILTIN_DEFN(mul)
OSE_BUILTIN_DEFN(div)
OSE_BUILTIN_DEFN(mod)
OSE_BUILTIN_DEFN(pow)
OSE_BUILTIN_DEFN(neg)
OSE_BUILTIN_DEFN(eql)
OSE_BUILTIN_DEFN(lte)
OSE_BUILTIN_DEFN(lt)
OSE_BUILTIN_DEFN(and)
OSE_BUILTIN_DEFN(or)

OSE_BUILTIN_DEFPRED(isAddressChar);
OSE_BUILTIN_DEFPRED(isKnownTypetag);
OSE_BUILTIN_DEFPRED(isStringType);
OSE_BUILTIN_DEFPRED(isIntegerType);
OSE_BUILTIN_DEFPRED(isFloatType);
OSE_BUILTIN_DEFPRED(isNumericType);
OSE_BUILTIN_DEFPRED(isUnitType);
OSE_BUILTIN_DEFPRED(isBoolType);

void ose_builtin_eval(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_bundle vm_d = OSEVM_DUMP(osevm);
        ose_bundle vm_o = OSEVM_OUTPUT(osevm);

	// move output to dump
	// ose_bundleAll(vm_o);
	// ose_moveBundleElemToDest(vm_o, "/d");
			
	// move input to dump
	ose_bundleAll(vm_i);
	ose_moveBundleElemToDest(vm_i, vm_d);
			
	// move bundle on the stack to input
	ose_moveBundleElemToDest(vm_s, vm_i);
	if(ose_peekType(vm_i) == OSETT_MESSAGE){
		if(ose_peekMessageArgType(vm_i) == OSETT_BLOB){
			// need a test here to make sure
			// the blob contains a bundle member
			if(1){
				ose_blobToElem(vm_i);
				ose_popAllDrop(vm_i);
			}
		}
	}else{
		ose_popAllDrop(vm_i);
	}

	// move stack to dump
	ose_bundleAll(vm_s);
	ose_moveBundleElemToDest(vm_s, vm_d);

	// move environment to dump
	// ose_bundleAll(vm_e);
	// ose_moveBundleElemToDest(vm_e, vm_d);

	// move control to dump
	// ose_bundleAll(vm_c);
	// ose_moveBundleElemToDest(vm_c, vm_d);
	ose_pushBundle(vm_d);

	// move input to top of environment
	// ose_bundleAll(vm_i);
	// ose_copyBundleElemToDest(vm_i, "/e");
	// ose_unpackDrop(vm_i);
	// ose_popAllDrop(vm_e);
}

void ose_builtin_if(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	int32_t t = ose_popInt32(vm_s);
	if(t){
		// nothing to do
	}else{
		while(1){
			char *address = ose_peekAddress(vm_i);
			if(strlen(address) == 0
			   && ose_peekMessageArgType(vm_i) == OSETT_STRING){
				address = ose_peekString(vm_i);
			}
			if(!strcmp(address, "/!/else")){
				ose_drop(vm_i);
				break;
			}
			ose_drop(vm_i);
		}
	}
}

void ose_builtin_else(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	while(1){
		char *address = ose_peekAddress(vm_i);
		if(strlen(address) == 0
		   && ose_peekMessageArgType(vm_i) == OSETT_STRING){
			address = ose_peekString(vm_i);
		}
		if(!strcmp(address, "/!/end/if")){
			ose_drop(vm_i);
			break;
		}
		ose_drop(vm_i);
	}
}

void ose_builtin_end_if(ose_bundle osevm)
{}

void ose_builtin_dotimes(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_d = OSEVM_DUMP(osevm);
	int32_t n = ose_popInt32(vm_s);
	if(n > 0){
		ose_pushBundle(vm_i);
		ose_pushInt32(vm_i, n - 1);
		ose_push(vm_i);
		ose_pushMessage(vm_i, "/!/dotimes", strlen("/!/dotimes"), 0);
		ose_push(vm_i);
		ose_swap(vm_i);
		ose_push(vm_i);
		ose_swap(vm_i);
		while(1){
			char *address = ose_peekAddress(vm_i);
			if(strlen(address) == 0
			   && ose_peekMessageArgType(vm_i) == OSETT_STRING){
				address = ose_peekString(vm_i);
			}
			if(!strcmp(address, "/!/end/dotimes")){
				ose_push(vm_i);
				ose_copyBundleElemToDest(vm_i, vm_d);
				ose_popAllDrop(vm_i);
				ose_drop(vm_i);
				ose_drop(vm_i);
				break;
			}else{
				ose_push(vm_i);
				ose_swap(vm_i);
			}
		}
	}else{
		while(1){
			char *address = ose_peekAddress(vm_i);
			if(strlen(address) == 0
			   && ose_peekMessageArgType(vm_i) == OSETT_STRING){
				address = ose_peekString(vm_i);
			}
			if(!strcmp(address, "/!/end/dotimes")){
				ose_drop(vm_i);
				break;
			}else{
				ose_drop(vm_i);
			}
		}
	}
}

void ose_builtin_end_dotimes(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_d = OSEVM_DUMP(osevm);
	//ose_drop(vm_i);
	ose_moveBundleElemToDest(vm_d, vm_i);
	ose_popAllDrop(vm_i);
}

void ose_builtin_moveElemToDest(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_push(vm_s);
	char *a = ose_peekString(vm_s);
	ose_moveBundleElemToDestAddr(vm_s, a);
	ose_pop(dest);
	ose_drop(dest);
}

void ose_builtin_copyElemToDest(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_push(vm_s);
	char *a = ose_peekString(vm_s);
	ose_copyBundleElemToDestAddr(vm_s, a);
	ose_pop(dest);
	ose_drop(dest);
	ose_pop(vm_s);
	ose_drop(vm_s);
}

void ose_builtin_replaceElemInDest(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_push(vm_s);
	char *a = ose_peekString(vm_s);
	ose_replaceBundleElemInDestAddr(vm_s, a);
	ose_pop(dest);
	ose_drop(dest);
	ose_pop(vm_s);
	ose_drop(vm_s);
}

void ose_builtin_quote(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	while(strcmp(ose_peekAddress(vm_i), "/!/end/quote")){
		ose_moveBundleElemToDest(vm_i, vm_s);
	}
	ose_drop(vm_i);
}

void ose_builtin_end_quote(ose_bundle osevm)
{}

void ose_builtin_clearEnv(ose_bundle osevm)
{
	ose_clear(OSEVM_ENV(osevm));
}

void ose_builtin_copyEnv(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	int32_t es = ose_readInt32(vm_e, -4);
	int32_t ss = ose_readInt32(vm_s, -4);
	ose_addToSize(vm_s, es + 4);
	memcpy(ose_getBundlePtr(vm_s) + ss,
	       ose_getBundlePtr(vm_e) - 4,
	       es + 4);
}
