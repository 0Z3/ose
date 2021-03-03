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
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include "linenoise.h"

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

/* Data and pointers for the main bundle and vm */
static char *bytes;
static ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o;

/* REPL options */
static int or_verbose = 0;
static int or_step = 0;
static char *or_prefix = NULL;
static int32_t or_prefixlen = 0;

/* Signaling and control */
static jmp_buf or_jmp_buf;
#if __STDC_VERSION__ < 199901L
/* There seems to not be a portable way to get the behavior of sigsetjmpp
   on C89 systems. We can check some of them and set the proper flags,
   but for now, we just hope for the best */
#define sigsetjmp(jb, i) setjmp(jb)
#define siglongjmp(jb, i) longjmp(jb, 1)
#endif
static volatile sig_atomic_t or_assertion_failed = 0;
static volatile sig_atomic_t or_quit = 0;
static volatile int or_have_input = 0;
static char *or_input_text = NULL;
struct linenoiseState *or_linenoise_state = NULL;

/* If we have access to a high precision timer,  */
/* we use it to time the run() operation */
#ifdef OSE_HAVE_HPTIMER
static ose_hptimer or_hptimer;
#endif

static const char *or_prompt_normal = "# ";
static const char *or_prompt_compile = "(compile)# ";
static const char *or_prompt = "# ";

/* UDP I/O */
struct or_udp_input
{
	int sock;
	struct sockaddr_in sa;
	uint16_t port;
};
static struct or_udp_input or_udp_input_srcs[32];
static int or_udp_input_src_count = 0;

static int or_udp_have_sender_addr = 0;
static char or_udp_sender_addr[16] = {0};
struct or_udp_output
{
	char *ipaddr;
	uint16_t port;
};
static struct or_udp_output or_udp_output_dests[32];
static int or_udp_output_dest_count = 0;

/**********************************************************************
 * Forward decls
 **********************************************************************/

/* REPL builtins accessable with /ose messages */
static void oserepl_import(ose_bundle osevm);
static void oserepl_prefix(ose_bundle bundle);
static void oserepl_print(ose_bundle bundle);
static void oserepl_step(ose_bundle bundle);
static void oserepl_udp_input(ose_bundle osevm);
static void oserepl_udp_output(ose_bundle osevm);
static void oserepl_verbose(ose_bundle bundle);
static void oserepl_version(ose_bundle bundle);

static struct or_fn
{
	char *sym;
	ose_fn fn;
} or_fns[] = {{"/import", oserepl_import},
	      {"/prefix", oserepl_prefix},
	      {"/print", oserepl_print},
	      {"/step", oserepl_step},
	      {"/udp/input", oserepl_udp_input},
	      {"/udp/output", oserepl_udp_output},
	      {"/verbose", oserepl_verbose},
	      {"/version", oserepl_version},
};

static struct or_opt *oserepl_getoptrec(const char * const opt);

static void oserepl_opt_printHelp(struct or_opt o);
static int oserepl_opt_help(int argnum, char **av);
static int oserepl_opt_prefix(int argnum, char **av);
static int oserepl_opt_udp_input(int argnum, char **av);
static int oserepl_opt_udp_output(int argnum, char **av);
static int oserepl_opt_verbose(int argnum, char **av);
static int oserepl_opt_version(int argnum, char **av);

struct or_opt_ex
{
	const char *example;
	const char *description;
};

