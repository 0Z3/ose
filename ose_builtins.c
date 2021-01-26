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
		ose_assert(ose_isIntegerType(ose_peekType(vm_s)) == OSETT_TRUE); \
		int32_t i = ose_popInt32(vm_s);				\
		ose_##name(i) == OSETT_TRUE				\
		? ose_pushInt32(vm_s, 1)				\
		: ose_pushInt32(vm_s, 0);				\
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
OSE_BUILTIN_DEFN(join)
OSE_BUILTIN_DEFN(pop)
OSE_BUILTIN_DEFN(popAll)
OSE_BUILTIN_DEFN(popAllDrop)
OSE_BUILTIN_DEFN(popAllBundle)
OSE_BUILTIN_DEFN(popAllDropBundle)
OSE_BUILTIN_DEFN(push)
OSE_BUILTIN_DEFN(split)
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

	/* move input to dump */
	ose_copyBundle(vm_i, vm_d);
	ose_clear(vm_i);

	/* int32_t n = ose_getBundleElemCount(vm_s); */
	/* move bundle on the stack to input */
	/* ose_moveElem(vm_s, vm_i); */
	/* if(ose_peekType(vm_i) == OSETT_MESSAGE){ */
	/* 	if(ose_peekMessageArgType(vm_i) == OSETT_BLOB){ */
	/* 		// need a test here to make sure */
	/* 		// the blob contains a bundle member */
	/* 		if(1){ */
	/* 			ose_blobToElem(vm_i); */
	/* 			ose_popAllDrop(vm_i); */
	/* 		} */
	/* 	} */
	/* }else{ */
	/* 	ose_popAllDrop(vm_i); */
	/* } */
	int32_t n = 0;

	/* move the contents of the bundle on the stack to the input, */
	/* and unpack it in reverse order */
	char *sp = ose_getBundlePtr(vm_s);
	char *ip = ose_getBundlePtr(vm_i);
	int32_t stackoffset = OSE_BUNDLE_HEADER_LEN;
	int32_t stacksize = ose_readInt32(vm_s, -4);
	ose_assert(stackoffset < stacksize);
	int32_t s = ose_readInt32(vm_s, stackoffset);
	n++;
	int32_t onm1 = stackoffset;
	int32_t snm1 = s;
	while(stackoffset + s + 4 < stacksize){
		onm1 = stackoffset;
		snm1 = s;
		stackoffset += s + 4;
		s = ose_readInt32(vm_s, stackoffset);
		n++;	
	}

	int32_t o1, o2;
	if(!strncmp(sp + stackoffset + 4,
		    OSE_BUNDLE_ID,
		    OSE_BUNDLE_ID_LEN)){
		/* bundle */
		o1 = stackoffset + 4 + OSE_BUNDLE_HEADER_LEN;
	}else{
		/* message */
		int32_t to, ntt, lto, po, lpo;
		ose_getNthPayloadItem(vm_s, 1, stackoffset,
				      &to, &ntt, &lto, &po, &lpo);
		if(ose_readByte(vm_s, lto) == OSETT_BLOB){
			s = ose_readInt32(vm_s, lpo);
			o1 = lpo + 4 + OSE_BUNDLE_HEADER_LEN;
		}else{
			/* this can potentially enter compiled */
			/* native code */
			ose_assert(0 && "found a message item that is not a blob");
			return;
		}
	}
	ose_incSize(vm_i, s - OSE_BUNDLE_HEADER_LEN);
	o2 = ose_readInt32(vm_i, -4);
	int32_t end = o1 + s - OSE_BUNDLE_HEADER_LEN;;
	while(o1 < end){
		int32_t ss = ose_readInt32(vm_s, o1);
		o2 -= ss + 4;
		memcpy(ip + o2, sp + o1, ss + 4);
		o1 += ss + 4;
	}
	ose_dropAtOffset(vm_s, stackoffset);

	/* copy environment to dump */
	ose_copyBundle(vm_e, vm_d);
	if(n >= 2){
		ose_clear(vm_e);
		/*if(ose_peekType(vm_s) == OSETT_MESSAGE){ */
		if(strncmp(sp + onm1 + 4, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN)){
		        if(ose_peekMessageArgType(vm_s) == OSETT_BLOB){
				ose_blobToElem(vm_s);
				snm1 = ose_readInt32(vm_s, onm1);
			}else{
				ose_assert(0 && "second argument to exec, the environment, must be a blob or a bundle.");
			}
		}
		/* ose_replaceBundle(vm_s, vm_e); */
		ose_incSize(vm_e, snm1 - OSE_BUNDLE_HEADER_LEN);
		memcpy(ose_getBundlePtr(vm_e) + OSE_BUNDLE_HEADER_LEN,
		       sp + onm1 + 4 + OSE_BUNDLE_HEADER_LEN,
		       snm1 - OSE_BUNDLE_HEADER_LEN);
		ose_dropAtOffset(vm_s, onm1);
	}else{
		;
	}

	/* move stack to dump */
	ose_copyBundle(vm_s, vm_d);
	ose_clear(vm_s);

	/* move control to dump */
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
		if(strcmp(ose_peekAddress(vm_c), "/!/dotimes")){
			ose_pushString(vm_c, "/dotimes");
		}
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

	ose_rassert(ose_bundleHasAtLeastNElems(vm_s, 1) == OSETT_TRUE, 1);
	while(1){
		char elemtype = ose_peekType(vm_s);
		if(elemtype == OSETT_BUNDLE){
			/* move input to dump */
			ose_copyBundle(vm_i, vm_d);
			ose_clear(vm_i);

			/* move bundle on the stack to input */
			/* ose_moveElem(vm_s, vm_i); */
			/* if(ose_peekType(vm_i) == OSETT_MESSAGE){ */
			/* 	if(ose_peekMessageArgType(vm_i) == OSETT_BLOB){ */
			/* 		// need a test here to make sure */
			/* 		// the blob contains a bundle member */
			/* 		if(1){ */
			/* 			ose_blobToElem(vm_i); */
			/* 			ose_popAllDrop(vm_i); */
			/* 		} */
			/* 	} */
			/* }else{ */
			/* 	ose_popAllDrop(vm_i); */
			/* } */
			{
				/* move the contents of the bundle on the stack to the input, */
				/* and unpack it in reverse order */
				char *sp = ose_getBundlePtr(vm_s);
				char *ip = ose_getBundlePtr(vm_i);
				int32_t stackoffset = OSE_BUNDLE_HEADER_LEN;
				int32_t stacksize = ose_readInt32(vm_s, -4);
				ose_assert(stackoffset < stacksize);
				int32_t s = ose_readInt32(vm_s, stackoffset);
				/* int32_t onm1 = stackoffset; */
				/* int32_t snm1 = s; */
				while(stackoffset + s + 4 < stacksize){
					/* onm1 = stackoffset; */
					/* snm1 = s; */
					stackoffset += s + 4;
					s = ose_readInt32(vm_s, stackoffset);
				}

				int32_t o1, o2;
				/* if(!strncmp(sp + stackoffset + 4, */
				/* 	    OSE_BUNDLE_ID, */
				/* 	    OSE_BUNDLE_ID_LEN)){ */
				/* bundle */
				o1 = stackoffset + 4 + OSE_BUNDLE_HEADER_LEN;
				/* }else{ */
				/* 	// message */
				/* 	int32_t to, ntt, lto, po, lpo; */
				/* 	ose_getNthPayloadItem(vm_s, 1, stackoffset, */
				/* 			      &to, &ntt, &lto, &po, &lpo); */
				/* 	if(ose_readByte(vm_s, lto) == OSETT_BLOB){ */
				/* 		s = ose_readInt32(vm_s, lpo); */
				/* 		o1 = lpo + 4 + OSE_BUNDLE_HEADER_LEN; */
				/* 	}else{ */
				/* 		// this can potentially enter compiled */
				/* 		// native code */
				/* 	} */
				/* } */
				ose_incSize(vm_i, s - OSE_BUNDLE_HEADER_LEN);
				o2 = ose_readInt32(vm_i, -4);
				int32_t end = o1 + s - OSE_BUNDLE_HEADER_LEN;;
				while(o1 < end){
					int32_t ss = ose_readInt32(vm_s, o1);
					o2 -= ss + 4;
					memcpy(ip + o2, sp + o1, ss + 4);
					o1 += ss + 4;
				}
				ose_dropAtOffset(vm_s, stackoffset);
			}

			/* copy environment to dump */
			ose_copyBundle(vm_e, vm_d);

			/* move stack to dump */
			ose_pushBundle(vm_d);

			/* move control to dump */
			ose_drop(vm_c);
			ose_copyBundle(vm_c, vm_d);
			ose_clear(vm_c);
			break;
		}else if(elemtype == OSETT_MESSAGE){
			int32_t o = ose_getLastBundleElemOffset(vm_s);
			int32_t to, ntt, lto, po, lpo;
			ose_getNthPayloadItem(vm_s, 1, o,
					      &to, &ntt, &lto, &po, &lpo);
			/*char itemtype = ose_peekMessageArgType(vm_s); */
			char itemtype = ose_getBundlePtr(vm_s)[lto];
			const char * const p = ose_getBundlePtr(vm_s) + lpo;
			if(ose_isStringType(itemtype) == OSETT_TRUE){
				int32_t mo = ose_getFirstOffsetForMatch(vm_e, p);
				if(mo >= OSE_BUNDLE_HEADER_LEN){
					/*ose_pushString(vm_e, p); */
					ose_dropAtOffset(vm_s, o);
					/*ose_pickMatch(vm_e); */
					/*ose_drop(vm_e); */
					/*ose_moveElem(vm_e, vm_s); */
					ose_copyElemAtOffset(mo, vm_e, vm_s);
					continue;
				}
				/* if it wasn't present in env, lookup in symtab */
				else{
					ose_fn f = ose_symtab_lookup_fn(p);
					if(f){
						ose_dropAtOffset(vm_s, o);
						f(osevm);
						break;
					}else{
						break;
					}
				}
			}else if(itemtype == OSETT_BLOB){
				int32_t len = ose_ntohl(*((int32_t *)p));
				if(len > OSE_BUNDLE_HEADER_LEN
				   && !strncmp(OSE_BUNDLE_ID, p + 4, 7)){
					/* blob is a bundle */
					ose_blobToElem(vm_s);
					continue;
				}else{
					/* blob is not a bundle */
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

void ose_builtin_map(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_swap(vm_s);
	char t = ose_peekType(vm_s);
	if(t == OSETT_BUNDLE){
		ose_countItems(vm_s);
		int32_t n = ose_popInt32(vm_s);
		ose_popAll(vm_s);
		int i = 0, j = -1;
		ose_pushBundle(vm_s);
		// ENm1 ... E0 B B -
		for(i = 0; i < n; i++){
			// B1 => items
			// B0 => elems
			ose_rot(vm_s); // ENm1 ... B1 B0 E0 -
			ose_countItems(vm_s);
			int jj = ose_popInt32(vm_s);
			if(jj == 0){
				for(i = 0; i < n + 3; i++){
					ose_drop(vm_s);
				}
				return;
			}else if(j > 0 && jj != j){
				ose_rassert(0 && "arguments to map must be the same length", 1);
				break;
			}else{
				ose_pop(vm_s); // ENm1 ... B1 B0 E0 I0 -
				ose_notrot(vm_s); // ENm1 ... B1 I0 B0 E0 -
				ose_push(vm_s); // ENm1 ... B1 I0 B0 -
				ose_notrot(vm_s); // ENm1 ... B0 B1 I0 -
				ose_push(vm_s); // ENm1 ... B0 B1 -
				ose_swap(vm_s); // ENm1 ... B1 B0 -
			}
		}
		// B1 B0 -
		if(!strcmp(ose_peekAddress(vm_c), "/!")){
			ose_drop(vm_c);
			ose_pushMessage(vm_c, "/!/map", 6, 0);
		}
		ose_copyElem(vm_s, vm_c);
		ose_drop(vm_s);
		ose_swap(vm_s);
		ose_copyElem(vm_s, vm_c);
		ose_swap(vm_c);
		ose_pushMessage(vm_c, "/!/apply", 8, 0);
		ose_pushMessage(vm_c, "/!/map", 6, 0);
		ose_push(vm_s);
		ose_unpackDrop(vm_s);
	}else{
		ose_countItems(vm_s);
		if(ose_popInt32(vm_s) > 0){
			ose_swap(vm_s);
			ose_copyElem(vm_s, vm_c);
			ose_swap(vm_s);
			ose_pop(vm_s);
			ose_swap(vm_s);
			ose_copyElem(vm_s, vm_c);
			ose_drop(vm_s);
			ose_swap(vm_s);
			ose_pushMessage(vm_c, "/!/apply", 8, 0);
			ose_pushMessage(vm_c, "/!/map", 6, 0);
		}else{
			;
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

#define OSE_USE_OPTIMIZED_CODE
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
	/* restore control */
	ose_replaceBundle(vm_d, vm_c);

	/* restore stack */
	ose_bundleAll(vm_s);
	ose_moveElem(vm_d, vm_s);
	ose_unpackDrop(vm_s);
	ose_rollBottom(vm_s);
	ose_unpackDrop(vm_s);

	/* restore env */
	/*ose_copyBundle(vm_e, vm_s); */
	ose_replaceBundle(vm_d, vm_e);

	/* restore input */
	ose_replaceBundle(vm_d, vm_i);
#endif
}

void ose_builtin_version(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_pushString(vm_s, ose_version);
#ifdef OSE_DEBUG
	ose_pushString(vm_s, ose_debug);
#endif
	ose_pushString(vm_s, ose_date_compiled);
}
