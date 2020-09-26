/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2009, The Regents of
  the University of California (Regents). 
  Permission to use, copy, modify, distribute, and distribute modified versions
  of this software and its documentation without fee and without a signed
  licensing agreement, is hereby granted, provided that the above copyright
  notice, this paragraph and the following two paragraphs appear in all copies,
  modifications, and distributions.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
  OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
  BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
  HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/
#include "ose_match.h"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

/* #define OSE_MATCH_LOGSTATE */
#define OSE_MATCH_RANGE_CAN_BE_NEGATIVE

static int ose_match_range(const char *pattern, const char *address);

#define OSE_MATCH_RANGE_NOMATCH 1

static const char * const _ose_match_errstr[] = 
	{
	 "no error",
	 "unmatched left square bracket",
	 "unmatched right square bracket",
	 "unmatched left curly brace",
	 "unmatched right curly brace",
	 "pattern does not begin with a slash",
	 "address does not begin with a slash",
	 "invalid character range",
	 "backtrack limit exceeded"
	};

const char * const ose_match_errstr(unsigned long e)
{
	e >>= 8;
	if(e < sizeof(_ose_match_errstr) / sizeof(char *)){
		return _ose_match_errstr[e];
	}
	return NULL;
}

#ifdef OSE_MATCH_LOGSTATE
#define OSE_MATCH_PRINTSTATE(p, a, po, ao) ose_match_printState(p, a, po, ao);
static void ose_match_printState(const char *pattern,
				 const char *address,
				 int po,
				 int ao)
{
	for(int i = 0; i < strlen(pattern); i++){
		if(i == po){
			fprintf(stderr, "v ");
		}else{
			fprintf(stderr, "  ");
		}
	}
	fprintf(stderr, "\n");
	for(int i = 0; i < strlen(pattern); i++){
		fprintf(stderr, "%c ", pattern[i]);
	}
	fprintf(stderr, "\n");
	for(int i = 0; i < strlen(address); i++){
		fprintf(stderr, "%c ", address[i]);
	}
	fprintf(stderr, "\n");
	for(int i = 0; i < strlen(address); i++){
		if(i == ao){
			fprintf(stderr, "^ ");
		}else{
			fprintf(stderr, "  ");
		}
	}
	fprintf(stderr, "\n");
}

#define OSE_MATCH_PUSH(__p, __a){					\
		sp++;							\
		if((sp - stack) >= OSE_MATCH_BACKTRACK_LIMIT){		\
			return OSE_MATCH_ERROR_BACKTRACK_LIMIT_EXCEEDED; \
		}							\
		sp->p = __p;						\
		sp->a = __a;						\
		fprintf(stderr, "PUSH{%c(%d); %c(%d);}\n",			\
		       pattern[__p], __p, address[__a], __a);		\
	}
#define OSE_MATCH_RETURN_SUCCESS(__r){				\
		switch(__r){					\
		case 1:						\
			fprintf(stderr, "return: ADDRESS complete\n");	\
			break;					\
		case 2:						\
			fprintf(stderr, "return: PATTERN complete\n");	\
			break;					\
		case 3:						\
			fprintf(stderr, "return: BOTH complete\n");	\
			break;					\
		default:					\
			break;					\
		}						\
		return __r;					\
	}
#define OSE_MATCH_RETURN_FAILURE(__r){					\
		*pattern_offset = 0;					\
		*address_offset = 0;					\
		if(__r >= 0x100){					\
			fprintf(stderr, "return: ERROR: %s\n", ose_match_errstr(__r)); \
		}else{							\
			fprintf(stderr, "return: NO MATCH (%d)\n", __LINE__);	\
		}							\
		return __r;						\
	}
#else
#define OSE_MATCH_PRINTSTATE(p, a, po, ao) ;
#define OSE_MATCH_PUSH(__p, __a){					\
		sp++;							\
		if((sp - stack) >= OSE_MATCH_BACKTRACK_LIMIT){		\
			return OSE_MATCH_ERROR_BACKTRACK_LIMIT_EXCEEDED; \
		}							\
		sp->p = __p;						\
		sp->a = __a;						\
	}
#define OSE_MATCH_RETURN_SUCCESS(__r) return __r;
#define OSE_MATCH_RETURN_FAILURE(__r) *pattern_offset = *address_offset = 0; \
	return __r;
#endif
#define OSE_MATCH_POP() sp--;