static struct or_opt
{
	const char *option;
	int (*fn)(int ac, char **av);
	const char *syntax;
	const char *description;
	struct or_opt_ex exs[8];
} or_opts[] = {{"--help",
		oserepl_opt_help,
		"--help",
		"Print this info and exit.",
		{}},
	       {"--prefix",
		oserepl_opt_prefix,
		"--prefix=<prefix>",
		"Set the prefix for messages directed to the REPL (default: /ose).",
		{{"--prefix=/esp32/1",
		  "Set the prefix to \"/esp32/1\""}}},
	       {"--udp-input",
		oserepl_opt_udp_input,
		"--udp-input=<port>[,<port>...]",
		"Set the UDP input port(s) to receive on.",
		{{"--udp-input=12345", "Listen for input on port 12345."},
		 {"--udp-input=12345,54321",
		  "Listen for input on ports 12345 and 54321."}}},
	       {"--udp-output",
		oserepl_opt_udp_output,
		"--udp-output=[ipaddress]:<port>[,[ipaddress]:<port>...]",
		"Set the UDP output address(es) and port(s) to send to.",
		{{"--udp-output=127.0.0.1:12345",
		  "Send output to localhost port 12345"},
		 {"--udp-output=:12345",
		  "If input was received via UDP, reply to sender on port 12345"},
		 {"--udp-output=192.168.178.22:12345,:54321,127.0.0.1:55555",
		  "Send output to three locations, leftmost=first, rightmost=last."}}},
	       {"--verbose",
		oserepl_opt_verbose,
		"--verbose",
		"Turn on verbose output.",
		{}},
	       {"--version",
		oserepl_opt_version,
		"--version",
		"Print the version and exit.",
		{}},
};

/* UDP I/O */
static int oserepl_udp_sock(const char * const addr,
			    uint16_t port,
			    struct sockaddr_in *sa);
static int oserepl_udp_send(ose_bundle bundle);
static int oserepl_udp_openInputPorts(void);

/* REPL */
static void oserepl_sigHandler(int signo);
static uint64_t oserepl_run(void);
static void oserepl_init(void);
int main(int ac, char **av);

/**********************************************************************
 * Implementation
 **********************************************************************/

/* REPL hooks */
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
	// change to compile prompt
	osevm_defun(osevm, address);
}

void oserepl_endDefun(ose_bundle osevm, char *address)
{
	// change from compile prompt back to normal
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
	}else if(!strcmp(address, "/prefix")){
		oserepl_prefix(osevm);
		ose_drop(vm_c);
		return;
	}
	osevm_default(osevm, address);
}

int oserepl_isKnownAddress(const char * const address)
{
	if(!strncmp(address, or_prefix, or_prefixlen)
	   || !strcmp(address, "/prefix")){
		return 1;
	}else{
		return 0;
	}
}

/* Linenoise callbacks */

void oserepl_linenoise_cb(struct linenoiseState *l, char *line, int len)
{
	if(!line){
		or_input_text = NULL;
		return;
	}
	linenoiseHistoryAdd(line);
	ose_pushMessage(vm_i, line, strlen(line), 0);
	free(line);
	or_have_input = 1;
}

void oserepl_linenoise_completion_cb(const char *str, linenoiseCompletions *c)
{
	size_t len = strlen(str);
	char buf[128];
	if(!strncmp(str, "/!", 2)){
		int symtablen = ose_symtab_len();
		for(int i = 0; i < symtablen; i++){
			char *sym = ose_symtab_getNthSym(i);
			if(!strncmp(str + 2, sym, len - 2)){
				snprintf(buf, 128, "/!%s", sym);
				linenoiseAddCompletion(c, buf);
			}
		}
	}else if(!strncmp(str, "/$", 2)){
		int32_t o = OSE_BUNDLE_HEADER_LEN;
		int32_t s = ose_readInt32(vm_e, -4);
		while(o < s){
			int32_t ss = ose_readInt32(vm_e, o);
			char *addy = ose_readString(vm_e, o + 4);
			if(!strncmp(str + 2, addy, len - 2)){
				snprintf(buf, 128, "/$%s", addy);
				linenoiseAddCompletion(c, buf);
			}
			o += ss + 4;
		}
	}
}

/* REPL builtin functions accessable with /ose messages */

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

static void oserepl_print(ose_bundle osevm)
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

