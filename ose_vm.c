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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#include "ose_conf.h"
#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_symtab.h"
#include "ose_vm.h"

static void popControlToStack(ose_bundle vm_c, ose_bundle vm_s)
{
	ose_copyBundleElemToDest(vm_c, vm_s);
}

static void popInputToControl(ose_bundle vm_i, ose_bundle vm_c)
{
	ose_moveBundleElemToDest(vm_i, vm_c);
}

static void popStackToEnv(ose_bundle vm_s,
			  ose_bundle vm_e)
{
	//ose_bundleAll(vm_e);
	ose_replaceBundleElemInDest(vm_s, vm_e);
	ose_drop(vm_s);
	//ose_swap(vm_e);
	//ose_unpackDrop(vm_e);
}

void osevm_assign(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char * const address = ose_peekAddress(vm_c);
	if(!strncmp(address, "/@/", 3)){
		ose_pushString(vm_s, address + 2);
	}
	if(ose_bundleIsEmpty(vm_s) == OSETT_TRUE){
		ose_rassert(0 && "assignment requires at least one argument", 1);
		return;
	}
	ose_moveStringToAddress(vm_s);
	while(1){
		int32_t n = ose_getBundleElemCount(vm_s);
		if(n == 1){
			break;
		}
		ose_swap(vm_s);
		ose_push(vm_s);
	}
	ose_replaceBundleElemInDest(vm_s, vm_e);
}

void osevm_lookup(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	const char * const address = ose_peekAddress(vm_c);
	if(!strncmp(address, "/$/", 3)){
		ose_pushString(vm_e,
			       address + 2);
	}else{
		popStackToEnv(vm_s, vm_e);
	}
	ose_countElems(vm_e);
	ose_moveBundleElemToDest(vm_e, vm_d);
	ose_pickMatch(vm_e);
	ose_countElems(vm_e);
	ose_moveBundleElemToDest(vm_d, vm_e);
	ose_eql(vm_e);
	
	if(ose_popInt32(vm_e) == 0){
		// success
		ose_swap(vm_e);
		ose_drop(vm_e);
	}else{
		// failure
		ose_copyAddressToString(vm_e);
		ose_swap(vm_e);
		ose_push(vm_e);
		ose_concatenateStrings(vm_e);
	}
	ose_moveBundleElemToDest(vm_e, vm_s);
}

void osevm_toInt32(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char * const address = ose_peekAddress(vm_c);
	if(address[2] == '/'){
		const int32_t l = strtol(address + 3, NULL, 10);
		ose_pushInt32(vm_s, l);
	}else{
		const char t = ose_peekMessageArgType(vm_s);
		switch(t){
		case OSETT_INT32: {
			;
		}
			break;
		case OSETT_FLOAT: {
			const float f = ose_popFloat(vm_s);
			ose_pushInt32(vm_s, (int32_t)f);
		}
			break;
		case OSETT_STRING: {
			const char * const s = ose_peekString(vm_s);
			int32_t l;
			if(*s == '/'){
				l = strtol(s + 1, NULL, 10);
			}else{
				l = strtol(s, NULL, 10);
			}
			ose_drop(vm_s);
			ose_pushInt32(vm_s, l);
		}
			break;
		case OSETT_BLOB: {
			const char * const b = ose_peekBlob(vm_s);
			if(ose_ntohl(*((int32_t *)b)) == 4){
				int32_t i = ose_ntohl(*((int32_t *)(b + 4)));
				ose_drop(vm_s);
				ose_pushInt32(vm_s, i);
			}else{
				popControlToStack(vm_c, vm_s);
			}
		}
			break;
		default:
			popControlToStack(vm_c, vm_s);
		}
	}
}