int ose_match_pattern(const char *pattern,
		      const char *address,
		      int *pattern_offset,
		      int *address_offset)
{
	/* loosen these restrictions as ose (osc also) doesn't require */
	/* an address to begin with a slash. */
	/* if(*pattern != '/' && *pattern != '#'){ */
	/* 	OSE_MATCH_RETURN_FAILURE(OSE_MATCH_ERROR_PATTERN_NO_LEADING_SLASH); */
	/* } */
	/* if(*address != '/' && *address != '#'){ */
	/* 	OSE_MATCH_RETURN_FAILURE(OSE_MATCH_ERROR_ADDRESS_NO_LEADING_SLASH); */
	/* } */
	/* this was a hack to speed things up in the case of a fast success. */
	/* most of the time it made things worse */
	/* if(!strcmp(address, pattern)){ */
	/* 	OSE_MATCH_RETURN_SUCCESS((OSE_MATCH_PATTERN_COMPLETE */
	/* 				  | OSE_MATCH_ADDRESS_COMPLETE)); */
	/* } */
	*pattern_offset = 0;
	*address_offset = 0;

	struct state {int p,a;} stack[OSE_MATCH_BACKTRACK_LIMIT];
	struct state *sp = stack;
	sp->p = sp->a = 0;

	while(1){
#ifdef OSE_MATCH_LOGSTATE
		fprintf(stderr, "stackptr: %ld: ", (long)(sp - stack));
#endif
		
		if(sp < stack){
			
#ifdef OSE_MATCH_LOGSTATE
			fprintf(stderr, "fail\n");
#endif
			
			OSE_MATCH_RETURN_FAILURE(OSE_MATCH_NOMATCH);
		}
#ifdef OSE_MATCH_LOGSTATE
		fprintf(stderr, "sp{%c(%d); %c(%d);}\n",
			pattern[sp->p], sp->p,
			address[sp->a], sp->a); 
#endif
		
		char p = pattern[sp->p];
		char a = address[sp->a];

		*pattern_offset = sp->p;
		*address_offset = sp->a;

		OSE_MATCH_PRINTSTATE(pattern, address, sp->p, sp->a);

		if(a == '\0'){
			while(pattern[sp->p] == '*'){
				sp->p++;
			}
			*pattern_offset = sp->p;
			p = pattern[sp->p];

			if(p == '\0'){
				OSE_MATCH_RETURN_SUCCESS((OSE_MATCH_PATTERN_COMPLETE | OSE_MATCH_ADDRESS_COMPLETE));
			}else if(p == '/'){
				OSE_MATCH_RETURN_SUCCESS(OSE_MATCH_ADDRESS_COMPLETE);
			}else{
				OSE_MATCH_POP();
				continue;
			}
		}
		if(a == '/'){
			if(p == '/'){
				int pp = sp->p;
				int aa = sp->a;
				sp = stack;
				sp->p = pp + 1;
				sp->a = aa + 1;
				continue;
			}else if(p == '\0'){
				OSE_MATCH_RETURN_SUCCESS(OSE_MATCH_PATTERN_COMPLETE);
			}else if(p == '*'){
				while(pattern[sp->p] == '*'){
					sp->p++;
				}
				continue;
			}else{
				OSE_MATCH_POP();
				continue;
			}
		}
#ifdef OSE_MATCH_ALLOW_STAR_IN_ADDRESS
		if(a == '*'
		   && sp->a > 0
		   && address[sp->a - 1] == '/'
		   && (address[sp->a + 1] == '/' || address[sp->a + 1] == '\0'))
			{
				while(address[sp->a] != '/'
				      && address[sp->a] != '\0')
					{
						sp->a++;
					}
				while(pattern[sp->p] != '/'
				      && pattern[sp->p] != '\0')
					{
						sp->p++;
					}
				continue;
			}
#endif

		switch(p){
		case '/':
			/* we already checked to see if a is a '/' or a '\0', */
			/* so just pop and continue; */
			OSE_MATCH_POP();
			break;
		case '\0':
			/* we know a is not a '/' or a '\0', */
			/* so just pop and continue; */
			OSE_MATCH_POP();
			break;
		case '?':
			sp->p++;
			sp->a++;
			break;
		case '[': {
			int ret = ose_match_range(pattern + sp->p,
						  address + sp->a);
			switch(ret){
			case 0:
				OSE_MATCH_POP();
				break;
			case 1:
				sp->a++;
				/* if the first char inside the opening square */
				/* bracket is a closing square bracket, it's */
				/* treated as a normal char, so skip over it */
				if(pattern[sp->p + 1] == ']'){
					sp->p += 2;
				}
				while(1){
					if(pattern[sp->p] == ']'){
						break;
					}else if(pattern[sp->p] == '/'
						 || pattern[sp->p] == '\0')
						{
							OSE_MATCH_RETURN_FAILURE(OSE_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET);
						}
					sp->p++;
				}
				sp->p++;
				break;
			default:

				OSE_MATCH_RETURN_FAILURE(ret);
			}
		}
			break;
		case '{': {
			int rest = sp->p;
			while(pattern[rest] != '}'){
				if(pattern[rest] == '/'
				   || pattern[rest] == '\0')
					{
						OSE_MATCH_RETURN_FAILURE(OSE_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE);
					}
				rest++;
			}
			rest++;
			int cont = 0;
			sp->p++;
			int p1 = sp->p, p2 = sp->p;
			int aa = sp->a;
			OSE_MATCH_POP();
			while(pattern[p2] != '/' && pattern[p2] != '\0'){
				if(pattern[p2] == '/'){
					OSE_MATCH_RETURN_FAILURE(OSE_MATCH_ERROR_UNMATCHED_LEFT_CURLY_BRACE);
				}else if(pattern[p2] == ','
					 || pattern[p2] == '}')
					{
						if(!strncmp(pattern + p1,
							    address + aa,
							    p2 - p1))
							{
								cont = 1;
								OSE_MATCH_PUSH(rest, aa + (p2 - p1));
							}
						p1 = p2 + 1;
					}
				if(pattern[p2] == '}'){
					break;
				}
				p2++;
			}
		}
			break;
		case '}':
			OSE_MATCH_RETURN_FAILURE(OSE_MATCH_ERROR_UNMATCHED_RIGHT_CURLY_BRACE);
		case ']':
			OSE_MATCH_RETURN_FAILURE(OSE_MATCH_ERROR_UNMATCHED_RIGHT_SQUARE_BRACKET);
		case '*': {
			while(pattern[sp->p + 1] == '*'){
				/* avoid all kinds of extra backtracking with */
				/* multiple stars */
				sp->p++;
			}
			if(pattern[sp->p + 1] == '\0'
			   || pattern[sp->p + 1] == '/')
				{
					sp->p++;
					while(address[sp->a] != '/'
					      && address[sp->a] != '\0')
						{
							sp->a++;
						}
				}else{
				int pp = sp->p + 1;
				int aa = sp->a;
				OSE_MATCH_POP();
				int here = aa;
				while(address[aa] != '/'
				      && address[aa] != '\0')
					{
						aa++;
					}
				aa--;
				while(aa != here){
					OSE_MATCH_PUSH(pp, aa);
					aa--;
				}
				OSE_MATCH_PUSH(pp, aa);
			}
		}
			break;
		default:
			if(p == a){
				sp->p++;
				sp->a++;
			}else{
				OSE_MATCH_POP();
			}
			break;
		}
	}
	/* unreachable */
}

static int ose_match_range(const char *pattern, const char *address)
{
	const char *p = pattern;
	p++;
	int val = 1;
	if(*p == '!'){
		p++;
		val = 0;
	}
	int matched = !val;

	/* we're on the first character inside the square brackets */
	/* if it's a - or a ], it gets treated as a normal character */
	if(*p == ']'){
		if(*p == *address){
			return val;
		}
	}
	while(*p != ']'){
		char c = *p++, c2;
		if(c == '\0' || c == '/'){
			return OSE_MATCH_ERROR_UNMATCHED_LEFT_SQUARE_BRACKET;
		}
		if(*p == '-'){
			if(p[1] != '\0' && p[1] != '/' && p[1] != ']'){
				p++;
				c2 = *p++;
#ifdef OSE_MATCH_RANGE_CAN_BE_NEGATIVE
				if((c <= *address && *address <= c2)
				   || (c >= *address && *address >= c2))
					{
#else
						if(c2 < c){
							return OSE_MATCH_ERROR_INVALID_CHARACTER_RANGE;
						}
						if(c <= *address
						   && *address <= c2)
							{
#endif
								matched = val;
								break;
							}
						continue;
					}
			}
			if(c == *address){
				matched = val;
				break;
			}
		}
		return matched;
	}
