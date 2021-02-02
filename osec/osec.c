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
#include <stdlib.h>
// #include <signal.h>
// #include <setjmp.h>
// #include <unistd.h>
// #include <arpa/inet.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <editline/readline.h>

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_symtab.h"
#include "ose_print.h"
#include "sys/ose_load.h"

#define debug(fmt, ...) \
	fprintf(stderr, "%s:%d: " fmt, __func__, __LINE__, ##__VA_ARGS__)

#define osec_emit(fmt, ...) \
	fprintf(stdout, fmt, ##__VA_ARGS__)

#define OSEC_PASS1 1
#define OSEC_PASS2 2
#define OSEC_PASS3 3
#define OSEC_SCOPE_GLOBAL 1
#define OSEC_SCOPE_NAMEDFN 2
#define OSEC_SCOPE_ANONFN 3

#define OSEC_CACHE_IDX_ANONFNCT 8

#define OSEC_SETPASS(osevm, pass) \
	(OSEVM_SET_FLAGS(osevm, \
			(OSEVM_GET_FLAGS(osevm) & ~0xFF000000) | (pass << 24)))
#define OSEC_GETPASS(osevm) \
	((OSEVM_GET_FLAGS(osevm) & 0xFF000000) >> 24)

#define OSEC_SETSCOPE(osevm, scope) \
	(OSEVM_SET_FLAGS(osevm, \
			(OSEVM_GET_FLAGS(osevm) & ~0xFF0000) | (scope << 16)))
#define OSEC_GETSCOPE(osevm) \
	((OSEVM_GET_FLAGS(osevm) & 0xFF0000) >> 16)

#define OSEC_SET_ANONFNCT(osevm, val) \
	OSEVM_SET_CACHE_VALUE(osevm, OSEC_CACHE_IDX_ANONFNCT, (val))
#define OSEC_INC_ANONFNCT(osevm) \
	OSEVM_SET_CACHE_VALUE(osevm,					\
			      OSEC_CACHE_IDX_ANONFNCT,			\
			      OSEC_GET_ANONFNCT(osevm) + 1)
#define OSEC_GET_ANONFNCT(osevm)				\
	OSEVM_GET_CACHE_VALUE(osevm, OSEC_CACHE_IDX_ANONFNCT)

#define MAX_BUNDLE_LEN 7000000
char *bytes;
ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o;

// options
int verbose = 0;

void osec_eol_impl(ose_bundle osevm, int32_t pass)
{
	switch(pass){
	case OSEC_PASS1:
		osec_emit("\\\n");
		break;
	default:
		osec_emit("\n");
		break;
	}
}

void osec_eol(ose_bundle osevm)
{
	const int32_t pass = OSEC_GETPASS(osevm);
	osec_eol_impl(osevm, pass);
}

void osec_writeIncludes(ose_bundle osevm)
{
	char *sys_includes[] = {
				"stdio.h",
				"string.h",
				"stdlib.h"
	};
	char *ose_includes[] = {
				"ose.h",
				"ose_context.h",
				"ose_util.h",
				"ose_stackops.h",
				"ose_assert.h",
				"ose_vm.h",
				"ose_builtins.h",
				"sys/ose_load.h"
	};
	for(int i = 0; i < sizeof(sys_includes) / sizeof(char*); i++){
		osec_emit("#include <%s>", sys_includes[i]);
		osec_eol_impl(osevm, 0);
	}
	for(int i = 0; i < sizeof(ose_includes) / sizeof(char*); i++){
		osec_emit("#include \"%s\"", ose_includes[i]);
		osec_eol_impl(osevm, 0);
	}
}

void osec_beginFunctionTableMacro(ose_bundle osevm)
{
	osec_emit("#define OSEC_FUNCTION_TABLE(FN) \\\n");
}

void osec_endFunctionTableMacro(ose_bundle osevm)
{
	//osec_emit(")\n\n");
}

void osec_writeVMBundles(ose_bundle osevm)
{
	osec_emit("ose_bundle vm_i = OSEVM_INPUT(osevm);");
	osec_eol(osevm);
	osec_emit("ose_bundle vm_s = OSEVM_STACK(osevm);");
	osec_eol(osevm);
	osec_emit("ose_bundle vm_e = OSEVM_ENV(osevm);");
	osec_eol(osevm);
	osec_emit("ose_bundle vm_c = OSEVM_CONTROL(osevm);");
	osec_eol(osevm);
	osec_emit("ose_bundle vm_d = OSEVM_DUMP(osevm);");
	osec_eol(osevm);
	osec_emit("ose_bundle vm_o = OSEVM_OUTPUT(osevm);");
	osec_eol(osevm);
}

void osec_writeFunctionDefinitions(ose_bundle osevm)
{
	osec_emit("#define OSEC_FUNCTION(NAME, CODE) void NAME (ose_bundle osevm) CODE\n");
	osec_emit("#define OSEC_SYMBOL(NAME, CODE) NAME,\n");
	osec_emit("#define OSEC_STRING(NAME, CODE) #NAME,\n");
	osec_emit("#define OSEC_ADDRESS(NAME, CODE) \"/\" #NAME,\n");
	osec_emit("\n");
}

void osec_writeTables(ose_bundle osevm)
{
	osec_emit("OSEC_FUNCTION_TABLE(OSEC_FUNCTION)\n");
	osec_emit("void (*ose_function_table[])(ose_bundle) = { OSEC_FUNCTION_TABLE(OSEC_SYMBOL) };\n");
	osec_emit("char *ose_symbol_table[] = { OSEC_FUNCTION_TABLE(OSEC_STRING) };\n");
	osec_emit("char *ose_address_table[] = { OSEC_FUNCTION_TABLE(OSEC_ADDRESS) };\n");
}

void osec_startMain(ose_bundle osevm)
{
	osec_emit("void ose_main(ose_bundle osevm)\n");
	osec_emit("{\n");
	//osec_emit("ose_bundle vm_s = OSEVM_STACK(osevm);\n");
	osec_writeVMBundles(osevm);
	osec_emit("for(int i = 0; i < sizeof(ose_function_table) / sizeof(ose_fn); i++){\n");
	osec_emit("ose_pushMessage(vm_e, ose_address_table[i], strlen(ose_address_table[i]), 1, OSETT_CFUNCTION, ose_function_table[i]);\n");
	osec_emit("}\n"); // close for loop
}

void osec_finishMain(ose_bundle osevm)
{
	osec_emit("}\n");
}

/**************************************************
 * hooks
 **************************************************/
	
void osec_assign(ose_bundle osevm, char *address)
{
	int32_t pass = OSEC_GETPASS(osevm);
	int32_t scope = OSEC_GETSCOPE(osevm);
	if((pass == OSEC_PASS1 && scope == OSEC_SCOPE_NAMEDFN)
	   || (pass == OSEC_PASS2 && scope == OSEC_SCOPE_ANONFN)
	   || (pass == OSEC_PASS3 && (scope == OSEC_SCOPE_GLOBAL))){
		//osec_emit("osevm_assign(osevm);");
		osec_emit("ose_pushString(vm_s, \"%s\");", address);
		osec_eol(osevm);
	}
}

void osec_lookup(ose_bundle osevm, char *address)
{
}

void osec_funcall(ose_bundle osevm, char *address)
{
	int32_t pass = OSEC_GETPASS(osevm);
	int32_t scope = OSEC_GETSCOPE(osevm);
	if((pass == OSEC_PASS1 && scope == OSEC_SCOPE_NAMEDFN)
	   || (pass == OSEC_PASS2 && scope == OSEC_SCOPE_ANONFN)
	   || (pass == OSEC_PASS3 && (scope == OSEC_SCOPE_GLOBAL))){
		if(address[2] == '/'){
			char *s = ose_symtab_lookup_fnsym(address + 2);
			if(s){
				osec_emit("%s(osevm);", s);
				osec_eol(osevm);
				return;
			}else{
				osec_emit("ose_pushString(vm_s, %s);",
					address + 2);
				osec_eol(osevm);
				osec_emit("ose_builtin_apply(osevm);");
				osec_eol(osevm);
			}
		}else{
			osec_emit("ose_builtin_apply(osevm);");
			osec_eol(osevm);
		}
	}
}

void osec_quote(ose_bundle osevm, char *address)
{
}

void osec_copyContextBundle(ose_bundle osevm, char *address)
{
}

void osec_appendToContextBundle(ose_bundle osevm, char *address)
{
}

void osec_replaceContextBundle(ose_bundle osevm, char *address)
{
}

void osec_moveElemToContextBundle(ose_bundle osevm, char *address)
{
}

void osec_exec(ose_bundle osevm, char *address)
{
}

void osec_execInCurrentContext(ose_bundle osevm, char *address)
{
}

void osec_apply(ose_bundle osevm, char *address)
{
}

void osec_return(ose_bundle osevm, char *address)
{
}

void osec_defun(ose_bundle osevm, char *address)
{
	OSEVM_SET_FLAG_COMPILE(osevm);
	if(address[2] == '/'){
		// named function, handle on first pass
		OSEC_SETSCOPE(osevm, OSEC_SCOPE_NAMEDFN);
		switch(OSEC_GETPASS(osevm)){
		case 1: {
			osec_emit("FN(%s, ", address + 3);
			osec_eol(osevm);
			osec_emit("{ ");
			osec_eol(osevm);
			osec_writeVMBundles(osevm);
		}
			break;
		}
	}else{
		// anon function, handle on second pass
		OSEC_SETSCOPE(osevm, OSEC_SCOPE_ANONFN);
		switch(OSEC_GETPASS(osevm)){
		case 2: {
			// osec_emit("osevm_defun(osevm);");
			// osec_eol(osevm);
			int32_t ctr = OSEC_GET_ANONFNCT(osevm);
			osec_emit("void ose_anonfn%d(ose_bundle osevm)\n\{\n",
				ctr);
			osec_writeVMBundles(osevm);
		}
			break;
		case 3: {
			osec_emit("ose_pushMessage(vm_s, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN, 1, OSETT_CFUNCTION, ose_anonfn%d);\n", OSEC_GET_ANONFNCT(osevm));
			OSEC_INC_ANONFNCT(osevm);
		}
		}
	}
}

void osec_endDefun(ose_bundle osevm, char *address)
{
	//int32_t pass = OSEC_GETPASS(osevm);
	int32_t scope = OSEC_GETSCOPE(osevm);
	OSEVM_UNSET_FLAG_COMPILE(osevm);
	OSEC_SETSCOPE(osevm, OSEC_SCOPE_GLOBAL);
	switch(OSEC_GETPASS(osevm)){
	case 1:
		if(scope == OSEC_SCOPE_NAMEDFN){
			osec_emit("} ");
			osec_eol(osevm);
			osec_emit(") ");
			osec_eol(osevm);
		}
		break;
	case 2:
		if(scope == OSEC_SCOPE_ANONFN){
			// osec_emit("osevm_endDefun(osevm);");
			// osec_eol(osevm);
			osec_emit("}\n");
			OSEC_INC_ANONFNCT(osevm);
		}
		break;
	}
}

void osec_toInt32(ose_bundle osevm, char *address)
{
	int32_t pass = OSEC_GETPASS(osevm);
	int32_t scope = OSEC_GETSCOPE(osevm);
	if((pass == OSEC_PASS1 && scope == OSEC_SCOPE_NAMEDFN)
	   || (pass == OSEC_PASS2 && scope == OSEC_SCOPE_ANONFN)
	   || (pass == OSEC_PASS3 && (scope == OSEC_SCOPE_GLOBAL))){
		ose_bundle vm_c = OSEVM_CONTROL(osevm);
		const char * const address = ose_peekAddress(vm_c);
		if(address[2] == '/'){
			osec_emit("ose_pushInt32(vm_s, %s);", address + 3);
			osec_eol(osevm);
		}else{
			osec_emit("osevm_toInt32(osevm);");
			osec_eol(osevm);
		}
	}
}

void osec_toFloat(ose_bundle osevm, char *address)
{
	int32_t pass = OSEC_GETPASS(osevm);
	int32_t scope = OSEC_GETSCOPE(osevm);
	if((pass == OSEC_PASS1 && scope == OSEC_SCOPE_NAMEDFN)
	   || (pass == OSEC_PASS2 && scope == OSEC_SCOPE_ANONFN)
	   || (pass == OSEC_PASS3 && (scope == OSEC_SCOPE_GLOBAL))){
		ose_bundle vm_c = OSEVM_CONTROL(osevm);
		const char * const address = ose_peekAddress(vm_c);
		if(address[2] == '/'){
			osec_emit("ose_pushFloat(vm_s, %s);", address + 3);
			osec_eol(osevm);
		}else{
			osec_emit("osevm_toFloat(osevm);");
			osec_eol(osevm);
		}
	}
}

void osec_toString(ose_bundle osevm, char *address)
{
	int32_t pass = OSEC_GETPASS(osevm);
	int32_t scope = OSEC_GETSCOPE(osevm);
	if((pass == OSEC_PASS1 && scope == OSEC_SCOPE_NAMEDFN)
	   || (pass == OSEC_PASS2 && scope == OSEC_SCOPE_ANONFN)
	   || (pass == OSEC_PASS3 && (scope == OSEC_SCOPE_GLOBAL))){
		ose_bundle vm_c = OSEVM_CONTROL(osevm);
		const char * const address = ose_peekAddress(vm_c);
		if(address[2] == '/'){
			osec_emit("ose_pushString(vm_s, %s);", address + 3);
			osec_eol(osevm);
		}else{
			osec_emit("osevm_toString(osevm);");
			osec_eol(osevm);
		}
	}
}

void osec_toBlob(ose_bundle osevm, char *address)
{
	int32_t pass = OSEC_GETPASS(osevm);
	int32_t scope = OSEC_GETSCOPE(osevm);
	if((pass == OSEC_PASS1 && scope == OSEC_SCOPE_NAMEDFN)
	   || (pass == OSEC_PASS2 && scope == OSEC_SCOPE_ANONFN)
	   || (pass == OSEC_PASS3 && (scope == OSEC_SCOPE_GLOBAL))){
		if(address[2] == '/'){
			int32_t len = strlen(address + 3);
			if(len < 256){
				osec_emit("{");
				osec_eol(osevm);
				osec_emit("char buf[] = {");
				osec_eol(osevm);
				const char *p = address + 3;
				for(int i = 0; i < len / 2; i++){
					char buf[3] = {*p, *(p + 1), 0};
					osec_emit("%d, ",
						(char)strtol(buf, NULL, 16));
					p += 2;
				}
				osec_emit("};");
				osec_eol(osevm);
				osec_emit("ose_pushBlob(vm_s, buf);");
				osec_eol(osevm);
				osec_emit("}");
				osec_eol(osevm);
			}
		}else{
			osec_emit("osevm_toBlob(osevm);");
			osec_eol(osevm);
		}
	}
}

void osec_appendByte(ose_bundle osevm, char *address)
{
}

void osec_default(ose_bundle osevm, char *address)
{
}

void osec_preInput(ose_bundle osevm)
{
	if(OSEC_GETPASS(osevm) == 1){
		osec_writeIncludes(osevm);
		osec_beginFunctionTableMacro(osevm);
	}
}

void osec_postInput(ose_bundle osevm)
{
	if(OSEC_GETPASS(osevm) == 1){
		osec_endFunctionTableMacro(osevm);
		osec_emit("\n");
		osec_writeFunctionDefinitions(osevm);
		osec_writeTables(osevm);
	}
	OSEC_SETPASS(osevm, 0);
}

void osec_preControl(ose_bundle osevm)
{
}

void osec_postControl(ose_bundle osevm)
{
}

#define OSEC_LANG_C	1
#define OSEC_LANG_JS	2

int main(int ac, char **av)
{
	char *input_filename = NULL;
	//int lang = OSEC_LANG_C;
	char *output_filename = "out.c";
	// process args
	for(int i = 1; i < ac; i++){
		if(!strcmp(av[i], "--verbose")){
			verbose = 1;
		}else if(!strcmp(av[i], "-i")){
			input_filename = av[++i];
		}else if(!strcmp(av[i], "-o")){
			output_filename = av[++i];
		}else if(!strcmp(av[i], "-l")){
			// language
		}else{
			fprintf(stderr, "unrecognized option %s\n", av[i]);
			return 0;
		}
	}

	// set up ose environment and vm
	bytes = (char *)malloc(MAX_BUNDLE_LEN);
	memset(bytes, 0, MAX_BUNDLE_LEN);
	bundle = ose_newBundleFromCBytes(MAX_BUNDLE_LEN, bytes);
	osevm = osevm_init(bundle);/* , */
			   /* 1000000, */
			   /* 1000000, */
			   /* 1000000, */
			   /* 1000000, */
			   /* 1000000, */
			   /* 1000000); */
	vm_i = OSEVM_INPUT(osevm);
	vm_s = OSEVM_STACK(osevm);
	vm_e = OSEVM_ENV(osevm);
	vm_c = OSEVM_CONTROL(osevm);
        vm_d = OSEVM_DUMP(osevm);
	vm_o = OSEVM_OUTPUT(osevm);

	ose_readFile(vm_s, input_filename);

	
	// pass 1
	OSEC_SETSCOPE(osevm, OSEC_SCOPE_GLOBAL);
	OSEC_SETPASS(osevm, 1);
	ose_copyBundle(vm_s, vm_i);
	ose_popAllDrop(vm_i);
	osevm_run(osevm);
	//osec_startMain(osevm);

	// pass 2
	OSEC_SETSCOPE(osevm, OSEC_SCOPE_GLOBAL);
	OSEC_SETPASS(osevm, 2);
	ose_copyBundle(vm_s, vm_i);
	ose_popAllDrop(vm_i);
	osevm_run(osevm);
	
	osec_startMain(osevm);
	
	// pass 3
	OSEC_SETSCOPE(osevm, OSEC_SCOPE_GLOBAL);
	OSEC_SETPASS(osevm, 3);
	OSEC_SET_ANONFNCT(osevm, 0);
	ose_copyBundle(vm_s, vm_i);
	ose_popAllDrop(vm_i);
	osevm_run(osevm);

	osec_finishMain(osevm);
	return 0;
}