static void oserepl_udp_input(ose_bundle osevm)
{
	int32_t n = ose_getBundleElemCount(vm_s);
	int i = 0;
	if(n == 0){
		for(i = 0; i < or_udp_input_src_count; i++){
			ose_pushInt32(vm_s,
				      or_udp_input_srcs[i].port);
		}
	}else{
		ose_bundleAll(vm_s);
		ose_popAllDrop(vm_s);
		while(or_udp_input_src_count){
			struct or_udp_input *s =
				&(or_udp_input_srcs[or_udp_input_src_count - 1]);
			close(s->sock);
			or_udp_input_src_count--;
		}
		for(i = 0; i < n; i++){
			struct or_udp_input *s =
				&(or_udp_input_srcs[i]);
			s->port = ose_popInt32(vm_s);
			or_udp_input_src_count++;
		}
		oserepl_udp_openInputPorts();
	}
}

static void oserepl_udp_output(ose_bundle osevm)
{
	int32_t n = ose_getBundleElemCount(vm_s);
	int i = 0;
	if(n == 0){
		for(i = 0; i < or_udp_output_dest_count; i++){
			ose_pushBundle(vm_s);
			ose_pushMessage(vm_s, "/addr", 5, 1,
					OSETT_STRING,
					or_udp_output_dests[i].ipaddr);
			ose_push(vm_s);
			ose_pushMessage(vm_s, "/port", 5, 1,
					OSETT_INT32,
					or_udp_output_dests[i].port);
			ose_push(vm_s);
		}
	}else{
		ose_bundleAll(vm_s);
		ose_popAllDrop(vm_s);
		or_udp_output_dest_count = 0;
		for(i = 0; i < n; i++){
			if(or_udp_output_dests[i].ipaddr){
				free(or_udp_output_dests[i].ipaddr);
			}
			char *addr = NULL;
			int32_t port = -1;
			int j = 0;
			int nn = ose_getBundleElemElemCount(vm_s,
							    ose_getLastBundleElemOffset(vm_s));
			for(j = 0; j < nn; j++){
				ose_pop(vm_s);
				const char * const msgaddr = ose_peekAddress(vm_s);
				if(!strcmp(msgaddr, "/addr")){
					char *a = ose_peekString(vm_s);
					if(!strlen(a)){
						;
					}else{
						addr = strdup(a);
					}
					ose_drop(vm_s);
				}else if(!strcmp(msgaddr, "/port")){
					port = (uint16_t)ose_peekInt32(vm_s);
					ose_drop(vm_s);
				}else{
					ose_assert(0, "/udp/output must be specified as one or more bundles containing the messages \"/addr\" : <ipaddr (string)> and \"/port\" : <port (int32)");
				}
			}
			if(port < 0){
				ose_assert(0, "/udp/output must be specified as one or more bundles containing the messages \"/addr\" : <ipaddr (string)> and \"/port\" : <port (int32)");
			}
			ose_drop(vm_s);
			or_udp_output_dests[i].ipaddr = addr;
			or_udp_output_dests[i].port = port;
			or_udp_output_dest_count = i + 1;
		}
	}
}

static void oserepl_prefix(ose_bundle bundle)
{
	if(ose_getBundleElemCount(vm_s) == 0){
		ose_pushString(vm_s, or_prefix);
	}else{
		if(ose_peekType(vm_s) == OSETT_MESSAGE
		   && ose_isStringType(ose_peekMessageArgType(vm_s)) == OSETT_TRUE){
			free(or_prefix);
			or_prefix = strdup(ose_peekString(vm_s));
			or_prefixlen = strlen(or_prefix);
			ose_drop(vm_s);
		}else{
			ose_assert(0, "prefix must be a string");
		}	
	}
}

static void oserepl_version(ose_bundle bundle)
{
	ose_pushString(vm_s, ose_version);
}

