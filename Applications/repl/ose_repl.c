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
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <editline/readline.h>

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_symtab.h"
#include "ose_print.h"
#include "sys/ose_load.h"
#include "sys/ose_time.h"
#include "ose_version.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#ifndef MAX_BUNDLE_LEN
#define MAX_BUNDLE_LEN 1000000
#endif

/**********************************************************************
 * Global vars
 **********************************************************************/

// Data and pointers for the main bundle and vm
static char *bytes;
static ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o;

// REPL options
static int or_verbose = 0;
static int or_step = 0;
static char *or_prefix = NULL;
static int32_t or_prefixlen = 0;
static int or_udp_input = 0;
static int or_udp_output = 0;

// Signaling and control
static jmp_buf or_jmp_buf;
static volatile sig_atomic_t or_assertion_failed = 0;
static volatile sig_atomic_t or_quit = 0;
static volatile int or_have_input = 0;

// If we have access to a high precision timer, 
// we use it to time the run() operation
#ifdef OSE_HAVE_HPTIMER
static ose_hptimer or_hptimer;
#endif

static char *or_prompt_normal = "# ";
static char *or_prompt_compile = "(compile)# ";
static char *or_prompt = "# ";

// UDP I/O
static int or_udp_socket_input, or_udp_socket_output;
static struct sockaddr_in or_udp_sockaddr_in_input;
static struct sockaddr_in or_udp_sockaddr_in_output;
static const char *or_udp_addrstr_output = "127.0.0.1";
static uint16_t or_udp_port_input = 10053;
static uint16_t or_udp_port_output = 10073;

/**********************************************************************
 * Forward decls
 **********************************************************************/

// REPL builtins accessable with /ose messages
static void oserepl_import(ose_bundle osevm);
static void oserepl_step(ose_bundle bundle);
static void oserepl_verbose(ose_bundle bundle);
static void oserepl_printContextBundle(ose_bundle bundle);
static void oserepl_udpport(ose_bundle bundle);
static void oserepl_udpaddr(ose_bundle bundle);
static void oserepl_setPrefix(ose_bundle bundle);

static struct or_fn
{
	char *sym;
	ose_fn fn;
} or_fns[] = {{"/import", oserepl_import},
	      {"/step", oserepl_step},
	      {"/verbose", oserepl_verbose},
	      {"/print", oserepl_printContextBundle},
	      {"/udp/input/port/get", oserepl_udpport},
	      {"/udp/input/port/set", oserepl_udpport},
	      {"/udp/output/port/get", oserepl_udpport},
	      {"/udp/output/port/set", oserepl_udpport},
	      {"/udp/output/addr/get", oserepl_udpaddr},
	      {"/udp/output/addr/set", oserepl_udpaddr},
	      {"/setprefix", oserepl_setPrefix},
};

// editline / readline
static void oserepl_rl_compgen_addContextBundleSfxs(const char * const text,
						    const int textlen,
						    const char * const pfx,
						    const int32_t pfxlen,
						    const char * const sym,
						    const char **pfxs,
						    const char **matches,
						    const char **sfxs,
						    int32_t *nmatches);
static char *oserepl_rl_compgen(const char *text, int state);
static char **oserepl_rl_compcb(const char *text, int start, int end);
static void oserepl_rl_cb(char *line);

// UDP I/O
static int oserepl_udp_sock(const char * const addr,
			    uint16_t port,
			    struct sockaddr_in *sa);
static int oserepl_udp_send(ose_bundle bundle);

// REPL
static void oserepl_sigHandler(int signo);
static uint64_t oserepl_run(void);
static void oserepl_init(void);
int main(int ac, char **av);

/**********************************************************************
 * Implementation
 **********************************************************************/

// REPL hooks
void oserepl_preInput(ose_bundle osevm)
{
}

void oserepl_postInput(ose_bundle osevm)
{
}

void oserepl_preControl(ose_bundle osevm)
{
}

void oserepl_postControl(ose_bundle osevm)
{
}

void oserepl_defun(ose_bundle osevm, char *address)
{
	rl_set_prompt(or_prompt_compile);
	osevm_defun(osevm, address);
}

void oserepl_endDefun(ose_bundle osevm, char *address)
{
	rl_set_prompt(or_prompt_normal);
	osevm_endDefun(osevm, address);
}

