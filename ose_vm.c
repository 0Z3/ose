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

#if defined(OSE_ENDIAN)

#if OSE_ENDIAN == OSE_LITTLE_ENDIAN

#define OSEVM_INTCMP_MASK_1  	0x000000ff
#define OSEVM_INTCMP_MASK_2  	0x0000ffff
#define OSEVM_INTCMP_MASK_3  	0x00ffffff

#define OSEVM_TOK_AT	     	0x002f402f
#define OSEVM_TOK_QUOTE      	0x002f272f
#define OSEVM_TOK_BANG		0x002f212f
#define OSEVM_TOK_DOLLAR	0x002f242f
#define OSEVM_TOK_GT       	0x002f3e2f
#define OSEVM_TOK_LTLT	     	0x2f3c3c2f
#define OSEVM_TOK_LT	     	0x002f3c2f
#define OSEVM_TOK_DASH     	0x002f2d2f
#define OSEVM_TOK_DOT     	0x002f2e2f
#define OSEVM_TOK_COLON     	0x002f3a2f
#define OSEVM_TOK_SCOLON     	0x002f3b2f
#define OSEVM_TOK_PIPE     	0x002f7c2f
#define OSEVM_TOK_OPAR     	0x002f282f
#define OSEVM_TOK_CPAR     	0x002f292f
#define OSEVM_TOK_i         	0x002f692f
#define OSEVM_TOK_f         	0x002f662f
#define OSEVM_TOK_s         	0x002f732f
#define OSEVM_TOK_b         	0x002f622f
#define OSEVM_TOK_AMP     	0x002f262f

#define route_init(address, varname)			\
	struct { int32_t i2, i3, i4; } varname;		\
 	do {						\
 		varname.i4 = *((int32_t *)address);	\
 		varname.i3 = varname.i4 & 0x00ffffff;	\
 		varname.i2 = varname.i3 & 0x0000ffff;	\
 	} while(0);

#elif OSE_ENDIAN == OSE_BIG_ENDIAN

#define OSEVM_INTCMP_MASK_1  0xff000000
#define OSEVM_INTCMP_MASK_2  0xffff0000
#define OSEVM_INTCMP_MASK_3  0xffffff00

#define OSEVM_TOK_AT	     	0x2f402f00
#define OSEVM_TOK_QUOTE      	0x2f272f00
#define OSEVM_TOK_BANG		0x2f212f00
#define OSEVM_TOK_DOLLAR	0x2f242f00
#define OSEVM_TOK_GT       	0x2f3e2f00
#define OSEVM_TOK_LTLT	     	0x2f3c3c2f
#define OSEVM_TOK_LT	     	0x2f3c2f00
#define OSEVM_TOK_DASH     	0x2f2d2f00
#define OSEVM_TOK_DOT     	0x2f2e2f00
#define OSEVM_TOK_COLON     	0x2f3a2f00
#define OSEVM_TOK_SCOLON     	0x2f3b2f00
#define OSEVM_TOK_PIPE     	0x2f7c2f00
#define OSEVM_TOK_OPAR     	0x2f282f00
#define OSEVM_TOK_CPAR     	0x2f292f00
#define OSEVM_TOK_i         	0x2f692f00
#define OSEVM_TOK_f         	0x2f662f00
#define OSEVM_TOK_s         	0x2f732f00
#define OSEVM_TOK_b         	0x2f622f00
#define OSEVM_TOK_AMP     	0x2f262f00

#define route_init(address, varname)			\
	struct { int32_t i2, i3, i4; } varname;		\
 	do {						\
 		varname.i4 = *((int32_t *)address);	\
 		varname.i3 = varname.i4 & 0xffffff00;	\
 		varname.i2 = varname.i3 & 0xffff0000;	\
 	} while(0);
#endif

#define route_pfx_2(var, sym) ((sym) == (var.i4))
#define route_pfx_1(var, sym) ((sym & OSEVM_INTCMP_MASK_3) == (var.i3))
#define route_pfx(var, sym, n) (route_pfx_##n((var), (sym)))

