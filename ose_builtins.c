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
#include "ose_symtab.h"

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
OSE_BUILTIN_DEFN(decatenateBlobFromEnd)
OSE_BUILTIN_DEFN(decatenateBlobFromStart)
OSE_BUILTIN_DEFN(decatenateStringFromEnd)
OSE_BUILTIN_DEFN(decatenateStringFromStart)
OSE_BUILTIN_DEFN(elemToBlob)
OSE_BUILTIN_DEFN(itemToBlob)
OSE_BUILTIN_DEFN(joinStrings)
OSE_BUILTIN_DEFN(moveStringToAddress)
OSE_BUILTIN_DEFN(splitStringFromEnd)
OSE_BUILTIN_DEFN(splitStringFromStart)
OSE_BUILTIN_DEFN(swap4Bytes)
OSE_BUILTIN_DEFN(swap8Bytes)
OSE_BUILTIN_DEFN(swapNBytes)
OSE_BUILTIN_DEFN(trimStringEnd)
OSE_BUILTIN_DEFN(trimStringStart)
OSE_BUILTIN_DEFN(match)
OSE_BUILTIN_DEFN(pmatch)
OSE_BUILTIN_DEFN(replace)
OSE_BUILTIN_DEFN(assign)
OSE_BUILTIN_DEFN(lookup)
	

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
OSE_BUILTIN_DEFN(neq)
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

void ose_builtin_exec(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_bundle vm_d = OSEVM_DUMP(osevm);
        ose_bundle vm_o = OSEVM_OUTPUT(osevm);

	// move input to dump
	ose_copyBundle(vm_i, vm_d);
	ose_clear(vm_i);

	int32_t n = ose_getBundleElemCount(vm_s);
	// if(1){//n >= 2){
	// 	if(n >= 2){
	// 		ose_pushString(vm_i, "/!/copy/bundle");
	// 		ose_pushString(vm_i, "/s//ee");
	// 		ose_pushString(vm_i, "/s//ss");
	// 	}else{
	// 		ose_pushString(vm_i, "/!/copy/bundle");
	// 		ose_pushString(vm_i, "/s//ee");
	// 		ose_pushString(vm_i, "/s//ss");
	// 	}
	// }

	// move bundle on the stack to input
	ose_moveElem(vm_s, vm_i);
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

	// copy environment to dump
	ose_copyBundle(vm_e, vm_d);
	if(n >= 2){
		ose_clear(vm_e);
		if(ose_peekType(vm_s) == OSETT_MESSAGE){
			if(ose_peekMessageArgType(vm_s) == OSETT_BLOB){
				ose_blobToElem(vm_s);
			}else{
				ose_assert(0 && "second argument to exec, the environment, must be a blob or a bundle.");
			}
		}
		ose_replaceBundle(vm_s, vm_e);
	}else{
		;
	}

	// move stack to dump
	ose_copyBundle(vm_s, vm_d);
	ose_clear(vm_s);

	// move control to dump
	ose_drop(vm_c);
	ose_copyBundle(vm_c, vm_d);
	ose_clear(vm_c);
}

void ose_builtin_if(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_pushInt32(vm_s, 0);
	ose_neq(vm_s);
	ose_roll(vm_s);
	ose_drop(vm_s);
	ose_copyBundle(vm_e, vm_s);
	ose_swap(vm_s);
	ose_pushMessage(vm_c, "/!/exec", 7, 0);
	ose_swap(vm_c);
}

void ose_builtin_dotimes(ose_bundle osevm)
{
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	int32_t n = ose_popInt32(vm_s);
	if(n > 0){
		ose_pushInt32(vm_c, n - 1);
		ose_copyElem(vm_s, vm_c);
		ose_pushMessage(vm_c, "/!/exec", 7, 0);
		ose_pushMessage(vm_c, "/!/dotimes", 10, 0);
	}else{
		ose_drop(vm_s);
	}
}

void ose_builtin_copyBundle(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_constbundle src = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_copyBundle(src, dest);
}

void ose_builtin_appendBundle(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle src = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_appendBundle(src, dest);
}

void ose_builtin_replaceBundle(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle src = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_replaceBundle(src, dest);
}

void ose_builtin_moveElem(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle src = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_moveElem(src, dest);
}

void ose_builtin_copyElem(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_constbundle src = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_rassert(ose_peekType(vm_s) == OSETT_MESSAGE, 1);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE, 1);
	ose_bundle dest = ose_enter(osevm, ose_peekString(vm_s));
	ose_drop(vm_s);

	ose_copyElem(src, dest);
}