void oserepl_default(ose_bundle osevm, char *address)
{
	const int32_t len = strlen(address);
	if(!strncmp(address, or_prefix, or_prefixlen)){
		if(len == or_prefixlen){
			return;
		}
		for(int i = 0;
		    i < sizeof(or_fns) / sizeof(struct or_fn);
		    i++){
			if(!strncmp(address + or_prefixlen,
				    or_fns[i].sym,
				    strlen(or_fns[i].sym))){
				or_fns[i].fn(osevm);
				return;
			}
		}
	}
	osevm_default(osevm, address);
}

int oserepl_isKnownAddress(const char * const address)
{
	if(!strncmp(address, or_prefix, or_prefixlen)){
		return 1;
	}else{
		return 0;
	}
}

// editline / readline
static void oserepl_rl_compgen_addContextBundleSfxs(const char * const text,
						    const int textlen,
						    const char * const pfx,
						    const int32_t pfxlen,
						    const char * const sym,
						    const char **pfxs,
						    const char **matches,
						    const char **sfxs,
						    int32_t *nmatches)
{
	const int32_t s = ose_readInt32(osevm, -4);
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	int32_t symlen = strlen(sym);
	while(o < s){
		const int32_t ss =
			ose_readInt32(osevm, o);
		const char * const addr =
			ose_readString(osevm, o + 4);
		pfxs[*nmatches] = pfx;
		matches[*nmatches] = sym;
		sfxs[*nmatches] = addr;
		(*nmatches)++;
		o += ss + 4;
	}
}

static char *oserepl_rl_compgen(const char *text, int state)
{
	int symtablen = ose_symtab_len();
	static const char *matches[256];
	static const char *pfxs[256];
	static const char *sfxs[256];
	static int nmatches;
	static int curmatch;

	if(state == 0){
		nmatches = 0;
		curmatch = 0;
		int len = strlen(text);
		if(*text != '/'){
			return NULL;
		}
		if(!strncmp(text, "/!", 2)){
			for(int i = 0; i < symtablen; i++){
				char *sym = ose_symtab_getNthSym(i);
				if(!strncmp(text + 2, sym, len - 2)){
					pfxs[nmatches] = "/!";
					matches[nmatches] = sym;
					sfxs[nmatches] = NULL;
					nmatches++;
				}
			}
		}
		if(!strncmp(text, "/$", 2)){
			int32_t o = OSE_BUNDLE_HEADER_LEN;
			int32_t s = ose_readInt32(vm_e, -4);
			while(o < s){
				int32_t ss = ose_readInt32(vm_e, o);
				char *addy = ose_readString(vm_e, o + 4);
				if(!strncmp(text + 2, addy, len - 2)){
					pfxs[nmatches] = "/$";
					matches[nmatches] = addy;
					sfxs[nmatches] = NULL;
					nmatches++;
				}
				o += ss + 4;
			}
		}
		if(!strncmp(text, or_prefix, len)){
			pfxs[nmatches] = or_prefix;
			matches[nmatches] = NULL;
			sfxs[nmatches] = NULL;
			nmatches++;
		}
		if(!strncmp(text, or_prefix, or_prefixlen)){
			for(int i = 0;
			    i < sizeof(or_fns) / sizeof(struct or_fn);
			    i++){
				if(!strncmp(text + or_prefixlen,
					    or_fns[i].sym,
					    len - or_prefixlen)){
					pfxs[nmatches] = or_prefix;
					matches[nmatches] = or_fns[i].sym;
					sfxs[nmatches] = NULL;
					nmatches++;
				}
			}
			if(!strncmp(text + or_prefixlen,
				    "/print",
				    6)){
				oserepl_rl_compgen_addContextBundleSfxs(text,
									len,
									or_prefix,
									or_prefixlen,
									"/print",
									pfxs,
									matches,
									sfxs,
									&nmatches);
			}
		}
		// builtins that take a fixed set of suffixes
		if(!strncmp(text, "/>", 2)
		   || !strncmp(text, "/>/", 3)){
			oserepl_rl_compgen_addContextBundleSfxs(text,
								len,
								NULL,
								0,
								"/>",
								pfxs,
								matches,
								sfxs,
								&nmatches);
		}else if(!strncmp(text, "/<", 2)){
			if(text[2] == '/'){
				oserepl_rl_compgen_addContextBundleSfxs(text,
									len,
									NULL,
									0,
									"/<",
									pfxs,
									matches,
									sfxs,
									&nmatches);
			}else if(text[2] == 0){
				oserepl_rl_compgen_addContextBundleSfxs(text,
									len,
									NULL,
									0,
									"/<",
									pfxs,
									matches,
									sfxs,
									&nmatches);
				oserepl_rl_compgen_addContextBundleSfxs(text,
									len,
									NULL,
									0,
									"/<<",
									pfxs,
									matches,
									sfxs,
									&nmatches);
			}else if(text[2] == '<'){
				oserepl_rl_compgen_addContextBundleSfxs(text,
									len,
									NULL,
									0,
									"/<<",
									pfxs,
									matches,
									sfxs,
									&nmatches);
			}
		}else if(!strncmp(text, "/-", 2)
			 || !strncmp(text, "/-/", 3)){
			oserepl_rl_compgen_addContextBundleSfxs(text,
								len,
								NULL,
								0,
								"/-",
								pfxs,
								matches,
								sfxs,
								&nmatches);
		}

	}
	if(curmatch == nmatches){
		return NULL;
	}else{
		const char * const pfx = pfxs[curmatch];
		const char * const sym = matches[curmatch];
		const char * const sfx = sfxs[curmatch];
		curmatch++;
		const int len = (pfx ? strlen(pfx) : 0)
			+ (sym ? strlen(sym) : 0)
			+ (sfx ? strlen(sfx) : 0);
		char *buf = malloc(len + 1);
		memset(buf, 0, len + 1);
		char *bufp = buf;
		if(pfx){
			bufp += sprintf(bufp, "%s", pfx);
		}
		if(sym){
			bufp += sprintf(bufp, "%s", sym);
		}
		if(sfx){
			bufp += sprintf(bufp, "%s", sfx);
		}
		return buf;
	}
}

