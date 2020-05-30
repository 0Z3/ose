#include <emscripten.h>

#include "ose_conf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_parse.h"
#include "ose_symtab.h"
#include "ose_print.h"

char *bytes;
ose_bundle bundle_, osevm_, vm_i_, vm_s_, vm_e_, vm_c_, vm_d_, vm_o_;
#define MAX_BUNDLE_LEN 1000000

ose_bundle EMSCRIPTEN_KEEPALIVE bundle(void)	{ return bundle_; }
ose_bundle EMSCRIPTEN_KEEPALIVE osevm(void) 	{ return osevm_; }
ose_bundle EMSCRIPTEN_KEEPALIVE vm_i(void)  	{ return vm_i_; }
ose_bundle EMSCRIPTEN_KEEPALIVE vm_s(void) 	{ return vm_s_; }
ose_bundle EMSCRIPTEN_KEEPALIVE vm_e(void) 	{ return vm_e_; }
ose_bundle EMSCRIPTEN_KEEPALIVE vm_c(void) 	{ return vm_c_; }
ose_bundle EMSCRIPTEN_KEEPALIVE vm_d(void) 	{ return vm_d_; }
ose_bundle EMSCRIPTEN_KEEPALIVE vm_o(void) 	{ return vm_o_; }

char* EMSCRIPTEN_KEEPALIVE osejs_popString(ose_bundle bundle)
{
	const char * const str = ose_peekString(bundle);
	const int32_t len = strlen(str);
	char *copy = (char *)malloc(len + 1);
	memcpy(copy, str, len + 1);
	ose_pop(bundle);
	ose_drop(bundle);
	return copy;
}

char* EMSCRIPTEN_KEEPALIVE osejs_popBlob(ose_bundle bundle)
{
	const char * const blob = ose_peekBlob(bundle);
	const int32_t len = ose_ntohl(*((int32_t *)blob));
	char *copy = (char *)malloc(len);
	memcpy(copy, blob + 4, len);
	ose_pop(bundle);
	ose_drop(bundle);
	return copy;
}

void EMSCRIPTEN_KEEPALIVE osejs_pprintBundle(ose_bundle bundle)
{
	char buf[8192];
	memset(buf, 0, 8192);
	ose_pprintBundle(bundle, buf, 8192);
	printf("%s\n", buf);
}

int main()
{
	bytes = (char *)calloc(1, MAX_BUNDLE_LEN);
	bundle_ = ose_newBundleFromCBytes(MAX_BUNDLE_LEN, bytes);
	osevm_ = osevm_init(bundle_);
	vm_i_ = OSEVM_INPUT(osevm_);
	vm_s_ = OSEVM_STACK(osevm_);
	vm_e_ = OSEVM_ENV(osevm_);
	vm_c_ = OSEVM_CONTROL(osevm_);
        vm_d_ = OSEVM_DUMP(osevm_);
	vm_o_ = OSEVM_OUTPUT(osevm_);

	// ose_pushInt32(vm_s, 66);
	// ose_pushInt32(vm_s, 666);
	// ose_pushMessage(vm_i, "/!/add", strlen("/!/add"), 0);
	// osevm_run(osevm);
	// char pb[8192];
	// memset(pb, 0, 8192);
	// ose_pprintBundle(vm_s, pb, 8192);
	// printf("%s\n", pb);
	return 0;
}
