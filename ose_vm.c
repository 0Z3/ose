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
#include "ose_builtins.h"
#include "ose_vm.h"

static void convertKnownStringAddressToAddress(ose_bundle vm_c);

static void popControlToStack(ose_bundle vm_c, ose_bundle vm_s)
{
	ose_copyElem(vm_c, vm_s);
}

static void popInputToControl(ose_bundle vm_i, ose_bundle vm_c)
{
	ose_moveElem(vm_i, vm_c);
}

/**
 * /@
 */
void osevm_assign(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char * const address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(!strncmp(address, "/@/", 3)){
		ose_pushString(vm_s, address + 2);
	}
	if(ose_bundleIsEmpty(vm_s) == OSETT_TRUE){
		ose_rassert(0 && "assignment requires at least one argument", 1);
		return;
	}
	ose_pushString(vm_e, ose_peekString(vm_s));
	while(ose_rollMatch_impl(vm_e)){
		ose_drop(vm_e);
		ose_pushString(vm_e, ose_peekString(vm_s));
	}
	ose_drop(vm_e);
	while(1){
		int32_t n = ose_getBundleElemCount(vm_s);
		if(n == 1){
			break;
		}
		ose_swap(vm_s);
		if(ose_peekType(vm_s) == OSETT_BUNDLE){
			ose_elemToBlob(vm_s);
		}
		ose_swap(vm_s);
		ose_push(vm_s);
	}
	ose_moveStringToAddress(vm_s);
	ose_moveElem(vm_s, vm_e);
	ose_clear(vm_s);
}

/**
 * /$
 */
void osevm_lookup(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	const char * const address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}	
	// canonical version:
	// if(!strncmp(address, "/$/", 3)){
	// 	ose_pushString(vm_s, address + 2);
	// }else{
	// 	;
	// }
	// ose_bundleAll(vm_s);
	// ose_pop(vm_s);
	// ose_copyBundle(vm_e, vm_s);
	// ose_swap(vm_s);
	// ose_push(vm_s);
	// ose_unpackDrop(vm_s);
	// ose_pickMatch(vm_s);
	// ose_rollBottom(vm_s);
	// ose_swap(vm_s);
	// ose_push(vm_s);
	// ose_bundleAll(vm_s);
	// ose_pop(vm_s);
	// ose_nip(vm_s);
	// ose_unpackDrop(vm_s);

	if(!strncmp(address, "/$/", 3)){
		ose_pushString(vm_e, address + 2);
	}else{
		ose_pushString(vm_e, ose_peekString(vm_s));
		ose_drop(vm_s);
	}
	ose_pickMatch_impl(vm_e);
	ose_moveElem(vm_e, vm_s);
}

/**
 * /!
 */
void osevm_funcall(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	const char *address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(!strncmp(address, "/!/", 3)){
		ose_pushString(vm_s, address + 2);
	}
	ose_builtin_apply(osevm);
}

/**
 * /'
 */
void osevm_quote(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char *address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	ose_pushMessage(vm_s, address + 2, strlen(address + 2), 0);
}

/**
 * />
 */
void osevm_copyContextBundle(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char *address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	ose_bundle src = vm_e;
	ose_bundle dest = vm_s;
	if(address[2] == '/'){
		src = ose_enter(osevm, address + 2);
	}
	ose_copyBundle(src, dest);
}

/**
 * /<<
 */
void osevm_appendToContextBundle(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char *address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	ose_bundle src = vm_s;
	ose_bundle dest = vm_e;
	if(address[2] == '/'){
		dest = ose_enter(osevm, address + 2);
	}
	ose_appendBundle(src, dest);	
}

/**
 * /<
 */
void osevm_replaceContextBundle(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char *address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	ose_bundle src = vm_s;
	ose_bundle dest = vm_e;
	if(address[2] == '/'){
		dest = ose_enter(osevm, address + 2);
	}
	ose_replaceBundle(src, dest);
}

/**
 * /-
 */