static char **oserepl_rl_compcb(const char *text, int start, int end)
{
	// Don't do filename completion even if our generator finds no matches.
	// rl_attempted_completion_over = 1;

	// Note: returning nullptr here will make readline use the default
	// filename oserepl_rl_compcb.
	return rl_completion_matches(text, oserepl_rl_compgen);
}

static void oserepl_rl_cb(char *line)
{
	if(!line){
		return;
	}
	or_have_input = 1;
	
	int len = strlen(line);
	if(strlen(line) <= 0){
		return;
	}
	
	const char *linep = line;
	int len_trimmed = len;
	// eat leading whitespace
	while(linep - line < len
	      && (*linep  == ' '
		  || *linep == '\t')){
		linep++;
		len_trimmed--;
	}

	add_history(line);
	ose_pushMessage(vm_i, line, strlen(line), 0);
	free(line);
}

// REPL builtin functions accessable with /ose messages

static void oserepl_import(ose_bundle bundle)
{
	const char * const path = ose_peekString(vm_s);
	const int32_t pathlen = strlen(path);
	if(!strcmp(path + (pathlen - 4), ".ose")){
		ose_readFile(vm_s, path);
		ose_rollBottom(vm_s);
		ose_drop(vm_s);
		ose_bundleAll(vm_s);
		ose_pushMessage(vm_i,
				"/!/exec", strlen("/!/exec"), 0);
	}else{
		ose_try{
			ose_loadLib(osevm, path);
		}ose_catch(1){
			fprintf(stderr, "couldn't open %s\n",
				path);
		}ose_end_try;
		fprintf(stdout, "---loaded %s successfully---\n", path);
	}
}

static void oserepl_step(ose_bundle bundle)
{
	or_step = (or_step == 0);
	fprintf(stdout, "---stepping %s---\n", or_step ? "enabled" : "disabled");
}

static void oserepl_verbose(ose_bundle bundle)
{
	or_verbose = (or_verbose == 0);
	fprintf(stdout, "---verbose %s---\n", or_verbose ? "on" : "off");
}

static void oserepl_printContextBundle(ose_bundle osevm)
{
	const char * const address = ose_peekAddress(vm_c);
	ose_bundle bundle;
	if(address[or_prefixlen + 6] == '/'){
		bundle = ose_enter(osevm, address + or_prefixlen + 6);
	}else{
		bundle = ose_enter(osevm, ose_peekString(vm_s));
		ose_drop(vm_s);
	}
	ose_pprintFullBundle(bundle, vm_s, NULL);
	printf("%s\n", ose_peekString(vm_s));
	ose_drop(vm_s);
	fflush(stdout);
}

