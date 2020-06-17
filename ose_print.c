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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ose_conf.h"
#include "ose.h"
#include "ose_util.h"
//#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_print.h"

#define INCP(bufp, amt) ((bufp) ? ((bufp) += (amt)) : (bufp))
#define INCL(bufp, bufl, amt) ((bufp) ? ((bufl) -= (amt)) : (bufl))

static int32_t _ose_pprintBundle(ose_bundle bundle,
				 int32_t offset,
				 char *buf,
				 int32_t buflen,
				 int32_t indent);

/**************************************************
 * pretty printing
 **************************************************/

int32_t ose_pprintMessageAddr(ose_bundle bundle,
			      int32_t offset,
			      int32_t ao,
			      char *buf,
			      int32_t buflen)
{
	char *addr = ose_readString(bundle, ao);
	int32_t addrlen = strlen(addr);
	// if(addrlen + 1 > ncols){
	// 	int n = snprintf(buf, ncols - 3, "%s", addr);
	// 	n += snprintf(buf + n, buflen - n, "..");
	// 	return n;
	// }else{
		return snprintf(buf, buflen, "%s ", addr);
		//}
}

int32_t ose_pprintMessageArg(ose_bundle bundle,
			     int32_t offset,
			     int32_t tto,
			     int32_t plo,
			     char *buf,
			     int32_t buflen)
{
	const char sep = ':';
	char *b = ose_getBundlePtr(bundle);
	char tt = ose_readByte(bundle, tto);
	switch(tt){
	case OSETT_INT32:
		return snprintf(buf, buflen,
				"[i%c%d]",
				sep,
				ose_readInt32(bundle, plo));
	case OSETT_FLOAT:
		return snprintf(buf, buflen,
				"[f%c%f]",
				sep,
				ose_readFloat(bundle, plo));
	case OSETT_STRING:
		return snprintf(buf, buflen,
				"[s%c%s]",
				sep,
				ose_readString(bundle, plo));
	case OSETT_BLOB: {
		int32_t blobsize = ose_readInt32(bundle, plo);
		int32_t n =  snprintf(buf, buflen,
				      "[b:<%d:",
				      blobsize);
		int32_t nn = n;
		INCP(buf, n);
		INCL(buf, buflen, n);
		char *p = ose_readString(bundle, plo + 4);
		for(int i = 0; i < (blobsize > 4 ? 4 : blobsize); i++){
			n = snprintf(buf, buflen, "%02X", p[i]);
			nn += n;
			INCP(buf, n);
			INCL(buf, buflen, n);
		}
		if(blobsize >= 8){
			n = snprintf(buf, buflen, "..");
			nn += n;
			INCP(buf, n);
			INCL(buf, buflen, n);
			for(int i = blobsize - 4; i < blobsize; i++){
				n = snprintf(buf, buflen, "%02X", p[i]);
				nn += n;
				INCP(buf, n);
				INCL(buf, buflen, n);
			}
		}
		

		n = snprintf(buf, buflen, ">]");
		nn += n;
		INCP(buf, n);
		INCL(buf, buflen, n);
		return nn;
	}
	default: {
		return snprintf(buf, buflen, "[%c%c<>]", tt, sep);
	}
	}
}

static int32_t ose_pprintBundleElem_msg(ose_bundle bundle,
					int32_t offset,
					char *buf,
					int32_t buflen,
					int32_t indent)
{
	char *b = ose_getBundlePtr(bundle);
	int32_t s = ose_readInt32(bundle, offset);
	int32_t ao = offset + 4;
	int32_t tto = ao + ose_pstrlen(b + ao);
	int32_t plo = tto + ose_pstrlen(b + tto);
	tto++;
	int32_t n = 0;
	int32_t nn = 0;
	char *bufp = buf;
	int32_t bufl = buflen;
	{
		// address
		n = ose_pprintMessageAddr(bundle,
					  offset,
					  ao,
					  bufp,
					  bufl);
		nn += n;
		INCP(bufp, n);
		INCL(bufp, bufl, n);
	}
	// payload
	while(plo < (offset + 4 + s) && ose_readByte(bundle, tto)){
		char tt = ose_readByte(bundle, tto);
		n = ose_pprintMessageArg(bundle,
					 offset,
					 tto,
					 plo,
					 bufp,
					 bufl);
		nn += n;
		INCP(bufp, n);
		INCL(bufp, bufl, n);
		if(ose_isKnownTypetag(tt) == OSETT_TRUE){
			plo += ose_getPayloadItemSize(bundle, tt, plo);
		}else{
			if(ose_readByte(bundle, tt + 1) == 0){
				plo = offset + s + 4;
			}
		}
		tto++;
	}
	return nn;
}

static int32_t ose_pprintBundleElem(ose_bundle bundle,
				    int32_t offset,
				    char *buf,
				    int32_t buflen,
				    int32_t indent)
{
	int32_t nn = 0;
	char *bufp = buf;
	int32_t bufl = buflen;
	int n = 0;
	char betype = ose_getBundleElemType(bundle, offset);
	if(betype == OSETT_MESSAGE){
		n = ose_pprintBundleElem_msg(bundle,
					      offset,
					      bufp,
					      bufl,
					      indent);
		return n;
	}else if(betype == OSETT_BUNDLE){
		if(ose_getBundleElemElemCount(bundle, offset) == 0){
			n = snprintf(bufp, bufl, "#bundle");
		}else{
			n = snprintf(bufp, bufl, "#bundle\n");
		}
		nn += n;
		INCP(bufp, n);
		INCL(bufp, bufl, n);
		n = _ose_pprintBundle(bundle,
				       offset + 4,
				       bufp,
				       bufl,
				       indent + 1);
		nn += n;
		INCP(bufp, n);
		INCL(buf, bufl, n);
		return nn;
	}else{
		ose_assert(0 &&
			   "bundle elem is neither a message nor a bundle");
	}
	return 0;
}