void osevm_moveElemToContextBundle(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_o = OSEVM_OUTPUT(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char *address = ose_peekAddress(vm_c);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	ose_bundle src = vm_s;
	ose_bundle dest = vm_o;
	if(address[2] == '/'){
		dest = ose_enter(osevm, address + 2);
	}
	ose_moveElem(src, dest);
}

/**
 * /.
 */
void osevm_exec(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	char *address = ose_peekAddress(vm_c);
	const int32_t addresslen = strlen(address);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(address[2] == '/'){
		int i = 2;
		char sep = 0;
		char char_before_sep = 0;
		while(i < addresslen){
			if(address[i] == '.'
			   || address[i] == ':'
			   || address[i] == ';'
			   || address[i] == '|'){
				sep = address[i];
				char_before_sep = address[i - 1];
				address[i - 1] = 0;
				break;
			}
			i++;
		}
		if(address[3] == '.'
		   || address[3] == ':'
		   || address[3] == ';'
		   || address[3] == '|'){
			ose_pushBundle(vm_s);
			ose_pushString(vm_s, address + 2);
			ose_push(vm_s);
			ose_pushMessage(vm_s, "/!/exec", 8, 0);
			ose_moveElem(vm_s, vm_c);
			ose_swap(vm_c);
			ose_moveElem(vm_s, vm_c);
			ose_swap(vm_c);
		}else{
			if(char_before_sep){
				ose_pushBundle(vm_s);
				ose_pushString(vm_s, address + 2);
				ose_push(vm_s);
				address[i - 1] = char_before_sep;
				ose_pushString(vm_s, address + (i - 1));
				ose_push(vm_s);
			}else{
				ose_pushBundle(vm_s);
				ose_pushString(vm_s, address + 2);
				ose_push(vm_s);
			}
		}
		ose_pushMessage(vm_c, "/!/exec", 7, 0);
		ose_swap(vm_c);
	}else{
		ose_pushMessage(vm_c, "/!/exec", 7, 0);
		ose_swap(vm_c);
	}	
}

/**
 * /|
 */
void osevm_execInCurrentContext(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	char *address = ose_peekAddress(vm_c);
	const int32_t addresslen = strlen(address);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(address[2] == '/'){
		int i = 2;
		char sep = 0;
		char char_before_sep = 0;
		while(i < addresslen){
			if(address[i] == '.'
			   || address[i] == ':'
			   || address[i] == ';'
			   || address[i] == '|'){
				sep = address[i];
				char_before_sep = address[i - 1];
				address[i - 1] = 0;
				break;
			}
			i++;
		}
		if(address[3] == '.'
		   || address[3] == ':'
		   || address[3] == ';'
		   || address[3] == '|'){
			address[i - 1] = char_before_sep;
			// ose_pushBundle(vm_s);
			// ose_pushString(vm_s, address + 2);
			// ose_push(vm_s);
			ose_pushMessage(vm_s,
					address + 2,
					strlen(address + 2),
					0);
			ose_moveElem(vm_s, vm_c);
			ose_swap(vm_c);
			ose_moveElem(vm_s, vm_c);
			convertKnownStringAddressToAddress(vm_c);
			ose_swap(vm_c);
		}else{
			if(char_before_sep){
				ose_pushMessage(vm_s,
						address + 2,
						strlen(address + 2),
						0);
				address[i - 1] = char_before_sep;
				ose_pushMessage(vm_s,
						address + (i - 1),
						strlen(address + (i - 1)),
						0);
				ose_moveElem(vm_s, vm_c);
				ose_swap(vm_c);
				ose_moveElem(vm_s, vm_c);
				ose_swap(vm_c);
			}else{
				ose_pushMessage(vm_c,
						address + 2,
						strlen(address + 2),
						0);
				ose_swap(vm_c);
			}
		}
	}else{
		ose_moveElem(vm_s, vm_c);
		convertKnownStringAddressToAddress(vm_c);
		ose_swap(vm_c);
	}	
}

/**
 * /:
 */
void osevm_apply(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	char *address = ose_peekAddress(vm_c);
	const int32_t addresslen = strlen(address);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(address[2] == '/'){
		int i = 2;
		char sep = 0;
		char char_before_sep = 0;
		while(i < addresslen){
			if(address[i] == '.'
			   || address[i] == ':'
			   || address[i] == ';'
			   || address[i] == '|'){
				sep = address[i];
				char_before_sep = address[i - 1];
				address[i - 1] = 0;
				break;
			}
			i++;
		}
		if(address[3] == '.'
		   || address[3] == ':'
		   || address[3] == ';'
		   || address[3] == '|'){
			address[i - 1] = char_before_sep;
			ose_pushBundle(vm_s);
			ose_pushString(vm_s, address + 2);
			ose_push(vm_s);
			ose_pushMessage(vm_s, "/!/apply", 8, 0);
			ose_moveElem(vm_s, vm_c);
			ose_swap(vm_c);
			ose_moveElem(vm_s, vm_c);
			ose_swap(vm_c);
		}else{
			if(char_before_sep){
				ose_pushBundle(vm_s);
				ose_pushString(vm_s, address + 2);
				ose_push(vm_s);
				address[i - 1] = char_before_sep;
				ose_pushString(vm_s, address + (i - 1));
				ose_push(vm_s);
			}else{
				ose_pushBundle(vm_s);
				ose_pushString(vm_s, address + 2);
				ose_push(vm_s);
			}
		}
		ose_pushMessage(vm_c, "/!/apply", 8, 0);
		ose_swap(vm_c);
	}else{
		ose_pushMessage(vm_c, "/!/apply", 8, 0);
		ose_swap(vm_c);
	}
}

/**
 * /;
 */
void osevm_return(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	char *address = ose_peekAddress(vm_c);
	const int32_t addresslen = strlen(address);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(address[2] == '/'){
		int i = 2;
		char sep = 0;
		char char_before_sep = 0;
		while(i < addresslen){
			if(address[i] == '.'
			   || address[i] == ':'
			   || address[i] == ';'
			   || address[i] == '|'){
				sep = address[i];
				char_before_sep = address[i - 1];
				address[i - 1] = 0;
				break;
			}
			i++;
		}
		if(address[3] == '.'
		   || address[3] == ':'
		   || address[3] == ';'
		   || address[3] == '|'){
			// /;/./~~~ or /;/:/~~~ means return and execute
			// address + 2 in the new context
				
			address[i - 1] = char_before_sep;
			ose_pushString(vm_s, address + (i - 1));
			ose_builtin_return(osevm);
			ose_moveElem(vm_s, vm_i);
			ose_pushMessage(vm_c,
					OSE_ADDRESS_ANONVAL,
					OSE_ADDRESS_ANONVAL_LEN,
					0);
		}else{
			// /;/~~~ means execute address + 2 in this
			// context and then return
				
			if(char_before_sep){
				// /;/~~~/./~~~ or /;/~~~/:/~~~
				// means execute address + 2 in this
				// context, then return and continue

				address[i - 1] = char_before_sep;
				for(int j = 0; j < i - 3; j++){
					address[j] = address[j + 2];
				}
				address[i - 3] = 0;
				address[i - 2] = ';';
				ose_pushMessage(vm_s,
						address,
						strlen(address),
						0);
				address[i - 3] = '/';
				ose_pushMessage(vm_s,
						address + (i - 3),
						strlen(address + (i - 3)),
						0);
				ose_moveElem(vm_s, vm_c);
				ose_swap(vm_c);
				ose_moveElem(vm_s, vm_c);
				ose_swap(vm_c);
			}else{
				// /;/~~~ means execute address + 2
				// in this context and then return

				for(int j = 0; j < i - 2; j++){
					address[j] = address[j + 2];
				}
				address[i - 2] = 0;
				address[i - 1] = ';';
				ose_pushMessage(vm_s,
						address,
						strlen(address),
						0);
				address[i - 2] = '/';
				ose_pushMessage(vm_s,
						address + (i - 2),
						strlen(address + (i - 2)),
						0);
				ose_moveElem(vm_s, vm_c);
				ose_swap(vm_c);
				ose_moveElem(vm_s, vm_c);
				ose_swap(vm_c);
			}
		}
	}else{
		ose_builtin_return(osevm);
		ose_pushMessage(vm_c,
				OSE_ADDRESS_ANONVAL,
				OSE_ADDRESS_ANONVAL_LEN,
				0);
	}
}

/**
 * /(
 */
void osevm_defun(ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	ose_bundle vm_o = OSEVM_OUTPUT(osevm);
	const char * const address = ose_peekAddress(vm_c);
	
	int32_t flags = OSEVM_GET_FLAGS(osevm);
	OSEVM_SET_FLAGS(osevm, flags | OSEVM_FLAG_COMPILE);
	// ose_copyBundle(vm_i, vm_d);
	// ose_copyBundle(vm_e, vm_d);
	ose_copyBundle(vm_s, vm_d);
	ose_clear(vm_s);
	// ose_copyBundle(vm_c, vm_d);
	
	if(address[2] == '/'){
		ose_pushMessage(vm_s,
				address + 2,
				strlen(address + 2),
				0);
	}else{
		ose_pushMessage(vm_s,
				OSE_ADDRESS_ANONVAL,
				OSE_ADDRESS_ANONVAL_LEN,
				0);
	}
}

/**
 * /)
 */
void osevm_endDefun(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_bundle vm_d = OSEVM_DUMP(osevm);
	const char * const address = ose_peekAddress(vm_c);
	int32_t flags = OSEVM_GET_FLAGS(osevm);
	OSEVM_SET_FLAGS(osevm, flags ^ OSEVM_FLAG_COMPILE);
	ose_rollBottom(vm_s);
	ose_bundleAll(vm_s);
	ose_pop(vm_s);
	ose_swap(vm_s);
	ose_push(vm_s);
	// ose_builtin_return(osevm);
	ose_moveElem(vm_d, vm_s);
	ose_unpackDrop(vm_s);
	ose_rollBottom(vm_s);
}

/**
 * /i
 */
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

/**
 * /f
 */
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

/**
 * /s
 */
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

/**
 * /b
 */
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
			char buf[3] = {*p, *(p + 1), 0};
			char c = (char)strtol(buf, NULL, 16);
			*bp++ = c;
			p += 2;
		}
	}else{
		ose_itemToBlob(vm_s);
	}
}