void osevm_toFloat(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char * const address = ose_peekAddress(vm_c);
	if(address[2] == '/'){
		const float f = strtof(address + 3, NULL);
		ose_pushFloat(vm_s, f);
	}else{
		const char t = ose_peekMessageArgType(vm_s);
		switch(t){
		case OSETT_INT32: {
			const int32_t i = ose_popInt32(vm_s);
			ose_pushFloat(vm_s, (float)i);
		}
			break;
		case OSETT_FLOAT: {
			;
		}
			break;
		case OSETT_STRING: {
			const char * const s = ose_peekString(vm_s);
			float f;
			if(*s == '/'){
				f = strtof(s + 1, NULL);
			}else{
				f = strtof(s, NULL);
			}
			ose_drop(vm_s);
			ose_pushFloat(vm_s, f);
		}
			break;
		case OSETT_BLOB: {
			const char * const b = ose_peekBlob(vm_s);
			if(ose_ntohl(*((int32_t *)b)) == 4){
				int32_t i = ose_ntohl(*((int32_t *)(b + 4)));
				float f = *((float *)&i);
				ose_drop(vm_s);
				ose_pushFloat(vm_s, f);
			}else{
				popControlToStack(vm_c, vm_s);
			}
		}
			break;
		default:
			popControlToStack(vm_c, vm_s);
		}
	}
}

void osevm_toString(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char * const address = ose_peekAddress(vm_c);
	if(address[2] == '/'){
		ose_pushString(vm_s, address + 3);
	}else{
		const char t = ose_peekMessageArgType(vm_s);
		switch(t){
		case OSETT_INT32: {
			const int32_t i = ose_popInt32(vm_s);
			const int32_t n = snprintf(NULL, 0, "%d", i);
			ose_pushBlob(vm_s, n - 3 >= 0 ? n - 3 : 0, NULL);
			char *p = ose_peekBlob(vm_s);
			snprintf(p, n + 1, "%d", i);
			p--;
			while(*p != OSETT_BLOB){
				p--;
			}
			*p = OSETT_STRING;
		}
			break;
		case OSETT_FLOAT: {
			const float f = ose_popFloat(vm_s);
			const int32_t n = snprintf(NULL, 0, "%f", f);
			ose_pushBlob(vm_s, n - 3 >= 0 ? n - 3 : 0, NULL);
			char *p = ose_peekBlob(vm_s);
			snprintf(p, n + 1, "%f", f);
			p--;
			while(*p != OSETT_BLOB){
				p--;
			}
			*p = OSETT_STRING;
		}
			break;
		case OSETT_STRING: {
			;
		}
			break;
		case OSETT_BLOB: {
			ose_pushInt32(vm_s, OSETT_STRING);
			ose_blobToType(vm_s);
		}
			break;
		default:
			popControlToStack(vm_c, vm_s);
		}
	}
}

void osevm_toBlob(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	char *address = ose_peekAddress(vm_c);
	if(address[2] == '/'){
		char *p = address + 3;
		const int32_t n = strlen(p);
		ose_assert(n % 2 == 0);
		ose_pushBlob(vm_s, n / 2, NULL);
		char *bp = ose_peekBlob(vm_s);
		bp += 4;
		while(*p){
			char buf[2] = {*p, *(p + 1)};
			char c = (char)strtol(buf, NULL, 16);
			*bp++ = c;
			p += 2;
		}
	}else{
		ose_itemToBlob(vm_s);
	}
}

