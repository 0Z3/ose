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

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define MAX_INPUT_LEN 4096
#define MAX_OUTPUT_LEN 4096
#define MAX_BUNDLE_LEN 1000000

#define PORT_INPUT 10350
#define PORT_SEND 10666

int udpsock_input;
struct sockaddr_in sockaddr_input;
struct sockaddr_in sockaddr_send;

void printStack(ose_bundle bundle, char *name);
struct sockaddr_in sockaddr_init(char *addr, short port);
int setupUDP(struct sockaddr_in *sa, short port);
int sendUDP(ose_bundle bundle, int sock, short port, struct sockaddr_in *addr);
void sendStacksUDP(int sock, struct sockaddr_in *addr);
void run(void);

char *bytes;
ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o;

// options
int verbose = 0;
int step = 0;
volatile sig_atomic_t abort_on_error = 0;

jmp_buf repl_jmp_buf;
volatile sig_atomic_t repl_assertion_failed = 0;

volatile sig_atomic_t quit = 0;

void repl_abortOnError(ose_bundle bundle);
void repl_import(ose_bundle osevm);
void repl_send(ose_bundle osevm);
void repl_step(ose_bundle bundle);
void repl_verbose(ose_bundle bundle);
	
struct repl_fn
{
	char *name;
	ose_fn fn;
} repl_fns[] = {
		{"/repl/abortonerror", repl_abortOnError},
		{"/repl/import", repl_import},
		{"/repl/send", repl_send},
		{"/repl/step", repl_step},
		{"/repl/verbose", repl_verbose},
		// print bundle
};

void ose_repl_preInput(ose_bundle osevm)
{
}

void ose_repl_postInput(ose_bundle osevm)
{
}

void ose_repl_preControl(ose_bundle osevm)
{
}

void ose_repl_postControl(ose_bundle osevm)
{
}

void ose_repl_funcall(ose_bundle osevm)
{
	ose_bundle vm_s = OSEVM_STACK(osevm);
	ose_bundle vm_c = OSEVM_CONTROL(osevm);
	const char *address = ose_peekAddress(vm_c);
	const int32_t len = strlen(address);
	if(len > 2){
		for(int i = 0;
		    i < sizeof(repl_fns) / sizeof(struct repl_fn);
		    i++){
			if(!strcmp(address + 2, repl_fns[i].name)){
				repl_fns[i].fn(osevm);
				return;
			}
		}
	}
	osevm_funcall(osevm);
}

char *completion_generator(const char *text, int state)
{
	int symtablen = ose_symtab_len();
	static char *matches[1024];
	static int nmatches;
	static int curmatch;
	static char tok = 0;
	if(state == 0){
		nmatches = 0;
		curmatch = 0;
		int len = strlen(text);
		if(len < 3){
			return NULL;
		}
		if(*text != '/'){
			return NULL;
		}
		text++;
		len--;
		tok = *text;
		text++;
		len--;
		if(tok == '!'){
			for(int i = 0;
			    i < sizeof(repl_fns) / sizeof(struct repl_fn);
			    i++){
				char *name = repl_fns[i].name;
				if(!strncmp(name, text, len)){
					matches[nmatches++] = name;
				}
			}
			for(int i = 0; i < symtablen; i++){
				char *sym = ose_symtab_getNthSym(i);
				if(!strncmp(text, sym, len)){
					matches[nmatches++] = sym;
				}
			}
		}else if(tok == '$'){
			int32_t o = OSE_BUNDLE_HEADER_LEN;
			int32_t s = ose_readInt32(vm_e, -4);
			while(o < s){
				int32_t ss = ose_readInt32(vm_e, o);
				char *addy = ose_readString(vm_e, o + 4);
				if(!strncmp(text, addy, len)){
					matches[nmatches++] = addy;
				}
				o += ss + 4;
			}
		}else{
			return NULL;
		}
	}
	if(curmatch == nmatches){
		return NULL;
	}else{
		char *sym = matches[curmatch++];
		int symlen = strlen(sym);
		char *buf = malloc(symlen + 3);
		sprintf(buf, "/%c%s", tok, sym);
		return buf;
		//return strdup(matches[curmatch++]);
	}
}

char **completer(const char *text, int start, int end)
{
	// Don't do filename completion even if our generator finds no matches.
	// rl_attempted_completion_over = 1;

	// Note: returning nullptr here will make readline use the default
	// filename completer.
	return rl_completion_matches(text, completion_generator);
}