/* UDP I/O */
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
	int i = 0;
	for(; i < or_udp_output_dest_count; i++){
		struct sockaddr_in sa;
		int sock = 0;
		if(or_udp_output_dests[i].ipaddr){
			sock = oserepl_udp_sock(or_udp_output_dests[i].ipaddr,
						or_udp_output_dests[i].port,
						&sa);
		}else{
			if(or_udp_have_sender_addr){
				sock = oserepl_udp_sock(or_udp_sender_addr,
							or_udp_output_dests[i].port,
							&sa);
			}
		}
		if(sock){
			int r = sendto(sock,
				       ose_getBundlePtr(bundle),
				       ose_readInt32(bundle, -4),
				       0,
				       (struct sockaddr *)&sa,
				       sizeof(struct sockaddr_in));
			if(r == -1){
				fprintf(stderr, "error sending bundle to %s:%d (%d)\n",
					or_udp_output_dests[i].ipaddr,
					or_udp_output_dests[i].port,
					r);
				return r;
			}
		}
	}
	or_udp_have_sender_addr = 0;
	return 0;
}

static int oserepl_udp_openInputPorts(void)
{
	int maxfdp1 = 0;
	for(int i = 0; i < or_udp_input_src_count; i++){
		or_udp_input_srcs[i].sock =
			oserepl_udp_sock("0.0.0.0",
					 or_udp_input_srcs[i].port,
					 &(or_udp_input_srcs[i].sa));
		if(or_udp_input_srcs[i].sock + 1 > maxfdp1){
			maxfdp1 = or_udp_input_srcs[i].sock + 1;
		}
	}
	return maxfdp1;
}