ose_bundle osevm_init(ose_bundle bundle)
{
	{
		// ose_pushContextMessage(bundle,
		// 		       OSE_CONTEXT_MESSAGE_OVERHEAD
		// 		       + OSE_VM_STATUS_SIZE,
		// 		       "/st");
		// ose_pushContextMessage(bundle,
		// 		       ose_spaceAvailable(bundle),
		// 		       "/vm");
		// bundle = ose_enter(bundle, "/vm");
	}

	{
		// init vm
#ifdef OSE_VM_SIZE
		const int32_t s = OSE_VM_STACK_SIZE;
#else
		int32_t s = ose_spaceAvailable(bundle);

		int32_t cachesize = OSE_CONTEXT_MESSAGE_OVERHEAD
			+ 4 + 4 + 8 + (6 * 4);
		s -= cachesize;
		ose_pushContextMessage(bundle,
				       cachesize,
				       "/co");
		while((s / 6) % 4){
			s--;
		}
		s /= 6;
#endif		

		// input from the world
		ose_pushContextMessage(bundle,
				       s,
				       "/ii");
		// stack
		ose_pushContextMessage(bundle,
				       s,
				       "/ss");
		// environment
		ose_pushContextMessage(bundle,
				       s,
				       "/ee");
		// control
		ose_pushContextMessage(bundle,
				       s,
				       "/cc");
		// dump
		ose_pushContextMessage(bundle,
				       s,
				       "/dd");
		// output to the world
		ose_pushContextMessage(bundle,
				       s,
				       "/oo");

#ifndef OSE_VM_SIZE
		ose_bundle vm_cache = ose_enter(bundle, "/co");
		ose_bundle vm_i = ose_enter(bundle, "/ii");
		ose_bundle vm_s = ose_enter(bundle, "/ss");
		ose_bundle vm_e = ose_enter(bundle, "/ee");
		ose_bundle vm_c = ose_enter(bundle, "/cc");
		ose_bundle vm_d = ose_enter(bundle, "/dd");
		ose_bundle vm_o = ose_enter(bundle, "/oo");
		ose_pushMessage(vm_cache,
				OSE_ADDRESS_ANONVAL,
				OSE_ADDRESS_ANONVAL_LEN,
				6,
				OSETT_INT32,
				ose_getBundlePtr(vm_i) - ose_getBundlePtr(bundle),
				OSETT_INT32,
				ose_getBundlePtr(vm_s) - ose_getBundlePtr(bundle),
				OSETT_INT32,
				ose_getBundlePtr(vm_e) - ose_getBundlePtr(bundle),
				OSETT_INT32,
				ose_getBundlePtr(vm_c) - ose_getBundlePtr(bundle),
				OSETT_INT32,
				ose_getBundlePtr(vm_d) - ose_getBundlePtr(bundle),
				OSETT_INT32,
				ose_getBundlePtr(vm_o) - ose_getBundlePtr(bundle));

		char *b = ose_getBundlePtr(bundle);
		char *c = ose_getBundlePtr(vm_cache);
		int32_t oi = (c + OSE_BUNDLE_HEADER_LEN + 16) - b;
#endif
	}
	return bundle;
}

