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

#include <stdlib.h>

#include "ose_conf.h"
#include "ose.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_context.h"
#include "ose_assert.h"
#include "ose_lex.h"
#include "ose_vm.h"

void ose_parse(const char *str, ose_bundle osevm)
{
	ose_bundle vm_i = OSEVM_INPUT(osevm);
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_o = OSEVM_OUTPUT(osevm);
	int state = 0;
	while(*str){
		if(!str){
			break;
		}
		str = ose_lex(&state, str, vm_i); // address
		if(!str){
			break;
		}
		str = ose_lex(&state, str, vm_i); // string
		// swap the order of the input bundle
		// that lex created
		ose_bundleAll(vm_i);
		ose_popAllDrop(vm_i);
		osevm_run(osevm);
		ose_swap(vm_s);
		char *a = ose_peekString(vm_s);
		char a0 = a[0];
		char a1 = a[1];
		char a2 = a[2];
		if(a0 == '/'
		   && a2 == '/'){
			switch(a1){
			case '@':
			case '!':
			case '$':
			case '(':
			case ')':
			case '&':
			case 'i':
			case 'f':
				ose_nip(vm_s);
				ose_moveStringToAddress(vm_s);
				ose_moveBundleElemToDest(vm_s, vm_o);
				break;
			case 's':
			case 'b':
				ose_moveStringToAddress(vm_s);
				ose_swap(vm_s);
				ose_moveBundleElemToDest(vm_s, vm_o);
				ose_moveBundleElemToDest(vm_s, vm_o);
				break;
			default:
				ose_nip(vm_s);
				ose_moveBundleElemToDest(vm_s, vm_o);
				break;
			}
		}else if(a0 == '/'
			 && a2 == 0){
			switch(a1){
			case '@':
			case '!':
			case '$':
			case '(':
			case ')':
			case '&':
				ose_nip(vm_s);
				ose_moveStringToAddress(vm_s);
				ose_moveBundleElemToDest(vm_s, vm_o);
				break;
			default:
				ose_nip(vm_s);
				ose_moveBundleElemToDest(vm_s, vm_o);
				break;
			}
		}else{
			ose_nip(vm_s);
			ose_moveBundleElemToDest(vm_s, vm_o);
		}
		
	}
}
