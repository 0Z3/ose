/*
n  Copyright (c) 2019-20 John MacCallum
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
#include <string.h>
#include "ose_conf.h"
#include "ose.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"

static const char * const char2address(unsigned char c)
{
	static const char * const char2address_tab[128] =
		{
		 "/&/0000", "/&/0001", "/&/0002", "/&/0003",
		 "/&/0004", "/&/0005", "/&/0006", "/&/0007",
		 "/&/0008", "/&/0009", "/&/0010", "/&/0011",
		 "/&/0012", "/&/0013", "/&/0014", "/&/0015",
		 "/&/0016", "/&/0017", "/&/0018", "/&/0019",
		 "/&/0020", "/&/0021", "/&/0022", "/&/0023",
		 "/&/0024", "/&/0025", "/&/0026", "/&/0027",
		 "/&/0028", "/&/0029", "/&/0030", "/&/0031",
		 "/&/0032", "/&/0033", "/&/0034", "/&/0035",
		 "/&/0036", "/&/0037", "/&/0038", "/&/0039",
		 "/&/0040", "/&/0041", "/&/0042", "/&/0043",
		 "/&/0044", "/&/0045", "/&/0046", "/&/0047",
		 "/&/0048", "/&/0049", "/&/0050", "/&/0051",
		 "/&/0052", "/&/0053", "/&/0054", "/&/0055",
		 "/&/0056", "/&/0057", "/&/0058", "/&/0059",
		 "/&/0060", "/&/0061", "/&/0062", "/&/0063",
		 "/&/0064", "/&/0065", "/&/0066", "/&/0067",
		 "/&/0068", "/&/0069", "/&/0070", "/&/0071",
		 "/&/0072", "/&/0073", "/&/0074", "/&/0075",
		 "/&/0076", "/&/0077", "/&/0078", "/&/0079",
		 "/&/0080", "/&/0081", "/&/0082", "/&/0083",
		 "/&/0084", "/&/0085", "/&/0086", "/&/0087",
		 "/&/0088", "/&/0089", "/&/0090", "/&/0091",
		 "/&/0092", "/&/0093", "/&/0094", "/&/0095",
		 "/&/0096", "/&/0097", "/&/0098", "/&/0099",
		 "/&/0100", "/&/0101", "/&/0102", "/&/0103",
		 "/&/0104", "/&/0105", "/&/0106", "/&/0107",
		 "/&/0108", "/&/0109", "/&/0110", "/&/0111",
		 "/&/0112", "/&/0113", "/&/0114", "/&/0115",
		 "/&/0116", "/&/0117", "/&/0118", "/&/0119",
		 "/&/0120", "/&/0121", "/&/0122", "/&/0123",
		 "/&/0124", "/&/0125", "/&/0126", "/&/0127"
		};
	return char2address_tab[c];
}

#define INIT 0
#define ADDRESS 1
#define END_ADDRESS 2
#define STRING 3
#define STRING_WS 4
#define END_STRING 5
static int processchar(char c, int state)
{
	switch(state){
	case INIT:
		switch(c){
		case '/':
			return ADDRESS;
		default:
			return INIT;
		}
		break;
	case ADDRESS:
		switch(c){
		case 32:
		case 35:
		case 42:
		case 44:
			//case 47:
		case 63:
		case 91:
		case 93:
		case 123:
		case 125:
			return END_ADDRESS;
		default:
			return ADDRESS;
		}
		break;
	case END_ADDRESS:
		switch(c){
		case '/':
			return END_STRING;
		default:
			return STRING;
		}
		break;
	case STRING:
		switch(c){
		case ' ':
		case '\t':
		case '\n':
			return STRING_WS;
		default:
			return STRING;
		}
		break;
	case STRING_WS:
		switch(c){
		case ' ':
		case '\t':
		case '\n':
			return STRING_WS;
		case '/':
			return END_STRING;
		default:
			return STRING;
		}
		break;
	case END_STRING:
		return processchar(c, ADDRESS);
	default: return INIT;
	}
}

static void pushbyte(ose_bundle bundle, char b)
{
	ose_pushMessage(bundle, char2address(b), 7, 0);
	ose_pushMessage(bundle, "/!/swap/bytes/4", strlen("/!/swap/bytes/4"), 0);
	ose_pushMessage(bundle, "/!/item/toblob", strlen("/!/item/toblob"), 0);
	ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN,
			1, OSETT_INT32, 3);
	ose_pushMessage(bundle, "/!/decat/blob", strlen("/!/decat/blob"), 0);
	ose_pushMessage(bundle, "/!/pop", strlen("/!/pop"), 0);
	ose_pushMessage(bundle, "/!/drop", strlen("/!/drop"), 0);
	ose_pushMessage(bundle, "/!/push", strlen("/!/push"), 0);
	ose_pushMessage(bundle, "/!/concat/blobs", strlen("/!/concat/blobs"), 0);
}

static void lenchk(ose_bundle bundle)
{
	ose_pushMessage(bundle, "/!/length/item", strlen("/!/length/item"), 0);
	ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN,
			1, OSETT_INT32, 4);
	ose_pushMessage(bundle, "/!/swap", strlen("/!/swap"), 0);
	ose_pushMessage(bundle, "/!/mod", strlen("/!/mod"), 0);
	ose_pushMessage(bundle, "/!/if", strlen("/!/if"), 0);
	ose_pushMessage(bundle, "/!/length/item", strlen("/!/length/item"), 0);
	ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN,
			1, OSETT_INT32, 4);
	ose_pushMessage(bundle, "/!/swap", strlen("/!/swap"), 0);
	ose_pushMessage(bundle, "/!/mod", strlen("/!/mod"), 0);
	ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN,
			1, OSETT_INT32, 4);
	ose_pushMessage(bundle, "/!/sub", strlen("/!/sub"), 0);
	ose_pushMessage(bundle, "/!/else", strlen("/!/else"), 0);
	ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN,
			1, OSETT_INT32, 4);
	ose_pushMessage(bundle, "/!/end/if", strlen("/!/end/if"), 0);
	ose_pushMessage(bundle, "/!/push/blob", strlen("/!/push/blob"), 0);
	ose_pushMessage(bundle, "/!/push", strlen("/!/push"), 0);
	ose_pushMessage(bundle, "/!/concat/blobs", strlen("/!/concat/blobs"), 0);
}

const char * const ose_lex(int *state_,
			   const char *str,
			   ose_bundle bundle)
{
	int state = *state_;
	ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_LEN,
			1, OSETT_BLOB, 0, NULL);
	while(*str != 0){
		int newstate = processchar(*str, state);
		*state_ = newstate;
		switch(newstate){
		case INIT:
			break;
		case ADDRESS:
			pushbyte(bundle, *str);
			break;
		case END_ADDRESS:
			lenchk(bundle);
			
			ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL,
					OSE_ADDRESS_ANONVAL_LEN,
					1, OSETT_INT32, OSETT_STRING);
			ose_pushMessage(bundle, "/!/blob/totype",
					strlen("/!/blob/totype"), 0);
			if(*str == ' ' || *str == '\t' || *str == '\n'){
				*state_ = STRING_WS;
			}
			return str;
		case STRING:
			pushbyte(bundle, *str);
			break;
		case STRING_WS:
			pushbyte(bundle, *str);
			break;
		case END_STRING:
			{
				lenchk(bundle);
			
				ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN, 1,
						OSETT_INT32, OSETT_STRING);
				ose_pushMessage(bundle,
						"/!/blob/totype",
						strlen("/!/blob/totype"), 0);
			}
			return str;
		}
		state = newstate;
		str++;
	}
	switch(state){
	case INIT:
		break;
	case ADDRESS:
		lenchk(bundle);
		ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL,
				OSE_ADDRESS_ANONVAL_LEN, 1,
				OSETT_INT32, OSETT_STRING);
		ose_pushMessage(bundle, "/!/blob/totype",
				strlen("/!/blob/totype"), 0);
		*state_ = END_ADDRESS;
		return str;
	case END_ADDRESS:
	case STRING:
	case STRING_WS:
	case END_STRING:
		{
			lenchk(bundle);
			ose_pushMessage(bundle, OSE_ADDRESS_ANONVAL,
					OSE_ADDRESS_ANONVAL_LEN, 1,
					OSETT_INT32, OSETT_STRING);
			ose_pushMessage(bundle,
					"/!/blob/totype",
					strlen("/!/blob/totype"), 0);
		}
		*state_ = END_STRING;
		return str;
	}
	return NULL; // can't get here
}