#define route_mthd_2(var, sym) ((sym & OSEVM_INTCMP_MASK_3) == (var.i4))
#define route_mthd_1(var, sym) ((sym & OSEVM_INTCMP_MASK_2) == (var.i4))
#define route_mthd(var, sym, n) (route_mthd_##n((var), (sym)))

#else

#define route_init(address, varname) const char * const varname = address

#define route_pfx(var, sym, n) (strncmp(var, sym, n + 2) == 0)
#define route_mthd(var, sym, n) (strncmp(var, sym, n + 1) == 0)

#define OSEVM_TOK_AT		"/@/\0"
#define OSEVM_TOK_QUOTE		"/'/\0"
#define OSEVM_TOK_BANG		"/!/\0"
#define OSEVM_TOK_DOLLAR	"/$/\0"
#define OSEVM_TOK_GT       	"/>/\0"
#define OSEVM_TOK_LTLT		"/<</"
#define OSEVM_TOK_LT	     	"/</\0"
#define OSEVM_TOK_DASH     	"/-/\0"
#define OSEVM_TOK_DOT     	"/./\0"
#define OSEVM_TOK_COLON     	"/:/\0"
#define OSEVM_TOK_SCOLON     	"/;/\0"
#define OSEVM_TOK_PIPE     	"/|/\0"
#define OSEVM_TOK_OPAR     	"/(/\0"
#define OSEVM_TOK_CPAR     	"/)/\0"
#define OSEVM_TOK_i         	"/i/\0"
#define OSEVM_TOK_f         	"/f/\0"
#define OSEVM_TOK_s         	"/s/\0"
#define OSEVM_TOK_b         	"/b/\0"
#define OSEVM_TOK_AMP     	"/&/\0"

#endif

