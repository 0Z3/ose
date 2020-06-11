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
#define PORT_ENV 10053
#define PORT_SEND 10666

int udpsock_input;
int udpsock_env;
struct sockaddr_in sockaddr_input;
struct sockaddr_in sockaddr_env;
struct sockaddr_in sockaddr_send;

void printStack(ose_bundle bundle, char *name);
struct sockaddr_in sockaddr_init(char *addr, short port);
int setupUDP(struct sockaddr_in *sa, short port);
int sendUDP(ose_bundle bundle, int sock, short port, struct sockaddr_in *addr);
void sendStacksUDP(int sock, struct sockaddr_in *addr);
void run(void);

char *bytes;
ose_bundle bundle, osevm, vm_i, vm_s, vm_e, vm_c, vm_d, vm_o;

int quit = 0;

int step = 0;

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
	rl_attempted_completion_over = 1;

	// Note: returning nullptr here will make readline use the default filename
	// completer.
	return rl_completion_matches(text, completion_generator);

}

void rl_cb(char *line)
{
	if(!line){
		quit = 1;
		return;
	}

	if(strlen(line) <= 0){
		return;
	}
	int len = strlen(line);
	const char *linep = line;
	// eat leading whitespace
	while(linep - line < len
	      && (*linep  == ' '
		  || *linep == '\t')){
		linep++;
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
		run();
		sendStacksUDP(udpsock_input,
			      &sockaddr_send);
		printStack(vm_s, "STACK");
		free(line);
	}else{
		if(ose_bundleIsEmpty(vm_i) == OSETT_FALSE
		   || ose_bundleIsEmpty(vm_c) == OSETT_FALSE){
			ose_bundleAll(vm_i);
			ose_moveBundleElemToDest(vm_i, vm_d);
			ose_bundleAll(vm_s);
			ose_moveBundleElemToDest(vm_s, vm_d);
			ose_bundleAll(vm_e);
			ose_moveBundleElemToDest(vm_e, vm_d);
			ose_bundleAll(vm_c);
			ose_moveBundleElemToDest(vm_c, vm_d);
		}
		add_history(line);
		//ose_parseString(line, osevm);
		//ose_bundleAll(vm_o);
		//ose_moveBundleElemToDest(vm_o, vm_i);
		//ose_popAllDrop(vm_i);
		ose_pushMessage(vm_i, line, strlen(line), 0);
		run();
		sendStacksUDP(udpsock_input,
			      &sockaddr_send);
		printStack(vm_s, "STACK");
		free(line);
	}
}

char *ose_reader_prompt = "# ";

void sig_handler(int signo)
{
	if(signo == SIGINT){
		quit = 1;
	}
}

void repl_import(ose_bundle bundle)
{
	const char * const path = ose_peekString(vm_s);
	const int32_t pathlen = strlen(path);
	if(!strcmp(path + (pathlen - 4), ".ose")){
		// FILE *fp = fopen(path, "rb");
		// if(!fp){
		// 	fprintf(stderr, "couldn't open %s\n", path);
		// 	return;
		// }
		// fseek(fp , 0L , SEEK_END);
		// const int32_t n = ftell(fp);
		// rewind(fp);
		// if(n > MAX_INPUT_LEN){
		// 	fprintf(stderr, "%s is too big (limit %d chars)\n",
		// 		path, MAX_INPUT_LEN);
		// 	fclose(fp);
		// 	return;
		// }
		// char buf[MAX_INPUT_LEN];
		// memset(buf, 0, MAX_INPUT_LEN);
		// if(fread(buf, n, 1, fp) != 1){
		// 	fprintf(stderr, "couldn't read %s\n", path);
		// 	fclose(fp);
		// 	return;
		// }
		// fclose(fp);
		
		// ose_drop(vm_s);
		
		//ose_pushString(vm_s, buf);
		
		ose_readFile(vm_s, path);
		ose_nip(vm_s);
		ose_pushMessage(vm_i,
				"/!/eval", strlen("/!/eval"), 0);
		ose_pushMessage(vm_i,
				"/!/parse", strlen("/!/parse"), 0);
	}else{
		ose_try{
			ose_loadLib(vm_e, path);
		}ose_catch(1){
			fprintf(stderr, "couldn't open %s\n",
				path);
		}ose_end_try;
		fprintf(stdout, "---loaded %s successfully---\n", path);
		ose_drop(vm_s);
	}
}