static void oserepl_udpport(ose_bundle bundle)
{
	const char * const addr = ose_peekAddress(vm_c);
	const int32_t addrlen = strlen(addr);
	if(!strncmp(addr + or_prefixlen + 4, "/input", 6)){
		if(!strncmp(addr + or_prefixlen + 4 + 6 + 5, "/set", 4)){
			or_udp_port_input = ose_popInt32(vm_s);
			if(or_udp_socket_input){
				close(or_udp_socket_input);
				or_udp_socket_input = oserepl_udp_sock("0.0.0.0",
								 or_udp_port_input,
								 &or_udp_sockaddr_in_input);
			}
		}else{
			ose_pushInt32(vm_s, or_udp_port_input);
		}
	}else if(!strncmp(addr + or_prefixlen + 4, "/output", 7)){
		if(!strncmp(addr + or_prefixlen + 4 + 7 + 5, "/set", 4)){
			or_udp_port_output = ose_popInt32(vm_s);
			if(or_udp_socket_output){
				close(or_udp_socket_output);
				or_udp_socket_output = oserepl_udp_sock(or_udp_addrstr_output,
								  or_udp_port_output,
								  &or_udp_sockaddr_in_output);
			}
		}else{
			ose_pushInt32(vm_s, or_udp_port_output);
		}
	}else if(addrlen > or_prefixlen + 5){
		ose_assert(0 && "ose port must be either /input or /output");
	}else{
		ose_assert(0 && "ose port must be either /input or /output");
	}
}

static void oserepl_udpaddr(ose_bundle bundle)
{
	// const char * const addr = ose_peekAddress(vm_c);
	// const int32_t addrlen = strlen(addr);
	const char * const addr = ose_peekString(vm_s);
	if(or_udp_socket_output){
		close(or_udp_socket_output);
	}
	or_udp_addrstr_output = addr;
	or_udp_socket_output = oserepl_udp_sock(or_udp_addrstr_output,
					  or_udp_port_output,
					  &or_udp_sockaddr_in_output);
	ose_drop(vm_s);
}

static void oserepl_setPrefix(ose_bundle bundle)
{
	if(ose_peekType(vm_s) == OSETT_MESSAGE
	   && ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
		free(or_prefix);
		or_prefix = strdup(ose_peekString(vm_s));
		or_prefixlen = strlen(or_prefix);
		ose_drop(vm_s);
	}else{
		ose_assert(0 && "prefix must be a string");
	}
}

// UDP I/O
static int oserepl_udp_sock(const char * const addr,
			    uint16_t port,
			    struct sockaddr_in *sa)
{
	memset(sa, 0, sizeof(struct sockaddr_in));
	sa->sin_family = AF_INET;
	sa->sin_addr.s_addr = inet_addr(addr);
	sa->sin_port = htons(port);
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	bind(sock, (struct sockaddr*)sa, sizeof(struct sockaddr_in));
	return sock;
}

static int oserepl_udp_send(ose_bundle bundle)
{
	int r = sendto(or_udp_socket_output,
		       ose_getBundlePtr(bundle),
		       ose_readInt32(bundle, -4),
		       0,
		       (struct sockaddr *)&or_udp_sockaddr_in_output,
		       sizeof(struct sockaddr_in));
	return r;
}

// REPL
static void oserepl_sigHandler(int signo)
{
	switch(signo){
	case SIGINT:
		or_quit++;
		if(or_quit > 1){
			exit(0);
		}
		siglongjmp(or_jmp_buf, 2);
	case SIGABRT:
		or_have_input = 0;
		or_assertion_failed = 1;
		siglongjmp(or_jmp_buf, 1);
	}
}

static void oserepl_init(void)
{
	memset(bytes, 0, MAX_BUNDLE_LEN);
	bundle = ose_newBundleFromCBytes(MAX_BUNDLE_LEN, bytes);
	osevm = osevm_init(bundle);
	vm_i = OSEVM_INPUT(osevm);
	vm_s = OSEVM_STACK(osevm);
	vm_e = OSEVM_ENV(osevm);
	vm_c = OSEVM_CONTROL(osevm);
        vm_d = OSEVM_DUMP(osevm);
	vm_o = OSEVM_OUTPUT(osevm);
}

