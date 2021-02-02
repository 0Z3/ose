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
#include "ose.h"
#include "ose_util.h"
#include "ose_assert.h"
#include "ose_print.h"

#define INCP(bufp, amt) ((bufp) ? ((bufp) += (amt)) : (bufp))
#define INCL(bufp, bufl, amt) ((bufp) ? ((bufl) -= (amt)) : (bufl))

#define ose_snprintfi(bufp, bufl, n, total, fmt, ...)			\
	{								\
		int32_t n = snprintf(bufp, bufl, fmt, __VA_ARGS__);	\
		if(bufp){						\
			bufp += n;					\
		}							\
		if(bufl >= n){						\
			bufl -= n;					\
		}else{							\
			bufl = 0;					\
		}							\
		total += n;						\
	}

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
	return snprintf(buf, buflen, "%s ", addr);
}

int32_t ose_pprintMessageArg(ose_bundle bundle,
			     int32_t offset,
			     int32_t tto,
			     int32_t plo,
			     char *buf,
			     int32_t buflen)
{
	const char sep = ':';
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
		
		if(blobsize > 8){
			for(int i = 0; i < 4; i++){
				n = snprintf(buf, buflen, "%02X", p[i]);
				nn += n;
				INCP(buf, n);
				INCL(buf, buflen, n);
			}
			n = snprintf(buf, buflen, "..");
			nn += n;
			INCP(buf, n);
			INCL(buf, buflen, n);
			for(int j = blobsize - 4; j < blobsize; j++){
				n = snprintf(buf, buflen, "%02X", p[j]);
				nn += n;
				INCP(buf, n);
				INCL(buf, buflen, n);
			}
		}else{
			for(int k = 0; k < blobsize; k++){
				n = snprintf(buf, buflen, "%02X", p[k]);
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
		/* address */
		n = ose_pprintMessageAddr(bundle,
					  offset,
					  ao,
					  bufp,
					  bufl);
		nn += n;
		INCP(bufp, n);
		INCL(bufp, bufl, n);
	}
	/* payload */
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
	for(int j = indent; j < indent * 2; j++){
		indentstr[j] = '#';
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

static const int cols = 80;
static int32_t ose_pprintFullBundle_r(ose_constbundle bundle,
				   char *buf, int32_t buflen,
				   int32_t o,
				   int32_t s,
				   int32_t to,
				   int32_t indent)
{
	int32_t nn = 0;
	ose_snprintfi(buf, buflen, n, nn, "%08x ", to);
	for(int i = 0; i < indent - 1; i++){
		ose_snprintfi(buf, buflen, n, nn, "%s", "|");
	}
	ose_snprintfi(buf, buflen, n, nn, "%s", OSE_BUNDLE_ID);
	for(int j = 0; j < (cols / 2) - (9 + (indent - 1) + 7 + 15); j++){
		ose_snprintfi(buf, buflen, n, nn, "%s", " ");
	}
	ose_snprintfi(buf, buflen, n, nn, "%08x.%08x\n",
		      ose_readInt32(bundle, OSE_BUNDLE_ID_LEN),
		      ose_readInt32(bundle, OSE_BUNDLE_ID_LEN + 4));
	while(o < s){
		const int32_t ss = ose_readInt32(bundle, o);
		const char * const addr = ose_readString((ose_bundle)bundle,
							 o + 4);
		const int32_t addrlen = strlen(addr);
		if(!strncmp(addr, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN)){
			int32_t oo = OSE_BUNDLE_HEADER_LEN;
			ose_constbundle subbundle = ose_makeBundle(ose_getBundlePtr(bundle) + o + 4);
			int n = ose_pprintFullBundle_r(subbundle,
						       buf, buflen,
						       oo, ss, to + o,
						       indent + 1);
			nn += n;
			if(buf && (buflen > n)){
				buf += n;
				buflen -= n;
			}
		}else{
			ose_snprintfi(buf, buflen, n, nn, "%08x ", o + to);
			for(int i = 0; i < indent; i++){
				ose_snprintfi(buf, buflen, n, nn, "%s", "|");
			}
			ose_snprintfi(buf, buflen, n, nn, "%s", addr);
			for(int j = 0;
			    j < (cols / 2) - (9 + indent + strlen(addr));
			    j++){
				ose_snprintfi(buf, buflen, n, nn, "%s", " ");
			}

			int32_t to = o + 4 + ose_pnbytes(addrlen);
			const char * const ttstr = ose_readString((ose_bundle)bundle, to);
			const int32_t ttstrlen = strlen(ttstr);
			int32_t po = to + ose_pnbytes(ttstrlen);
			for(int ii = 0; ii < ttstrlen; ii++){
				char tt = ose_readByte(bundle, to);
				if(ii > 1){
					for(int j1 = 0; j1 < cols / 2 + 1; j1++){
						ose_snprintfi(buf, buflen, n, nn, "%s", " ");
					}
					ose_snprintfi(buf, buflen, n, nn, "%c ", tt);
				}else if(ii == 0){
					ose_snprintfi(buf, buflen, n, nn, "%c", tt);
				}else{
					ose_snprintfi(buf, buflen, n, nn, "%c ", tt);
				}
				
				switch(tt){
				case OSETT_ID:
					break;
				case OSETT_INT32:
					ose_snprintfi(buf, buflen, n, nn, "%d",
						      ose_readInt32(bundle, po));
					po += 4;
					break;
				case OSETT_FLOAT:
					ose_snprintfi(buf, buflen, n, nn, "%f",
						      ose_readFloat(bundle, po));
					po += 4;
					break;
				case OSETT_STRING:{
					const char * const st =
						ose_readString((ose_bundle)bundle,
							       po);
					ose_snprintfi(buf, buflen, n, nn, "%s", st);
					po += ose_pstrlen(st);
				}
					break;
				case OSETT_BLOB:{
					int32_t bs = ose_getPaddedBlobSize(bundle, po);
					po += 4;
					if(bs == 4){
						ose_snprintfi(buf, buflen, n, nn,
							      "%02x %02x %02x %02x",
							      ose_readByte(bundle, po),
							      ose_readByte(bundle, po + 1),
							      ose_readByte(bundle, po + 2),
							      ose_readByte(bundle, po + 3));
						po += 4;
					}else{
						for(int j2 = 0; j2 < bs; j2 += 8){
							if(bs - j2 == 4){
								ose_snprintfi(buf, buflen, n, nn,
									      "%02x %02x %02x %02x",
									      ose_readByte(bundle, po),
									      ose_readByte(bundle, po + 1),
									      ose_readByte(bundle, po + 2),
									      ose_readByte(bundle, po + 3));
								po += 4;
							}else{
								ose_snprintfi(buf, buflen, n, nn,
									      "%02x %02x %02x %02x "
									      "%02x %02x %02x %02x",
									      0xff & ose_readByte(bundle, po),
									      0xff & ose_readByte(bundle, po + 1),
									      0xff & ose_readByte(bundle, po + 2),
									      0xff & ose_readByte(bundle, po + 3),
									      0xff & ose_readByte(bundle, po + 4),
									      0xff & ose_readByte(bundle, po + 5),
									      0xff & ose_readByte(bundle, po + 6),
									      0xff & ose_readByte(bundle, po + 7));
								po += 8;
							}
							if(bs - j2 > 8){
								ose_snprintfi(buf, buflen, n, nn, "%s", "\n");
								for(int j3 = 0;
								    j3 < cols / 2 + 3;
								    j3++){
									ose_snprintfi(buf, buflen, n, nn, "%s", " ");
								}
							}
						}
					}
				}
					break;
				}
				to++;
				if(ii > 0 || (ii == 0 && ttstrlen == 1)){
					ose_snprintfi(buf, buflen, n, nn, "%s", "\n");
				}
			}
		}
		o += 4 + ss;
	}
	return nn;
}

int32_t ose_pprintFullBundle_impl(ose_constbundle bundle,
			     char *buf, int32_t buflen,
			     const char * const name)
{
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	const int32_t s = ose_readInt32(bundle, -4);
	int32_t nn = 0;
	if(name){
		const int namelen = strlen(name);
		ose_snprintfi(buf, buflen, n, nn, "%s", name);
		for(int i = 0; i < cols - namelen; i++){
			ose_snprintfi(buf, buflen, n, nn, "%s", "_");
		}
		ose_snprintfi(buf, buflen, n, nn, "%s", "\n");
	}
	ose_snprintfi(buf, buflen, n, nn, "%s", "OFFSET   ADDRESS");
	for(int i = 0; i < (cols / 2) - 16; i++){
		ose_snprintfi(buf, buflen, n, nn, "%s", " ");
	}
	ose_snprintfi(buf, buflen, n, nn, "%s", "TT DATA\n");
	nn += ose_pprintFullBundle_r(bundle,
				     buf, buflen,
				     o, s, 0, 1);
	return nn;
}

void ose_pprintFullBundle(ose_constbundle src,
			  ose_bundle dest,
			  const char * const name)
{
	int32_t o = ose_getLastBundleElemOffset(dest);
	if(o < ose_readInt32(dest, -4)){
		o += ose_readInt32(dest, o) + 4;
	}
	int32_t s = ose_readInt32(dest, o);
	ose_writeInt32(dest, o, 0);
	char *p = ose_getBundlePtr(dest);
	int32_t n = ose_pprintFullBundle_impl(src,
					      p + o + 12,
					      s - 12,
					      name);
	int32_t pn = ose_pnbytes(n);
	ose_writeInt32(dest, o, pn + 8);
	ose_writeByte(dest, o + 8, OSETT_ID);
	ose_writeByte(dest, o + 9, OSETT_STRING);
	ose_addToSize(dest, pn + 12);
}