#define route(var, sym, n) (route_mthd(var, sym, n) || route_pfx(var, sym, n))

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
void osevm_assign(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
#ifdef OSE_SKIP//OSE_USE_OPTIMIZED_CODE
	char *eb = ose_getBundlePtr(vm_e);
	int32_t es = ose_readInt32(vm_e, -4);
	char *sb = ose_getBundlePtr(vm_s);
	int32_t ss = ose_readInt32(vm_s, -4);
	
	int addr_is_on_stack = 0;
	if(address[2] == '/'){
		address += 2;
	}else{
		addr_is_on_stack = 1;
		address = ose_peekString(vm_s);
	}
	int32_t addrlen = strlen(address);
	int32_t paddrlen = ose_pnbytes(addrlen);

	int32_t eo = OSE_BUNDLE_HEADER_LEN;
	int32_t amt = 0;
	while(eo < es){
		if(!strcmp(address, eb + eo + 4)){
			int32_t ss = ose_readInt32(vm_e, eo);
			int32_t no = eo + ss + 4;
			memmove(eb + eo,
				eb + no,
				es - no);
			amt += ss + 4;
			es -= ss + 4;
		}
		eo += ose_readInt32(vm_e, eo) + 4;
	}
	memset(eb + es, 0, amt);
	ose_decSize(vm_e, amt);
	memcpy(eb + es + 4, address, addrlen);

	eo = es + 4 + paddrlen;
	eb[eo] = OSETT_ID;
	eo++;
	int32_t so = OSE_BUNDLE_HEADER_LEN;
	while(so < ss){
		int32_t to = so + 4 + ose_pstrlen(sb + so + 4);
		int32_t ntt = strlen(sb + to);
		memcpy(eb + eo, sb + to + 1, ntt - 1);
		int32_t pntt = ose_pnbytes(ntt);
		eo += ntt - 1;
		// we store the offset of the data section where
		// the address starts since we'll be deleting this anyway
		ose_writeInt32(vm_s, so + 4, to + pntt);
		so += ose_readInt32(vm_s, so) + 4;
	}
	while(eo % 4){
		eo++;
	}
	
	so = OSE_BUNDLE_HEADER_LEN;
	while(so < ss){
		int32_t size = ose_readInt32(vm_s, so);
		// get the offset of the data section we stored earlier
		int32_t offset = ose_readInt32(vm_s, so + 4);
		int32_t amt = size - ((offset - so) - 4);
		memcpy(eb + eo, sb + offset, amt);
		eo += amt;
		so += size + 4;
	}
	ose_writeInt32(vm_e, es, (eo - es) - 4);
	ose_incSize(vm_e, eo - es);
	ose_clear(vm_s);
#else	
	if(address[2] == '/'){
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
#endif
}

/**
 * /$
 */
void osevm_lookup(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(address[2] == '/'){
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
void osevm_funcall(ose_bundle osevm, char *address)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	if(address[2] == '/'){
		ose_pushString(vm_s, address + 2);
	}
	ose_builtin_apply(osevm);
}

/**
 * /'
 */
void osevm_quote(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	ose_pushMessage(vm_s, address + 2, strlen(address + 2), 0);
}

/**
 * />
 */
void osevm_copyContextBundle(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_appendToContextBundle(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	if(OSEVM_GET_FLAGS(osevm) & OSEVM_FLAG_COMPILE){
		popControlToStack(vm_c, vm_s);
		return;
	}
	ose_bundle src = vm_s;
	ose_bundle dest = vm_e;
	if(address[3] == '/'){
		dest = ose_enter(osevm, address + 3);
	}
	ose_appendBundle(src, dest);	
}

/**
 * /<
 */
void osevm_replaceContextBundle(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_moveElemToContextBundle(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_o = OSEVM_OUTPUT(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_exec(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
			ose_pushMessage(vm_s, "/!/exec", 7, 0);
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
void osevm_execInCurrentContext(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_apply(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_return(ose_bundle osevm, char *address)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_defun(ose_bundle osevm, char *address)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_e = OSEVM_ENV(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
        ose_bundle vm_d = OSEVM_DUMP(osevm);
	ose_bundle vm_o = OSEVM_OUTPUT(osevm);
	
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
void osevm_endDefun(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	ose_bundle vm_d = OSEVM_DUMP(osevm);
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
void osevm_toInt32(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_toFloat(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_toString(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_toBlob(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
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
void osevm_appendByte(ose_bundle osevm, char *address)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);

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

void osevm_default(ose_bundle osevm, char *address)
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

	route_init(address, a);

	int32_t flags = OSEVM_GET_FLAGS(osevm);
	if(flags & OSEVM_FLAG_COMPILE){
		if(route_mthd(a, OSEVM_TOK_CPAR, 1)){
			ose_try{
				OSEVM_ENDDEFUN(osevm, address);
			}ose_catch(1){
				;
				// debug
			}ose_finally{
				;
			}ose_end_try;
			return;
		}
	}

	if(!strncmp(address, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN)){
		ose_copyElem(vm_c, vm_s);
	}else if(route(a, OSEVM_TOK_AT, 1)){
		ose_try{
			OSEVM_ASSIGN(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_DOLLAR, 1)){
		ose_try{
			OSEVM_LOOKUP(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_BANG, 1)){
		ose_try{
			OSEVM_FUNCALL(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route_pfx(a, OSEVM_TOK_QUOTE, 1)){
		ose_try{
			OSEVM_QUOTE(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_GT, 1)){
		ose_try{
			OSEVM_COPYCONTEXTBUNDLE(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_LTLT, 2)){
		ose_try{
			OSEVM_APPENDTOCONTEXTBUNDLE(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_LT, 1)){
		ose_try{
			OSEVM_REPLACECONTEXTBUNDLE(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_DASH, 1)){
		ose_try{
			OSEVM_MOVEELEMTOCONTEXTBUNDLE(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_DOT, 1)){
		ose_try{
			OSEVM_EXEC(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_PIPE, 1)){
		ose_try{
			OSEVM_EXECINCURRENTCONTEXT(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_COLON, 1)){
		ose_try{
			OSEVM_APPLY(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_SCOLON, 1)){
		ose_try{
			OSEVM_RETURN(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_OPAR, 1)){
		ose_try{
			OSEVM_DEFUN(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_i, 1)){
		ose_try{
			OSEVM_TOINT32(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_f, 1)){
		ose_try{
			OSEVM_TOFLOAT(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_s, 1)){
		ose_try{
			OSEVM_TOSTRING(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_b, 1)){
		ose_try{
			OSEVM_TOBLOB(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else if(route(a, OSEVM_TOK_AMP, 1)){
		ose_try{
			OSEVM_APPENDBYTE(osevm, address);
		}ose_catch(1){
			;
			// debug
		}ose_finally{
			;
		}ose_end_try;
	}else{
		ose_try{
			OSEVM_DEFAULT(osevm, address);
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
		char *address = ose_peekString(vm_c);
		route_init(address, ac);
		if(route(ac, OSEVM_TOK_AT, 1)
		   || route_pfx(ac, OSEVM_TOK_QUOTE, 1)
		   || route(ac, OSEVM_TOK_BANG, 1)
		   || route(ac, OSEVM_TOK_DOLLAR, 1)
		   || route(ac, OSEVM_TOK_GT, 1)
		   || route(ac, OSEVM_TOK_LTLT, 2)
		   || route(ac, OSEVM_TOK_LT, 1)
		   || route(ac, OSEVM_TOK_DASH, 1)
		   || route(ac, OSEVM_TOK_DOT, 1)
		   || route(ac, OSEVM_TOK_COLON, 1)
		   || route(ac, OSEVM_TOK_SCOLON, 1)
		   || route(ac, OSEVM_TOK_PIPE, 1)
		   || route(ac, OSEVM_TOK_OPAR, 1)
		   || route(ac, OSEVM_TOK_CPAR, 1)
		   || route(ac, OSEVM_TOK_i, 1)
		   || route(ac, OSEVM_TOK_f, 1)
		   || route(ac, OSEVM_TOK_s, 1)
		   || route(ac, OSEVM_TOK_b, 1)
		   || route(ac, OSEVM_TOK_AMP, 1)
		   || OSEVM_ISKNOWNADDRESS(address) != 0){
			ose_moveStringToAddress(vm_c);
		}
	}
}

static int isKnownAddress(const char * const address)
{
	route_init(address, ac);
	if(route(ac, OSEVM_TOK_AT, 1)
	   || route_pfx(ac, OSEVM_TOK_QUOTE, 1)
	   || route(ac, OSEVM_TOK_BANG, 1)
	   || route(ac, OSEVM_TOK_DOLLAR, 1)
	   || route(ac, OSEVM_TOK_GT, 1)
	   || route(ac, OSEVM_TOK_LTLT, 2)
	   || route(ac, OSEVM_TOK_LT, 1)
	   || route(ac, OSEVM_TOK_DASH, 1)
	   || route(ac, OSEVM_TOK_DOT, 1)
	   || route(ac, OSEVM_TOK_COLON, 1)
	   || route(ac, OSEVM_TOK_SCOLON, 1)
	   || route(ac, OSEVM_TOK_PIPE, 1)
	   || route(ac, OSEVM_TOK_OPAR, 1)
	   || route(ac, OSEVM_TOK_CPAR, 1)
	   || route(ac, OSEVM_TOK_i, 1)
	   || route(ac, OSEVM_TOK_f, 1)
	   || route(ac, OSEVM_TOK_s, 1)
	   || route(ac, OSEVM_TOK_b, 1)
	   || route(ac, OSEVM_TOK_AMP, 1)
	   || OSEVM_ISKNOWNADDRESS(address) != 0){
		return 1;
	}else{
		return 0;
	}
}

static void popAllControl(ose_bundle osevm)
{
	ose_bundle vm_c = OSEVM_CONTROL(osevm);

#ifdef OSE_USE_OPTIMIZED_CODE

	const int32_t cs = ose_readInt32(vm_c, -4);
	if(cs == OSE_BUNDLE_HEADER_LEN){
		return;
	}
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	int32_t s = ose_readInt32(vm_c, o);
	while(o + s + 4 < cs){
		o += s + 4;
		int32_t s = ose_readInt32(vm_c, o);
	}
	char *b = ose_getBundlePtr(vm_c);
	const char * const addr = b + o + 4;
	if(!strncmp(addr, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN)){
		return;
	}
	const int32_t addrlen = strlen(addr);
	const int32_t paddrlen = ose_pnbytes(addrlen);
	const int32_t to = o + 4 + paddrlen;
	const char * const tt = b + to;
	const int32_t ttlen = strlen(tt);
	const int32_t pttlen = ose_pnbytes(ttlen);
	const int32_t po = to + pttlen;
	const char * const payload = b + po;
	*((int32_t *)(b + o + s + 4)) = 0;
	int32_t *sizes = (int32_t *)(b + o + s + 4);
	int32_t *offsets = (sizes + (4 * (ttlen - 1)));
	int32_t *knownaddresses = (offsets + (4 * (ttlen - 1)));
	char *p = (char *)(knownaddresses + (4 * (ttlen - 1)));
	char *pstart = p;
	int32_t offset = po;
	for(int i = 0; i < ttlen - 1; i++){
		char typetag = tt[i + 1];
		int32_t itemsize = ose_getTypedDatumSize(typetag, b + offset);
		sizes[i] = itemsize;
		offsets[i] = offset;
		if(ose_isStringType(typetag) == OSETT_TRUE){
			knownaddresses[i] = isKnownAddress(b + offset);
		}else{
			knownaddresses[i] = 0;
		}
	}
	for(int i = ttlen - 2; i >= 0; i--){
		int32_t size = sizes[i];
		int32_t offset = offsets[i];
		int32_t knownaddress = knownaddresses[i];
		if(knownaddress){
			*((int32_t *)p) = ose_htonl(size + 4);
			memcpy(p + 4, b + offset, size);
			memcpy(p + 4 + size,
			       OSE_EMPTY_TYPETAG_STRING,
			       OSE_EMPTY_TYPETAG_STRING_SIZE);
			p += 4 + size + OSE_EMPTY_TYPETAG_STRING_SIZE;
		}else{
			*((int32_t *)p) = ose_htonl(size + 8);
			p[8] = OSETT_ID;
			p[9] = tt[i + 1];
			memcpy(p + 12, b + offset, size);
			p += 4 + 4 + 4 + size;
		}
	}
	if(strcmp(addr, OSE_ADDRESS_ANONVAL)){
		if(isKnownAddress(addr)){
			*((int32_t *)p) = ose_htonl(paddrlen + 4);
			memcpy(p + 4, addr, paddrlen);
			memcpy(p + 4 + paddrlen,
			       OSE_EMPTY_TYPETAG_STRING,
			       OSE_EMPTY_TYPETAG_STRING_SIZE);
			p += 4 + paddrlen + OSE_EMPTY_TYPETAG_STRING_SIZE;
		}else{
			*((int32_t *)p) = ose_htonl(paddrlen + 8);
			p[8] = OSETT_ID;
			p[9] = OSETT_STRING;
			memcpy(p + 12, addr, paddrlen);
			p += 4 + 4 + 4 + paddrlen;
		}
	}
	memmove(b + o, pstart, p - pstart);
	memset(p - (pstart - (b + o)), 0, pstart - (b + o));
	int32_t amt = (p - pstart) - (s + 4);
	ose_incSize(vm_c, amt);

#else
	const char * const address = ose_peekAddress(vm_c);
	if(!strncmp(address, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN)){
		return;
	}else if(strncmp(address, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE)){
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
#endif
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