static uint64_t oserepl_run(void)
{
	int32_t stepnum = 0;
	char t = OSETT_TRUE;
#ifdef OSE_HAVE_HPTIMER
	uint64_t here = ose_now(or_hptimer);
#endif
	while((t = osevm_step(osevm)) == OSETT_TRUE){
		stepnum++;
		if(or_step){
			ose_pprintFullBundle(vm_c, vm_s, "Control");
			printf("%s\n", ose_peekString(vm_s));
			ose_drop(vm_s);
			break;
		}
	}
#ifdef OSE_HAVE_HPTIMER
	uint64_t there = ose_now(or_hptimer);
	here = ose_timeToMonotonicNanos(or_hptimer, here);
	there = ose_timeToMonotonicNanos(or_hptimer, there);
	return there - here;
#else
	return 0;
#endif
}

int main(int ac, char **av)
{
	or_prefix = strdup("/ose");
	or_prefixlen = 4;
	{
		// process args
		for(int i = 1; i < ac; i++){
			if(!strcmp(av[i], "--verbose")){
				or_verbose = 1;
			}else if(!strcmp(av[i], "--input-port")){
				uint16_t ip = (uint16_t)strtol(av[i + 1], NULL, 10);
				if(ip <= 1024 || ip > 65535){
					fprintf(stderr,
						"input port out of range [1025,65535] (%d)\n",
						ip);
					fprintf(stderr,
						"using default value: %d\n",
						or_udp_port_input);
				}else{
					or_udp_port_input = ip;
				}
				i++;
			}else if(!strcmp(av[i], "--output-port")){
				uint16_t op = (uint16_t)strtol(av[i + 1], NULL, 10);
				if(op <= 1024 || op > 65535){
					fprintf(stderr,
						"output port out of range [1025,65535] (%d)\n",
						op);
					fprintf(stderr,
						"using default value: %d\n",
						or_udp_port_output);
				}else{
					or_udp_port_output = op;
				}
				i++;
			}else if(!strcmp(av[i], "--output-address")){
				or_udp_addrstr_output = av[i + 1];
				i++;
			}else if(!strcmp(av[i], "--udp-output")){
				or_udp_output = 1;
			}else if(!strcmp(av[i], "--udp-input")){
				or_udp_input = 1;
			}else if(!strcmp(av[i], "--prefix")){
				or_prefix = av[i + 1];
				or_prefixlen = strlen(or_prefix);
				i++;
			}
		}
	}
	{
		printf("Ose %s\n", OSE_VERSION);
		printf("\n");
		if(or_udp_input){
			printf("Receiving on UDP port %d\n", or_udp_port_input);
		}else{
			printf("UDP input disabled (enable with /ose/input/udp/on)\n");
		}
		if(or_udp_output){
			printf("Sending to %s:%d\n", or_udp_addrstr_output, or_udp_port_output);
		}else{
			printf("UDP output disabled (enable with /ose/output/udp/on)\n");
		}
		printf("\n");
	}

	// install signal handler
	if(signal(SIGINT, oserepl_sigHandler) == SIG_ERR){
		fprintf(stderr,
			"error installing signal handler to catch SIGINT\n");
		return 0;
	}
	if(signal(SIGABRT, oserepl_sigHandler) == SIG_ERR){
		fprintf(stderr,
			"error installing signal handler to catch SIGABRT\n");
		return 0;
	}

	// history
	const char *homedir = getenv("HOME");
	char histfile[strlen(homedir) + 6];
	sprintf(histfile, "%s/.ose", homedir);
	int r = read_history(histfile);
	if(r){
		fprintf(stderr,
			"ose: couldn't read history file: "
			"read_history(%s) returned %d\n",
			histfile, r);
	}

	// set up ose environment and vm
	bytes = (char *)malloc(MAX_BUNDLE_LEN);
	oserepl_init();

	// libedit
	rl_callback_handler_install(or_prompt, oserepl_rl_cb);
	rl_attempted_completion_function = oserepl_rl_compcb;
	//rl_basic_word_break_characters = " \t\n\"\\'`@><=;|&{(";
	rl_basic_word_break_characters = " \t\n\"\\'`@=;|&{(";
	rl_completion_append_character = 0;

	or_udp_socket_input = oserepl_udp_sock("0.0.0.0",
						    or_udp_port_input,
						    &or_udp_sockaddr_in_input);
	or_udp_socket_output = oserepl_udp_sock(or_udp_addrstr_output,
						     or_udp_port_output,
						     &or_udp_sockaddr_in_output);

	fd_set rset;
	FD_ZERO(&rset);
	int maxfdp1 = or_udp_socket_input + 1;

	or_hptimer = ose_initTimer();

	while(1){
		switch(sigsetjmp(or_jmp_buf, 1)){
		case 1: {
			if(or_assertion_failed){
				or_assertion_failed = 0;
				oserepl_init();
				fputs("\r  \r", stdout);
				rl_on_new_line();
				rl_forced_update_display();
			}
		}
			break;
		case 2: {
			goto cleanup_and_quit;
		}
			break;
		}

		if(or_udp_input){
			FD_SET(or_udp_socket_input, &rset);
			maxfdp1 = or_udp_socket_input + 1;
		}else{
			maxfdp1 = STDIN_FILENO + 1;
		}
		FD_SET(STDIN_FILENO, &rset);
		int nselect = select(maxfdp1, &rset, NULL, NULL, NULL);
		if(nselect <= 0){
			continue;
		}
		//////////////////////////////////////////////////
		// read
		//////////////////////////////////////////////////
		if(FD_ISSET(or_udp_socket_input, &rset)){
			struct sockaddr_in ca;
			socklen_t ca_len = sizeof(struct sockaddr_in);;
			memset(&ca, 0, sizeof(struct sockaddr_in));
			char buf[65536];
			int len = recvfrom(or_udp_socket_input,
					   buf, 1024, 0,
					   (struct sockaddr *)&ca,
					   &ca_len);
			if(len > 0){
				ose_pushBlob(vm_i, len, buf);
				ose_blobToElem(vm_i);
				ose_popAllDrop(vm_i);
				or_have_input = 1;
				{
					// announce that we received a bundle
					// over udp
					fputs("\r  \r", stdout);
					uint32_t a = ntohl(ca.sin_addr.s_addr);
					printf("Received %d byte bundle from "
					       "%d.%d.%d.%d:%d\n",
					       len,
					       (a & 0xFF000000) >> 24,
					       (a & 0x00FF0000) >> 16,
					       (a & 0x0000FF00) >> 8,
					       a & 0x000000FF,
					       or_udp_port_input);
				}
			}
		}

		if(FD_ISSET(STDIN_FILENO, &rset)){
			rl_callback_read_char();
		}

		int should_print = 0;
		//////////////////////////////////////////////////
		// eval
		//////////////////////////////////////////////////
		uint64_t elapsed_time = 0;
		if(or_have_input){
			fputs("\r                \r", stdout);
			elapsed_time = oserepl_run();
			should_print = 1;
		}

		//////////////////////////////////////////////////
		// print
		//////////////////////////////////////////////////
		if(or_udp_port_output && should_print){
			oserepl_udp_send(vm_o);
		}
		ose_clear(vm_o);
		
		if(should_print){
			printf(ANSI_COLOR_CYAN);
			ose_pprintFullBundle(vm_s, vm_s, "Stack");
			printf("%s\n", ose_peekString(vm_s));
			ose_drop(vm_s);
			if(or_verbose){
				int32_t s = ose_readInt32(vm_s, -4);
				int32_t sa = ose_spaceAvailable(vm_s);
				printf(ANSI_COLOR_YELLOW);
				printf("\n");
				printf("%d%% used, %d bytes free\n",
				       (int32_t)(s / (s + sa)), sa);
#ifdef OSE_HAVE_HPTIMER
				printf("Elapsed time: %lluns, %fms, %fs\n",
				       elapsed_time,
				       (double)elapsed_time / 1000000.,
				       (double)elapsed_time / 1000000000.);
#endif
			}
			printf(ANSI_COLOR_RESET);
			or_have_input = 0;
			rl_on_new_line();
			rl_forced_update_display();
		}
	}
 cleanup_and_quit:
	r = write_history(histfile);
	if(r){
		fprintf(stderr,
			"ose: couldn't write history file: "
			"write_history(%s) returned %d\n",
			histfile, r);
	}
	return 0;
}