/**
 * /&
 */
void osevm_appendByte(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	char *address = ose_peekAddress(vm_c);

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
}

void osevm_default(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	popControlToStack(vm_c, vm_s);
}

#ifdef OSEVM_HAVE_SIZES
ose_bundle osevm_init(ose_bundle bundle)
#else
ose_bundle osevm_init(ose_bundle bundle,
		      int32_t input_size,
		      int32_t stack_size,
		      int32_t env_size,
		      int32_t control_size,
		      int32_t dump_size,
		      int32_t output_size)
#endif
{
#ifdef OSEVM_HAVE_SIZES
	const int32_t input_size = OSEVM_INPUT_SIZE
		+ OSE_CONTEXT_MESSAGE_OVERHEAD;
	const int32_t stack_size = OSEVM_STACK_SIZE
		+ OSE_CONTEXT_MESSAGE_OVERHEAD;
	const int32_t env_size = OSEVM_ENV_SIZE
		+ OSE_CONTEXT_MESSAGE_OVERHEAD;
	const int32_t control_size = OSEVM_CONTROL_SIZE
		+ OSE_CONTEXT_MESSAGE_OVERHEAD;
	const int32_t dump_size = OSEVM_DUMP_SIZE
		+ OSE_CONTEXT_MESSAGE_OVERHEAD;
	const int32_t output_size = OSEVM_OUTPUT_SIZE
		+ OSE_CONTEXT_MESSAGE_OVERHEAD;
#endif

	// cache
	ose_pushContextMessage(bundle,
			       OSEVM_CACHE_MSG_SIZE,
			       "/_0");

	// input from the world
	ose_pushContextMessage(bundle,
			       input_size,
			       "/_i");
	// stack
	ose_pushContextMessage(bundle,
			       stack_size,
			       "/_s");
	// environment
	ose_pushContextMessage(bundle,
			       env_size,
			       "/_e");
	// control
	ose_pushContextMessage(bundle,
			       control_size,
			       "/_c");
	// dump
	ose_pushContextMessage(bundle,
			       dump_size,
			       "/_d");
	// output to the world
	ose_pushContextMessage(bundle,
			       output_size,
			       "/_o");

	ose_bundle vm_cache = ose_enter(bundle, "/_0");
	ose_bundle vm_i = ose_enter(bundle, "/_i");
	ose_bundle vm_s = ose_enter(bundle, "/_s");
	ose_bundle vm_e = ose_enter(bundle, "/_e");
	ose_bundle vm_c = ose_enter(bundle, "/_c");
	ose_bundle vm_d = ose_enter(bundle, "/_d");
	ose_bundle vm_o = ose_enter(bundle, "/_o");
	ose_pushMessage(vm_cache,
			OSE_ADDRESS_ANONVAL,
			OSE_ADDRESS_ANONVAL_LEN,
			30,
			OSETT_INT32,
			7,
			OSETT_INT32,
			0,
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
			ose_getBundlePtr(vm_o) - ose_getBundlePtr(bundle),
			OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
			OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
			OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
			OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
			OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
			OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
			OSETT_INT32, 0, OSETT_INT32, 0, OSETT_INT32, 0,
			OSETT_INT32, 0);
	return bundle;
}