void repl_step(ose_bundle bundle)
{
	step = 1;
	fprintf(stdout, "---stepping enabled---\n");
}

int main(int ac, char **av)
{
	if(signal(SIGINT, sig_handler) == SIG_ERR){
		fprintf(stderr, "can't catch SIGINT\n");
		return 0;
	}
	const char *homedir = getenv("HOME");
	char histfile[strlen(homedir) + 6];
	sprintf(histfile, "%s/.ose", homedir);
	
	read_history(histfile);
	bytes = (char *)calloc(1, MAX_BUNDLE_LEN);
	bundle = ose_newBundleFromCBytes(MAX_BUNDLE_LEN, bytes);
	osevm = osevm_init(bundle);
	vm_i = OSEVM_INPUT(osevm);
	vm_s = OSEVM_STACK(osevm);
	vm_e = OSEVM_ENV(osevm);
	vm_c = OSEVM_CONTROL(osevm);
        vm_d = OSEVM_DUMP(osevm);
	vm_o = OSEVM_OUTPUT(osevm);
	
	rl_callback_handler_install(ose_reader_prompt, rl_cb);
	rl_attempted_completion_function = completer;
	rl_basic_word_break_characters = " \t\n\"\\'`@><=;|&{(";

	sockaddr_input = sockaddr_init("127.0.0.1", PORT_INPUT);
	sockaddr_env = sockaddr_init("127.0.0.1", PORT_ENV);
	sockaddr_send = sockaddr_init("127.0.0.1", PORT_SEND);
	
	udpsock_input = setupUDP(&sockaddr_input, PORT_INPUT);
	udpsock_env = setupUDP(&sockaddr_env, PORT_ENV);
	fd_set rset;
	FD_ZERO(&rset);
	int maxfdp1 = (udpsock_env > udpsock_input
		       ? udpsock_env
		       : udpsock_input) + 1;

	ose_pushMessage(vm_e, "/repl/import", strlen("/repl/import"),
			1, OSETT_CFUNCTION, repl_import);
	ose_pushMessage(vm_e, "/repl/step", strlen("/repl/step"),
			1, OSETT_CFUNCTION, repl_step);
	
	while(1){
		FD_SET(udpsock_input, &rset);
		FD_SET(udpsock_env, &rset);
		FD_SET(STDIN_FILENO, &rset);
		int nselect = select(maxfdp1, &rset, NULL, NULL, NULL);
		if(quit){
			break;
		}
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
			}
		}
		if(FD_ISSET(udpsock_env, &rset)){
			struct sockaddr_in clientaddr;
			socklen_t clientaddr_len = sizeof(struct sockaddr_in);;
			memset(&clientaddr, 0, sizeof(struct sockaddr_in));
			char buf[1024];
			int len = recvfrom(udpsock_env, buf, 1024, 0,
					   (struct sockaddr *)&clientaddr,
					   &clientaddr_len);
			if(len > 0){
				ose_pushBlob(vm_i, len, buf);
				ose_blobToElem(vm_i);
				ose_popAllDrop(vm_i);
				ose_countElems(vm_i);
				int32_t n = ose_popInt32(vm_i);
				for(int i = 0; i < n; i++){
					ose_replaceBundleElemInDest(vm_i, vm_e);
					ose_drop(vm_i);
				}
				sendStacksUDP(udpsock_input,
					      (struct sockaddr_in *)&clientaddr);
			}
		}

		if(FD_ISSET(STDIN_FILENO, &rset)){
			rl_callback_read_char();
		}
	}
	int r = write_history(histfile);
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
	int32_t s = ose_readInt32(bundle, -4);
	printf(ANSI_COLOR_GREEN
	       "%s (%d bytes used of %d (%d%%)):\n",
	       name,
	       s,
	       s + ose_spaceAvailable(bundle),
	       (int)(s / (s + ose_spaceAvailable(bundle))));
	printf("%s\n\n" ANSI_COLOR_RESET, buf);
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
	sendUDP(vm_e,
		sock,
		PORT_SEND,
		addr);
	sendUDP(vm_s,
		sock,
		PORT_SEND,
		addr);
}

void run(void)
{
	// while(osevm_step(osevm) == OSETT_TRUE){
	// 	if(step){
	// 		fprintf(stdout, "con(t)inue, (n)ext\n");
	// 		break;
	// 	}
	// }
	osevm_run(osevm);
}
