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

#include "ose_conf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_symtab.h"
#include "ose_print.h"
#include "ose_load.h"

void ose_loadLib(ose_bundle bundle, const char * const name)
{
	ose_bundle vm_s = OSEVM_STACK(bundle);
	void *h = dlopen(name, RTLD_LAZY);
	ose_rassert(h != NULL, 1);
	void (*ose_main)(ose_bundle) = NULL;
	*(void**)(&ose_main) = dlsym(h, "ose_main");
	ose_rassert(ose_main != NULL, 1);
	ose_drop(vm_s);
	ose_main(bundle);
}

void ose_readFile(ose_bundle bundle, const char * const name)
{
	FILE *fp = fopen(name, "rb");
	ose_rassert(fp, 1);
	const int maxlen = 256;
	char buf[maxlen];
	while(fgets(buf, maxlen, fp)){
		int len = strlen(buf);
		if(buf[len - 1] == '\n'){
			buf[len - 1] = 0;
		}
		ose_pushString(bundle, buf);
	}
	fclose(fp);
}