static int32_t _ose_pprintBundle(ose_bundle bundle,
				 int32_t offset,
				 char *buf,
				 int32_t buflen,
				 int32_t indent)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	int32_t nn = 0;
	int32_t ss = ose_readInt32(bundle, offset - 4);
	char *bufp = buf;
	int32_t bufl = buflen;
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	offset += OSE_BUNDLE_HEADER_LEN;
	char indentstr[indent * 2 + 1];
	memset(indentstr, 0, indent * 2 + 1);
	for(int i = 0; i < indent; i++){
		indentstr[i] = ' ';
	}
	for(int i = indent; i < indent * 2; i++){
		indentstr[i] = '#';
	}
	while(o < ss){
		int n = 0;
		n = snprintf(bufp, bufl, "%s", indentstr);
		nn += n;
		INCP(bufp, n);
		INCL(bufp, bufl, n);

		int32_t s = ose_readInt32(bundle, offset);
		n = ose_pprintBundleElem(bundle,
					  offset,
					  bufp,
					  bufl,
					  indent);
		nn += n;
		INCP(bufp, n);
		INCL(bufp, bufl, n);
		o += s + 4;
		offset += s + 4;
		if(o < ss){
			n = snprintf(bufp, bufl, "\n");
			INCP(bufp, n);
			INCL(bufp, bufl, n);
			nn += n;
		}
	}
	return nn;
}

int32_t ose_pprintBundle(ose_bundle bundle,
			 char *buf,
			 int32_t buflen)
{
	return _ose_pprintBundle(bundle,
				 0,
				 buf,
				 buflen,
				 0);
}

/**************************************************
 * printing: symmetrical with ose_lex and ose_parse
 **************************************************/

int32_t ose_printMessageAddr(ose_bundle bundle,
			     int32_t offset,
			     int32_t ao,
			     char *buf,
			     int32_t buflen)
{
	if(ose_getStringLen(bundle, ao) > 0){
		return snprintf(buf, buflen, "/@%s ",
				ose_readString(bundle, ao));
	}else{
		return 0;
	}
}

int32_t ose_printMessageArg(ose_bundle bundle,
			    int32_t offset,
			    int32_t tto,
			    int32_t plo,
			    char *buf,
			    int32_t buflen)
{
	char *b = ose_getBundlePtr(bundle);
	char tt = ose_readByte(bundle, tto);
	switch(tt){
	case OSETT_INT32:
		return snprintf(buf, buflen,
				"/%c/%d ",
				tt,
				ose_readInt32(bundle, plo));
	case OSETT_FLOAT:
		return snprintf(buf, buflen,
				"/%c/%f ",
				tt,
				ose_readFloat(bundle, plo));
	case OSETT_STRING:
		return snprintf(buf, buflen,
				"/%c/%s ",
				tt,
				ose_readString(bundle, plo));
	default: return 0;
	}
}

int32_t ose_printBundleElem_msg(ose_bundle bundle,
				int32_t offset,
				char *buf,
				int32_t buflen)
{
	char *b = ose_getBundlePtr(bundle);
	int32_t s = ose_readInt32(bundle, offset);
	int32_t ao = offset + 4;
	int32_t tto = ao + ose_pstrlen(b + ao);
	int32_t plo = tto + ose_pstrlen(b + tto);
	tto++;
	int32_t n = 0;
	char *bufp = buf;
	int32_t bufl = buflen;
	while(plo < (offset + 4 + s) && ose_readByte(bundle, tto)){
		char tt = ose_readByte(bundle, tto);
		int32_t nn = ose_printMessageArg(bundle,
						 offset,
						 tto,
						 plo,
						 bufp,
						 bufl);
		
		n += nn;
		INCP(bufp, nn);
		INCL(bufp, bufl, nn);
		plo += ose_getPayloadItemSize(bundle, tt, plo);
		tto++;
	}
	int32_t nn = ose_printMessageAddr(bundle,
					  offset,
					  ao,
					  bufp,
					  bufl);
		
	n += nn;
	INCP(bufp, nn);
	INCL(bufp, bufl, nn);
	return n;
}

int32_t ose_printBundleElem_bndl(ose_bundle bundle,
				 int32_t offset,
				 char *buf,
				 int32_t buflen)
{
	return 0;
}

int32_t ose_printBundleElem(ose_bundle bundle,
			    int32_t offset,
			    char *buf,
			    int32_t buflen)
{
	char *b = ose_getBundlePtr(bundle);
	char betype = ose_getBundleElemType(bundle, offset);
	if(betype == OSETT_MESSAGE){
		return ose_printBundleElem_msg(bundle,
					       offset,
					       buf,
					       buflen);
	}else if(betype == OSETT_BUNDLE){
		return 0;
	}else{
		ose_assert(0 &&
			   "bundle elem is neither a message nor a bundle");
	}
	return 0;
}

int32_t ose_printBundle(ose_bundle bundle,
			char *buf,
			int32_t buflen)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	int32_t ss = ose_readInt32(bundle, -4);
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	char *bufp = buf;
	int32_t bufl = buflen;
	while(o < ss){
		int32_t s = ose_readInt32(bundle, o);
		int32_t n = ose_printBundleElem(bundle,
						o,
						bufp,
						bufl);
		INCP(bufp, n);
		INCL(bufp, bufl, n);
		o += s + 4;
	}
	return bufp - buf;
}