static void applyControl(ose_bundle osevm, char *address)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);

	// pop control, push to stack, and evaluate
	int32_t addresslen = strlen(address);

	if(!strncmp(address, "/@", 2)){
		ose_try{
			OSEVM_ASSIGN(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/'/", 3)){
		ose_pushMessage(vm_s, address + 2, strlen(address + 2), 0);
	}else if(!strncmp(address, "/$", 2)){
		ose_try{
			OSEVM_LOOKUP(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/!", 2)){
		if(!strncmp(address, "/!/", 3)){
			ose_fn f = ose_symtab_lookup(address + 2);
			if(!f){
				int32_t o = ose_getFirstOffsetForMatch(vm_e,
								       address + 2);
				if(o){
					// assert stuff about o
				        int32_t to, ntt, lto, po, lpo;
					ose_getNthPayloadItem(vm_e,
							      1,
							      o,
							      &to,
							      &ntt,
							      &lto,
							      &po,
							      &lpo);
					if(ose_readByte(vm_e, to + 1)
					   == OSETT_BLOB){
						f = ose_readCFn(vm_e, po + 4);
					}
				}
			}
			if(f){
				f(osevm);
			}else{
				ose_copyAddressToString(vm_c);
				ose_moveBundleElemToDest(vm_c, vm_s);
			}
		}else{
			if(ose_peekMessageArgType(vm_s) == OSETT_BLOB){
				ose_blobToElem(vm_s);
			}
			if(ose_peekType(vm_s) == OSETT_BUNDLE){
				ose_bundleAll(vm_i);
				ose_moveBundleElemToDest(vm_i, vm_d);
				ose_pushBundle(vm_d); // stack
				ose_bundleAll(vm_e);
				ose_copyBundleElemToDest(vm_e, vm_d);
				ose_unpackDrop(vm_e);
				ose_bundleAll(vm_c);
				ose_moveBundleElemToDest(vm_c, vm_d);
				
				ose_pop(vm_s);
				ose_countItems(vm_s);
				int32_t n = ose_popInt32(vm_s);
				for(int32_t i = 0; i < n; i++){
					ose_pop(vm_s);
					ose_pushInt32(vm_s, 3);
					ose_roll(vm_s);
					ose_swap(vm_s);
					ose_push(vm_s);
					ose_moveStringToAddress(vm_s);
					ose_moveBundleElemToDest(vm_s, vm_e);
				}
				ose_drop(vm_s);
				ose_moveBundleElemToDest(vm_s, vm_i);
				ose_popAllDrop(vm_i);
			}else{
				void (*f)(ose_bundle) =
					ose_symtab_lookup(ose_peekString(vm_s));
				if(f){
					ose_drop(vm_s);
					f(osevm);
				}else{
					ose_copyAddressToString(vm_c);
					ose_moveBundleElemToDest(vm_c, vm_s);
				}
			}
		}
	}else if(!strcmp(address, "/(")){
		// // move input to dump
		// ose_bundleAll(vm_i);
		// ose_moveBundleElemToDest(vm_i, "/dd");
		// move stack to dump
		ose_bundleAll(vm_s);
		ose_moveBundleElemToDest(vm_s, vm_d);

		// copy env to dump
		ose_bundleAll(vm_e);
		ose_copyBundleElemToDest(vm_e, vm_d);
		ose_unpackDrop(vm_e);
		
		// // move control to dump
		// ose_bundleAll(vm_c);
		// ose_moveBundleElemToDest(vm_c, vm_d);
	}else if(!strcmp(address, "/)")){
		//xx ose_drop(vm_c);
		// env
		ose_clear(vm_e);
		ose_moveBundleElemToDest(vm_d, vm_e);
		ose_unpackDrop(vm_e);
		// ose_bundleAll(vm_e);
		// ose_moveBundleElemToDest(vm_e, "/ss");
		// ose_moveBundleElemToDest(vm_d, "/ee");
		// ose_unpackDrop(vm_e);

		// stack
		if(ose_getBundleElemCount(vm_s)){
			ose_bundleAll(vm_s);
		}else{
			ose_pushBundle(vm_s);
		}
		ose_moveBundleElemToDest(vm_d, vm_s);
		ose_unpackDrop(vm_s);
		ose_rollBottom(vm_s);
	}else if(!strncmp(address, "/i", 2)){
		OSEVM_TOINT32(osevm);
	}else if(!strncmp(address, "/f", 2)){
		OSEVM_TOFLOAT(osevm);
	}else if(!strncmp(address, "/s", 2)){
		OSEVM_TOSTRING(osevm);
	}else if(!strncmp(address, "/b", 2)){
		OSEVM_TOBLOB(osevm);
	}else if(!strncmp(address, "/&", 2)){
		const char SLIP_END = 0300;
		const char SLIP_ESC = 0333;
		const char SLIP_ESC_END = 0334;
		const char SLIP_ESC_ESC = 0335;
		char c = 0;
		if(address[2] == '/'){
			c = (char)strtol(address + 3, NULL, 10);
		}else{
			
		}
		const int32_t n = ose_getBundleElemCount(vm_s);
		if(n == 0){
			ose_pushMessage(vm_s,
					OSE_ADDRESS_ANONVAL,
					OSE_ADDRESS_ANONVAL_LEN,
					2,
					OSETT_BLOB, 0, NULL,
					OSETT_INT32, 1);
		}else{
			const char elemtype = ose_peekType(vm_s);
			if(elemtype == OSETT_BUNDLE){
				ose_pushMessage(vm_s,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						2,
						OSETT_BLOB, 0, NULL,
						OSETT_INT32, 1);
			}else if(elemtype == OSETT_MESSAGE){
				if(ose_peekMessageArgType(vm_s) == OSETT_INT32){
					ose_pop(vm_s);
					int32_t state = ose_popInt32(vm_s);
					if(ose_peekMessageArgType(vm_s)
					   == OSETT_BLOB){
						switch(state){
						case 0:
							ose_pushInt32(vm_s, 1);
							ose_push(vm_s);
							break;
						case 1:
							switch(c){
							case SLIP_END:
								// done
								break;
							case SLIP_ESC:
								ose_pushInt32(vm_s, 2);
								ose_push(vm_s);
								break;
							default:
								ose_pushBlob(vm_s,
									     1,
									     &c);
								ose_push(vm_s);
								ose_concatenateBlobs(vm_s);
								ose_pushInt32(vm_s, 1);
								ose_push(vm_s);
								break;
							}
							break;
						case 2:
							switch(c){
							case SLIP_ESC_END: {
								const char cc = SLIP_END;	
								ose_pushBlob(vm_s,
									     1,
									     &cc);
								ose_push(vm_s);
								ose_concatenateBlobs(vm_s);
								ose_pushInt32(vm_s, 1);
								ose_push(vm_s);
							}
								break;
							case SLIP_ESC_ESC: {
								const char cc = SLIP_ESC;	
								ose_pushBlob(vm_s,
									     1,
									     &cc);
								ose_push(vm_s);
								ose_concatenateBlobs(vm_s);
								ose_pushInt32(vm_s, 1);
								ose_push(vm_s);
							}
								break;
							default:
								ose_assert(0
									   && "SLIP ESC not followed by ESC_END or ESC_ESC.");
							}
							break;
						default:
							ose_pushInt32(vm_s, state);
							ose_push(vm_s);
							ose_pushMessage(vm_s,
									OSE_ADDRESS_ANONVAL,
									OSE_ADDRESS_ANONVAL_LEN,
									2,
									OSETT_BLOB, 0, NULL,
									OSETT_INT32, 1);
							break;
						}
					}else{
						ose_pushInt32(vm_s, state);
						ose_push(vm_s);
						ose_pushMessage(vm_s,
							OSE_ADDRESS_ANONVAL,
							OSE_ADDRESS_ANONVAL_LEN,
							2,
							OSETT_BLOB, 0, NULL,
							OSETT_INT32, 1);
					}
				}else{
					ose_pushMessage(vm_s,
							OSE_ADDRESS_ANONVAL,
							OSE_ADDRESS_ANONVAL_LEN,
							2,
							OSETT_BLOB, 0, NULL,
							OSETT_INT32, 1);
				}
			}else{
				ose_assert(0 && "found something other than "
					   "a bundle or message");
			}		
		}
	}else{
		popControlToStack(vm_c, vm_s);
	}
}

static void popAllControl(ose_bundle osevm)
{
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	// load an element from the input to
	// the control, and unpack it
	if(strcmp(ose_peekAddress(vm_c), OSE_ADDRESS_ANONVAL)){
		ose_pushString(vm_c, OSE_ADDRESS_ANONVAL);
		ose_push(vm_c);
		ose_swapStringToAddress(vm_c);
	}
	ose_countItems(vm_c);
	int32_t n = ose_popInt32(vm_c);
	for(int i = 0; i < n; i++){
		ose_pop(vm_c);
		if(ose_isStringType(ose_peekMessageArgType(vm_c))
		   == OSETT_TRUE){
			char *str = ose_peekString(vm_c);
			if(!strcmp(str, "/@")
			   || !strncmp(str, "/@/", 3)
			   || !strncmp(str, "/'/", 3)
			   || !strcmp(str, "/!")
			   || !strncmp(str, "/!/", 3)
			   || !strcmp(str, "/$")
			   || !strncmp(str, "/$/", 3)
			   || !strcmp(str, "/(")
			   || !strcmp(str, "/)")
			   || !strcmp(str, "/&")
			   || !strncmp(str, "/&/", 3)
			   || !strncmp(str, "/i/", 3)
			   || !strcmp(str, "/i")
			   || !strncmp(str, "/f/", 3)
			   || !strcmp(str, "/f")
			   || !strncmp(str, "/s/", 3)
			   || !strcmp(str, "/s")
			   || !strncmp(str, "/b/", 3)
			   || !strcmp(str, "/b")){
				ose_moveStringToAddress(vm_c);
			}
		}
		ose_swap(vm_c);
	}
	ose_drop(vm_c);
}

static void restoreDump(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	// output
	// input
	// stack
	// environment
	// control

	// restore control
	ose_moveBundleElemToDest(vm_d, vm_c);
	ose_unpackDrop(vm_c);
			
	// restore env
	// ose_clear(vm_e);
	// ose_moveBundleElemToDest(vm_d, vm_e);
	// ose_unpackDrop(vm_e);

	// restore stack
	ose_bundleAll(vm_s);
	ose_moveBundleElemToDest(vm_d, vm_s);
	ose_unpackDrop(vm_s);
	ose_rollBottom(vm_s);
	//ose_unpackDrop(vm_s);

	// restore input
	ose_moveBundleElemToDest(vm_d, vm_i);
	ose_unpackDrop(vm_i);

	// move output to stack and restore output
	// ose_bundleAll(vm_o);
	// //ose_moveBundleElemToDest(vm_o, "/s");
	// ose_moveBundleElemToDest(vm_d, "/o");
	// ose_unpackDrop(vm_o);
}

void osevm_preInput(ose_bundle osevm)
{
}

void osevm_postInput(ose_bundle osevm)
{
}

void osevm_preControl(ose_bundle osevm)
{
}

void osevm_postControl(ose_bundle osevm)
{
}

char osevm_step(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	if(ose_bundleIsEmpty(vm_c) == OSETT_FALSE){
		// extern jmp_buf ose_jmp_buf;
		// if(!setjmp(ose_jmp_buf)){
		// 	applyControl(osevm, ose_peekAddress(vm_c));
		// 	ose_drop(vm_c);
		// }else{
		// 	printf("drop into debugger\n");
		// 	abort();
		// }
		ose_try{
			applyControl(osevm, ose_peekAddress(vm_c));
		}ose_catch(1){
			// debug
		}ose_finally{
			ose_drop(vm_c);
		}ose_end_try;
		return OSETT_TRUE;
	}else if(ose_bundleIsEmpty(vm_i) == OSETT_FALSE){
		popInputToControl(vm_i, vm_c);
		popAllControl(osevm);		
		return OSETT_TRUE;
	}else if(ose_bundleIsEmpty(vm_d) == OSETT_FALSE){
		restoreDump(osevm);
		return OSETT_TRUE;
	}
	return OSETT_FALSE;
}

void osevm_run(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	int32_t n = ose_getBundleElemCount(vm_d);
	OSEVM_PREINPUT(osevm);
	while(1){
		while(1){
			if(ose_bundleIsEmpty(vm_c) == OSETT_TRUE){
				if(ose_bundleIsEmpty(vm_i) == OSETT_TRUE){
					break;
				}
				popInputToControl(vm_i, vm_c);
				popAllControl(osevm);
			}
			OSEVM_PRECONTROL(osevm);
			while(1){
				if(ose_bundleIsEmpty(vm_c) == OSETT_TRUE){
					break;
				}
				applyControl(osevm, ose_peekAddress(vm_c));
				// check status and drop into
				// debugger if necessary
				ose_drop(vm_c);
			}
			OSEVM_POSTCONTROL(osevm);
		}
		if(ose_bundleIsEmpty(vm_d) == OSETT_FALSE
		   && ose_getBundleElemCount(vm_d) > n){
			restoreDump(osevm);
		}else{
			break;
		}
	}
	OSEVM_POSTINPUT(osevm);
}