static void applyControl(ose_bundle osevm, char *address)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	ose_bundle vm_o = OSEVM_OUTPUT(osevm);

	// pop control, push to stack, and execute
	int32_t addresslen = strlen(address);

	int32_t flags = OSEVM_GET_FLAGS(osevm);
	if(flags & OSEVM_FLAG_COMPILE){
		if(!strcmp(address, "/)")){
			ose_try{
				OSEVM_ENDDEFUN(osevm);
			}ose_catch(1){
				;
				// debug
			}ose_finally{
				;
			}ose_end_try;
			return;
		// }else{
		// 	ose_try{
		// 		OSEVM_DEFAULT(osevm);
		// 	}ose_catch(1){
		// 		;
		// 		// debug
		// 	}ose_finally{
		// 		;
		// 	}ose_end_try;
		// 	return;
		}
	}

	if(!strcmp(address, OSE_BUNDLE_ID)){
		ose_copyElem(vm_c, vm_s);
	}else if(!strncmp(address, "/@", 2)){
		ose_try{
			OSEVM_ASSIGN(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
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
		ose_try{
			OSEVM_FUNCALL(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/'/", 3)){
		ose_try{
			OSEVM_QUOTE(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/>", 2)){
		ose_try{
			OSEVM_COPYCONTEXTBUNDLE(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/<<", 3)){
		ose_try{
			OSEVM_APPENDTOCONTEXTBUNDLE(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/<", 2)){
		ose_try{
			OSEVM_REPLACECONTEXTBUNDLE(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/-", 2)){
		ose_try{
			OSEVM_MOVEELEMTOCONTEXTBUNDLE(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/.", 2)){
		ose_try{
			OSEVM_EXEC(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/|", 2)){
		ose_try{
			OSEVM_EXECINCURRENTCONTEXT(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/:", 2)){
		ose_try{
			OSEVM_APPLY(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/;", 2)){
		ose_try{
			OSEVM_RETURN(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/(", 2)){
		ose_try{
			OSEVM_DEFUN(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/i", 2)){
		ose_try{
			OSEVM_TOINT32(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/f", 2)){
		ose_try{
			OSEVM_TOFLOAT(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/s", 2)){
		ose_try{
			OSEVM_TOSTRING(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/b", 2)){
		ose_try{
			OSEVM_TOBLOB(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(!strncmp(address, "/&", 2)){
		ose_try{
			OSEVM_APPENDBYTE(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else{
		ose_try{
			OSEVM_DEFAULT(osevm);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}
}

static void convertKnownStringAddressToAddress(ose_bundle vm_c)
{
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
		   || !strcmp(str, "/>")
		   || !strncmp(str, "/>/", 3)
		   || !strcmp(str, "/<<")
		   || !strncmp(str, "/<</", 4)
		   || !strcmp(str, "/<")
		   || !strncmp(str, "/</", 3)
		   || !strcmp(str, "/-")
		   || !strncmp(str, "/-/", 3)
		   || !strcmp(str, "/.")
		   || !strncmp(str, "/./", 3)
		   || !strcmp(str, "/:")
		   || !strncmp(str, "/:/", 3)
		   || !strcmp(str, "/;")
		   || !strncmp(str, "/;/", 3)
		   || !strcmp(str, "/|")
		   || !strncmp(str, "/|/", 3)
		   || !strcmp(str, "/(")
		   || !strncmp(str, "/(/", 3)
		   || !strcmp(str, "/)")
		   || !strncmp(str, "/)/", 3)
		   || !strncmp(str, "/i/", 3)
		   || !strcmp(str, "/i")
		   || !strncmp(str, "/f/", 3)
		   || !strcmp(str, "/f")
		   || !strncmp(str, "/s/", 3)
		   || !strcmp(str, "/s")
		   || !strncmp(str, "/b/", 3)
		   || !strcmp(str, "/b")
		   || !strcmp(str, "/&")
		   || !strncmp(str, "/&/", 3)){
			ose_moveStringToAddress(vm_c);
		}
	}
}

static void popAllControl(ose_bundle osevm)
{
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	// load an element from the input to
	// the control, and unpack it
	const char * const address = ose_peekAddress(vm_c);
	if(!strcmp(address, OSE_BUNDLE_ID)){
		return;
	}else if(strcmp(address, OSE_ADDRESS_ANONVAL)){
		ose_pushString(vm_c, OSE_ADDRESS_ANONVAL);
		ose_push(vm_c);
		ose_swapStringToAddress(vm_c);
	}
	ose_countItems(vm_c);
	int32_t n = ose_popInt32(vm_c);
	for(int i = 0; i < n; i++){
		ose_pop(vm_c);
		convertKnownStringAddressToAddress(vm_c);
		ose_swap(vm_c);
	}
	ose_drop(vm_c);
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
		ose_try{
			applyControl(osevm, ose_peekAddress(vm_c));
		}ose_catch(1){
			// debug
		}ose_finally{
			if(ose_bundleHasAtLeastNElems(vm_c, 1) == OSETT_TRUE){
				ose_drop(vm_c);
			}	
		}ose_end_try;
		if(ose_bundleIsEmpty(vm_c) == OSETT_TRUE){
			OSEVM_POSTCONTROL(osevm);
		}
	}else if(ose_bundleIsEmpty(vm_i) == OSETT_FALSE){
		popInputToControl(vm_i, vm_c);
		popAllControl(osevm);
		OSEVM_PRECONTROL(osevm);
	}else if(ose_bundleIsEmpty(vm_d) == OSETT_FALSE
		 && !(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE)){
		ose_builtin_return(osevm);
	}
	if(ose_bundleIsEmpty(vm_i) == OSETT_FALSE
	   || ose_bundleIsEmpty(vm_c) == OSETT_FALSE){
		return OSETT_TRUE;
	}else if(ose_bundleIsEmpty(vm_d) == OSETT_FALSE){
		if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
			return OSETT_FALSE;
		}else{
			return OSETT_TRUE;
		}
	}else{
		return OSETT_FALSE;
	}
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
				if(ose_bundleHasAtLeastNElems(vm_c, 1) == OSETT_TRUE){
					ose_drop(vm_c);
				}
			}
			OSEVM_POSTCONTROL(osevm);
		}
		if(ose_bundleIsEmpty(vm_d) == OSETT_FALSE
		   && ose_getBundleElemCount(vm_d) > n
		   && !(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE)){
			ose_builtin_return(osevm);
		}else{
			break;
		}
	}
	OSEVM_POSTINPUT(osevm);
}