/* REPL */
static void oserepl_sigHandler(int signo)
{
	switch(signo){
	case SIGINT:
		or_quit++;
		if(or_quit > 1){
			exit(0);
		}
		break;
	case SIGABRT:
		or_have_input = 0;
		or_assertion_failed = 1;
		break;
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

static struct or_opt *oserepl_getoptrec(const char * const opt)
{
	for(int i = 0; i < sizeof(or_opts) / sizeof(struct or_opt); i++){
		if(!strcmp(opt, or_opts[i].option)){
			return or_opts + i;
		}
	}
	return NULL;
}

static void oserepl_opt_printHelp(struct or_opt o)
{
	const char * const indent = "  ";
	if(strlen(o.syntax) > 12){
		fprintf(stdout, "%s%-12s\n                %s\n",
			indent,
			o.syntax,
			o.description);
	}else{
		fprintf(stdout, "%s%-12s  %s\n",
			indent,
			o.syntax,
			o.description);
	}
	if(o.exs[0].example){
		fprintf(stdout, "                examples:\n");
	}
	for(int j = 0;
	    j < sizeof(o.exs) / sizeof(struct or_opt_ex);
	    j++){
		if(o.exs[j].example){
			fprintf(stdout, "              %s\n                %s\n",
				o.exs[j].example,
				o.exs[j].description);
		}else{
			break;
		}
	}
}

static int oserepl_opt_help(int argnum, char **av)
{
	fprintf(stdout, "USAGE: ose [options]\n");
	fprintf(stdout, "OPTIONS:\n");
	for(int i = 0; i < sizeof(or_opts) / sizeof(struct or_opt); i++){
		oserepl_opt_printHelp(or_opts[i]);	
	}
	exit(0);
}

static int oserepl_opt_prefix(int argnum, char **av)
{
	struct or_opt *o = oserepl_getoptrec("--prefix");
	ose_assert(o);
	int switchlen = strlen("--prefix");
	if(av[argnum][switchlen] == '='){
		if(or_prefix){
			free(or_prefix);
		}
		or_prefix = strdup(av[argnum] + switchlen + 1);
		or_prefixlen = strlen(or_prefix);
		return 1;
	}
	fprintf(stderr, "error: --prefix\n");
	oserepl_opt_printHelp(*o);
	exit(1);
}

static int oserepl_opt_udp_input(int argnum, char **av)
{
	struct or_opt *o = oserepl_getoptrec("--udp-input");
	ose_assert(o);
	char *a = av[argnum];
	char *tok = strtok(a, "=");
	tok = strtok(NULL, ",");
	while(tok){
		uint16_t port = (uint16_t)strtol(tok, NULL, 10);
		or_udp_input_srcs[or_udp_input_src_count].port = port;		
		or_udp_input_src_count++;
		tok = strtok(NULL, ",");
	}
	if(or_udp_input_src_count == 0){
		fprintf(stderr, "error: --udp-input: no input ports specified!\n");
		oserepl_opt_printHelp(*o);
		exit(1);
	}
	return 1;
}

static int oserepl_opt_udp_output(int argnum, char **av)
{
	struct or_opt *o = oserepl_getoptrec("--udp-output");
	ose_assert(o);
	char *a = av[argnum];
	char *tok = strtok(a, "=");
	tok = strtok(NULL, ",");
	while(tok){
		char *p = strchr(tok, ':');
		*p = 0;
		if(p == tok){
			/* no address -- return to sender */
			or_udp_output_dests[or_udp_output_dest_count].ipaddr = NULL;
		}else{
			or_udp_output_dests[or_udp_output_dest_count].ipaddr = strdup(tok);
		}
		uint16_t port = (uint16_t)strtol(p + 1, NULL, 10);
		or_udp_output_dests[or_udp_output_dest_count].port = port;
		*p = ':';
		or_udp_output_dest_count++;
		tok = strtok(NULL, ",");
	}
	if(or_udp_output_dest_count == 0){
		fprintf(stderr, "error: --udp-output: no destinations specified!\n");
		oserepl_opt_printHelp(*o);
		exit(1);
	}
	return 1;
}

static int oserepl_opt_verbose(int argnum, char **av)
{
	or_verbose = 1;
	return 1;
}

static int oserepl_opt_version(int argnum, char **av)
{
	fprintf(stdout, "%s\n%s\n", ose_version, ose_date_compiled);
	exit(0);
}

int main(int ac, char **av)
{
	int i;
	or_prefix = strdup("/ose");
	or_prefixlen = 4;
	{
		/* process args */
		int opt = 1;
		while(opt < ac){
			int found = 0;
			for(int i = 0;
			    i < sizeof(or_opts) / sizeof(struct or_opt);
			    i++){
				if(!strncmp(av[opt],
					    or_opts[i].option,
					    strlen(or_opts[i].option))){
					opt += or_opts[i].fn(opt, av);
					found = 1;
					break;
				}
			}
			if(!found){
				fprintf(stderr,
					"error: unrecognized option: %s\n",
					av[opt]);
				oserepl_opt_help(opt, av);
				return 1;
			}
		}
	}

	/* install signal handler */
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

	/* history */
	const char *homedir = getenv("HOME");
	char histfile[strlen(homedir) + 6];
	sprintf(histfile, "%s/.ose", homedir);
	linenoiseHistoryLoad(histfile);

	/* set up ose environment and vm */
	bytes = (char *)malloc(MAX_BUNDLE_LEN);
	oserepl_init();

	/* linenoise setup */
	linenoiseSetWordBreakChars(" \t\n\"\\'`@=;|&{(/");
	linenoiseSetCompletionCallback(oserepl_linenoise_completion_cb);
	struct linenoiseState l = linenoiseSetLineCallback(oserepl_linenoise_cb,
							   or_prompt);
	or_linenoise_state = &l;
	{
		linenoisePrintf(&l, "Ose %s\n", ose_version);
#ifdef OSE_DEBUG
		linenoisePrintf(&l, "%s\n", ose_debug);
#endif
		linenoisePrintf(&l, "\n");
	}
	
	int maxfdp1 = oserepl_udp_openInputPorts();
	fd_set rset;
	FD_ZERO(&rset);
	
	
#ifdef OSE_HAVE_HPTIMER
	or_hptimer = ose_initTimer();
#endif

	while(1){
		if(or_assertion_failed){
			or_assertion_failed = 0;
			oserepl_init();
		}
		if(or_quit){
			goto cleanup_and_quit;
		}

		maxfdp1 = 1;
		
		for(i = 0; i < or_udp_input_src_count; i++){
			FD_SET(or_udp_input_srcs[i].sock, &rset);
			if(or_udp_input_srcs[i].sock + 1 > maxfdp1){
				maxfdp1 = or_udp_input_srcs[i].sock + 1;
			}
		}
		FD_SET(STDIN_FILENO, &rset);
		int nselect = select(maxfdp1, &rset, NULL, NULL, NULL);
		if(nselect <= 0){
			continue;
		}

		if(or_assertion_failed){
			or_assertion_failed = 0;
			oserepl_init();
		}
		if(or_quit){
			goto cleanup_and_quit;
		}
		/*************************************************
		 * read
		 *************************************************/
		for(i = 0; i < or_udp_input_src_count; i++){
			if(FD_ISSET(or_udp_input_srcs[i].sock, &rset)){
				struct sockaddr_in ca;
				socklen_t ca_len = sizeof(struct sockaddr_in);;
				memset(&ca, 0, sizeof(struct sockaddr_in));
				char buf[65536];
				int len = recvfrom(or_udp_input_srcs[i].sock,
						   buf, 1024, 0,
						   (struct sockaddr *)&ca,
						   &ca_len);
				if(len > 0){
					ose_pushBlob(vm_i, len, buf);
					ose_blobToElem(vm_i);
					ose_popAllDrop(vm_i);
					or_have_input = 1;
					uint32_t a = ntohl(ca.sin_addr.s_addr);

					/* announce that we received */
					/* a bundle over udp */
					linenoisePrintf(&l,
							"Received %d byte bundle"
							"from %d.%d.%d.%d\n",
							len,
							(a & 0xFF000000) >> 24,
							(a & 0x00FF0000) >> 16,
							(a & 0x0000FF00) >> 8,
							a & 0x000000FF,
							or_udp_input_srcs[i].port);

					snprintf(or_udp_sender_addr, 16,
						 "%d.%d.%d.%d",
						 (a & 0xFF000000) >> 24,
						 (a & 0x00FF0000) >> 16,
						 (a & 0x0000FF00) >> 8,
						 a & 0x000000FF);
					or_udp_have_sender_addr = 1;
				}
			}
		}

		if(FD_ISSET(STDIN_FILENO, &rset)){
			linenoiseReadChar(&l);
		}

		int should_print = 0;
		/*************************************************
		 * eval
		 *************************************************/
		uint64_t elapsed_time = 0;
		if(or_have_input){
			elapsed_time = oserepl_run();
			should_print = 1;
		}

		/*************************************************
		 * print
		 *************************************************/
		if(should_print){
			oserepl_udp_send(vm_o);
		}
		ose_clear(vm_o);
		
		if(should_print){
			linenoisePrintf(&l, "%s", ANSI_COLOR_CYAN);
			int32_t n = ose_pprintFullBundle_impl(vm_s, NULL, 0, "Stack");
			char buf[n + 1];
			ose_pprintFullBundle_impl(vm_s, buf, n + 1, "Stack");
			linenoisePrintf(&l, "\n%s", buf);
			if(or_verbose){
				int32_t s = ose_readInt32(vm_s, -4);
				int32_t sa = ose_spaceAvailable(vm_s);
				linenoisePrintf(&l, ANSI_COLOR_YELLOW);
				linenoisePrintf(&l, "\n");
				linenoisePrintf(&l, "%d%% used, %d bytes free\n",
				       (int32_t)(s / (s + sa)), sa);
#ifdef OSE_HAVE_HPTIMER
				linenoisePrintf(&l,
						"Elapsed time: %"PRIo64"ns, %fms, %fs\n",
						elapsed_time,
						(double)elapsed_time / 1000000.,
						(double)elapsed_time / 1000000000.);
#endif
			}
			linenoisePrintf(&l, "%s\n", ANSI_COLOR_RESET);
			or_have_input = 0;
		}
	}
 cleanup_and_quit:
	linenoiseHistorySave(histfile);
	return 0;
}