void rl_cb(char *line)
{
	if(!line){
		return;
	}

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
			
	if(*linep != '/'){
		switch(*linep){
		case '?':
			printf("better ask someone else\n");
			return;
		case 'i':
			printStack(vm_i, "INPUT");
			return;
		case 's':
			printStack(vm_s, "STACK");
			return;
		case 'e':
			printStack(vm_e, "ENV");
			return;
		case 'c':
			printStack(vm_c, "CONTROL");
			return;
		case 'd':
			printStack(vm_d, "DUMP");
			return;
		case 'o':
			printStack(vm_o, "OUTPUT");
			return;
		case 't':
			if(step){
				step = 0;
			}else{
				return;
			}
			break;
		case 'n':
			if(!step){
				return;
			}
			break;
		default:
			return;
		}
		free(line);
		run();
		sendStacksUDP(udpsock_input,
			      &sockaddr_send);
		printStack(vm_s, "STACK");
	}else{
		ose_pushMessage(vm_i, line, strlen(line), 0);
		if(ose_bundleIsEmpty(vm_i) == OSETT_FALSE
		   || ose_bundleIsEmpty(vm_c) == OSETT_FALSE){
			;
		}
		add_history(line);
		free(line);
		run();
		sendStacksUDP(udpsock_input,
			      &sockaddr_send);
		printStack(vm_s, "STACK");
	}
}

char *ose_reader_prompt = "# ";

void sig_handler(int signo)
{
	switch(signo){
	case SIGINT:
		quit++;
		if(quit > 1){
			exit(0);
		}
		longjmp(repl_jmp_buf, 2);
	case SIGABRT:
		repl_assertion_failed = 1;
		longjmp(repl_jmp_buf, 1);
	}
}

void repl_abortOnError(ose_bundle bundle)
{
	abort_on_error = (abort_on_error == 0);
	if(abort_on_error){
		signal(SIGABRT, SIG_DFL);
	}else{
		signal(SIGABRT, sig_handler);
	}
	fprintf(stdout, "---abort_on_error %s---\n",
		abort_on_error ? "on" : "off");
}

void repl_import(ose_bundle bundle)
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
		// ose_rollBottom(vm_s);
		// ose_drop(vm_s);
	}
}

void repl_send(ose_bundle osevm)
{
	char *addr = NULL;
	short port = 0;
	if(ose_peekMessageArgType(vm_s) == OSETT_STRING){
		int32_t addrlen = strlen(ose_peekString(vm_s));
		addr = (char *)calloc(addrlen + 1, 1);
		ose_popString(vm_s, addr);
	}else{
		ose_rassert(0 && "send arg 1 must be a string", 1);
	}
	if(ose_peekMessageArgType(vm_s) == OSETT_INT32){
		port = ose_popInt32(vm_s);
	}else{
		ose_rassert(0 && "send arg 2 must be a port num (int)", 1);
	}
	struct sockaddr_in sockaddr = sockaddr_init(addr, port);
	int udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	bind(udpsock, (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in));
	int r = sendto(udpsock,
		       ose_getBundlePtr(vm_o),
		       ose_readInt32(vm_o, -4),
		       0,
		       (struct sockaddr *)&sockaddr,
		       sizeof(struct sockaddr_in));
}

void repl_step(ose_bundle bundle)
{
	step = (step == 0);
	fprintf(stdout, "---stepping %s---\n", step ? "enabled" : "disabled");
}

void repl_verbose(ose_bundle bundle)
{
	verbose = (verbose == 0);
	fprintf(stdout, "---verbose %s---\n", verbose ? "on" : "off");
}

void repl_init(void)
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