void ose_builtin_apply(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_bundle vm_d = OSEVM_DUMP(osevm);
        ose_bundle vm_o = OSEVM_OUTPUT(osevm);
	ose_rassert(ose_bundleHasAtLeastNElems(vm_s, 1) == OSETT_TRUE, 1);
	while(1){
		char elemtype = ose_peekType(vm_s);
		if(elemtype == OSETT_BUNDLE){
			// move input to dump
			ose_bundleAll(vm_i);
			ose_moveElem(vm_i, vm_d);

			// move bundle on the stack to input
			ose_moveElem(vm_s, vm_i);
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

			// copy environment to dump
			ose_bundleAll(vm_e);
			ose_copyElem(vm_e, vm_d);
			ose_unpackDrop(vm_e);

			// move stack to dump
			// ose_bundleAll(vm_s);
			// ose_moveElem(vm_s, vm_d);
			ose_pushBundle(vm_d);

			// move control to dump
			ose_drop(vm_c);
			ose_bundleAll(vm_c);
			ose_moveElem(vm_c, vm_d);
			//ose_pushBundle(vm_d);		
			break;
		}else if(elemtype == OSETT_MESSAGE){
			char itemtype = ose_peekMessageArgType(vm_s);
			if(ose_isStringType(itemtype) == OSETT_TRUE){
				const char * const s = ose_peekString(vm_s);
				// lookup in env
				// fix this--it does two lookups with matches
				// this should call the lookup function
				int32_t o = ose_getFirstOffsetForMatch(vm_e, s);
				if(o >= OSE_BUNDLE_HEADER_LEN){
					ose_pushString(vm_e, s);
					ose_drop(vm_s);
					ose_pickMatch(vm_e);
					ose_drop(vm_e);
					ose_moveElem(vm_e, vm_s);
					continue;
				}
				// if it wasn't present in env, lookup in symtab
				else{
					ose_fn f = ose_symtab_lookup_fn(s);
					if(f){
						ose_drop(vm_s);
						f(osevm);
						break;
					}else{
						break;
					}
				}
			}else if(itemtype == OSETT_BLOB){
				const char * const p = ose_peekBlob(vm_s);
				int32_t len = ose_ntohl(*((int32_t *)p));
				if(len > OSE_BUNDLE_HEADER_LEN
				   && !strncmp(OSE_BUNDLE_ID, p + 4, 7)){
					// blob is a bundle
					ose_blobToElem(vm_s);
					continue;
				}else{
					// blob is not a bundle
					int32_t o = p - ose_getBundlePtr(vm_s);
					ose_alignCFn(vm_s, o + 4);
					ose_fn f = ose_readCFn(vm_s, o + 4);
					if(f){
						ose_drop(vm_s);
						f(osevm);
						break;
					}else{
						break;
					}
				}
			}else{
				ose_assert(0 && "found an unexpected type during application");
				break;
			}
		}else{
			ose_assert(0 && "encountered unknown element type!");
			break;
		}
	}
}

void ose_builtin_return(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);

#ifdef OSE_USE_OPTIMIZED_CODE
	char *dp = ose_getBundlePtr(vm_d);
	char *cp = ose_getBundlePtr(vm_c);
	char *sp = ose_getBundlePtr(vm_s);
	char *ep = ose_getBundlePtr(vm_e);
	char *ip = ose_getBundlePtr(vm_i);

	int32_t onm3, onm2, onm1, on, snm3, snm2, snm1, sn;
	extern void be4(ose_bundle bundle,
			int32_t *onm3,
			int32_t *snm3,
			int32_t *onm2,
			int32_t *snm2,
			int32_t *onm1,
			int32_t *snm1,
			int32_t *on,
			int32_t *sn);
	be4(vm_d, &onm3, &snm3, &onm2, &snm2, &onm1, &snm1, &on, &sn);
	
	ose_clear(vm_c);
	ose_incSize(vm_c, sn - OSE_BUNDLE_HEADER_LEN);
	memcpy(cp + OSE_BUNDLE_HEADER_LEN,
	       dp + on + 4 + OSE_BUNDLE_HEADER_LEN,
	       sn - OSE_BUNDLE_HEADER_LEN);

	ose_incSize(vm_s, snm1 - OSE_BUNDLE_HEADER_LEN);
	memmove(sp + OSE_BUNDLE_HEADER_LEN + (snm1 - OSE_BUNDLE_HEADER_LEN),
		sp + OSE_BUNDLE_HEADER_LEN,
		ose_readInt32(vm_s, -4) - OSE_BUNDLE_HEADER_LEN);
	memcpy(dp + onm1 + 4 + OSE_BUNDLE_HEADER_LEN,
	       sp + OSE_BUNDLE_HEADER_LEN,
	       snm1 - OSE_BUNDLE_HEADER_LEN);

	ose_clear(vm_e);
	ose_incSize(vm_e, snm2 - OSE_BUNDLE_HEADER_LEN);
	memcpy(ep + OSE_BUNDLE_HEADER_LEN,
	       dp + onm2 + 4 + OSE_BUNDLE_HEADER_LEN,
	       snm2 - OSE_BUNDLE_HEADER_LEN);

	ose_clear(vm_i);
	ose_incSize(vm_i, snm3 - OSE_BUNDLE_HEADER_LEN);
	memcpy(ip + OSE_BUNDLE_HEADER_LEN,
	       dp + onm3 + 4 + OSE_BUNDLE_HEADER_LEN,
	       snm3 - OSE_BUNDLE_HEADER_LEN);

	int32_t s = snm3 + snm2 + snm1 + sn + 16;
	memset(dp + onm3, 0, s);
	ose_decSize(vm_d, s);
#else
	// restore control
	ose_replaceBundle(vm_d, vm_c);

	// restore stack
	ose_bundleAll(vm_s);
	ose_moveElem(vm_d, vm_s);
	ose_unpackDrop(vm_s);
	ose_rollBottom(vm_s);
	ose_unpackDrop(vm_s);

	// restore env
	//ose_copyBundle(vm_e, vm_s);
	ose_replaceBundle(vm_d, vm_e);

	// restore input
	ose_replaceBundle(vm_d, vm_i);
#endif
}