int main(int ac, char **av)
{
	// process args
	for(int i = 1; i < ac; i++){
		if(!strcmp(av[i], "--verbose")){
			verbose = 1;
		}
	}

	// install signal handler
	if(signal(SIGINT, sig_handler) == SIG_ERR){
		fprintf(stderr,
			"error installing signal handler to catch SIGINT\n");
		return 0;
	}
	if(signal(SIGABRT, sig_handler) == SIG_ERR){
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
	repl_init();

	// libedit
	rl_callback_handler_install(ose_reader_prompt, rl_cb);
	rl_attempted_completion_function = completer;
	rl_basic_word_break_characters = " \t\n\"\\'`@><=;|&{(";
	rl_completion_append_character = 0;

	// set up udp sockets and select
	sockaddr_input = sockaddr_init("127.0.0.1", PORT_INPUT);
	sockaddr_send = sockaddr_init("127.0.0.1", PORT_SEND);
	udpsock_input = setupUDP(&sockaddr_input, PORT_INPUT);
	fd_set rset;
	FD_ZERO(&rset);
	int maxfdp1 = udpsock_input + 1;

	while(1){
		switch(setjmp(repl_jmp_buf)){
		case 1: {
			if(repl_assertion_failed){
				repl_assertion_failed = 0;
				repl_init();
				rl_callback_read_char();
			}
		}
			break;
		case 2: {
			goto cleanup_and_quit;
		}
			break;
		}

		FD_SET(udpsock_input, &rset);
		FD_SET(STDIN_FILENO, &rset);
		int nselect = select(maxfdp1, &rset, NULL, NULL, NULL);
		if(nselect <= 0){
			continue;
		}
		//////////////////////////////////////////////////
		// service udp
		//////////////////////////////////////////////////
		if(FD_ISSET(udpsock_input, &rset)){
			struct sockaddr_in clientaddr;
			socklen_t clientaddr_len = sizeof(struct sockaddr_in);;
			memset(&clientaddr, 0, sizeof(struct sockaddr_in));
			char buf[1024];
			int len = recvfrom(udpsock_input, buf, 1024, 0,
					   (struct sockaddr *)&clientaddr,
					   &clientaddr_len);
			if(len > 0){
				ose_pushBlob(vm_i, len, buf);
				ose_blobToElem(vm_i);
				ose_popAllDrop(vm_i);
				run();
				sendStacksUDP(udpsock_input,
					      (struct sockaddr_in *)&clientaddr);
				ose_clear(vm_o);
				printStack(vm_s, "STACK");
				rl_callback_read_char();
			}
		}

		if(FD_ISSET(STDIN_FILENO, &rset)){
			rl_callback_read_char();
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

void printStack(ose_bundle bundle, char *name)
{
	int32_t len = ose_pprintBundle(bundle,
				       NULL,
				       0);
	char buf[len + 1];
	memset(buf, 0, len + 1);
	ose_pprintBundle(bundle,
			 buf,
			 len + 1);
	if(verbose){
		int32_t s = ose_readInt32(bundle, -4);
		printf(ANSI_COLOR_GREEN
		       "%s (%d bytes used of %d (%d%%)):\n",
		       name,
		       s,
		       s + ose_spaceAvailable(bundle),
		       (int)(s / (s + ose_spaceAvailable(bundle))));
	}else{
		printf(ANSI_COLOR_GREEN "%s:\n", name);
	}
	printf("%s\n" ANSI_COLOR_RESET, buf);
}

struct sockaddr_in sockaddr_init(char *addr, short port)
{
	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(struct sockaddr_in));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = inet_addr(addr);//htonl(INADDR_ANY);
	sa.sin_port = htons(port);
	return sa;
}

int setupUDP(struct sockaddr_in *sa, short port)
{
	// memset(sa, 0, sizeof(struct sockaddr_in));
	// sa->sin_family = AF_INET;
	// sa->sin_addr.s_addr = inet_addr("127.0.0.1");//htonl(INADDR_ANY);
	// sa->sin_port = htons(port);
	int udpsock = socket(AF_INET, SOCK_DGRAM, 0);
	bind(udpsock,
	     (struct sockaddr*)sa,
	     sizeof(struct sockaddr_in));
	return udpsock;
}

int sendUDP(ose_bundle bundle, int sock, short port, struct sockaddr_in *addr)
{
	addr->sin_port = htons(PORT_SEND);
	int r = sendto(sock,
		       ose_getBundlePtr(bundle),
		       ose_readInt32(bundle, -4),
		       0,
		       (struct sockaddr *)addr,
		       sizeof(struct sockaddr_in));
	return r;
}

void sendStacksUDP(int sock, struct sockaddr_in *addr)
{
	// sendUDP(vm_o,
	// 	sock,
	// 	PORT_SEND,
	// 	addr);
}

void run(void)
{
	char t = OSETT_TRUE;
	while((t = osevm_step(osevm)) == OSETT_TRUE){
		if(step){
			fprintf(stdout, "con(t)inue, (n)ext\n");
			break;
		}
	}
	if(step && t == OSETT_FALSE){
		fprintf(stdout, "---finished---\n");
	}
	//osevm_run(osevm);
}
