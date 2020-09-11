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

#include "ose_conf.h"
#include "ose_assert.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose.h"
#include "ose_context.h"

static int32_t writeContextMessage(ose_bundle bundle,
				   int32_t size,
				   const char * const address)
{
	ose_assert(strlen(address) == 3);
	ose_assert(size >= OSE_CONTEXT_MESSAGE_OVERHEAD);
	char *b = ose_getBundlePtr(bundle);
	int32_t o = ose_readInt32(bundle, -4);
	int32_t alen = 3;
	int32_t palen = 4;
	int32_t overhead = OSE_CONTEXT_MESSAGE_OVERHEAD;
	int32_t freespace = size - overhead;
	int32_t msize = size - 4;
	char *p = b + o;
	*((int32_t *)p) = ose_htonl(msize);
	p += 4;
	strncpy(p, address, alen);
	p += palen;
	/*
	 * ,
	 * i : offset of data section relative to start of bundle
	 * i : offset of data section relative to start of message
	 * i : total number of bytes
	 * b : bundle (blob)
	 * b : free space (blob)
	 */
	strcpy(p, ",iiibb");
	p += 8;
	*((int32_t *)p) = ose_htonl(o + 4 + palen + 8 + 4 + 4 + 4 + 4);
	p += 4; // int
	*((int32_t *)p) = ose_htonl(4 + palen + 8 + 4 + 4 + 4);
	p += 4; // int
	*((int32_t *)p) = ose_htonl(freespace
				    + OSE_BUNDLE_HEADER_LEN);
	p += 4; // int

	// bundle
	*((int32_t *)p) = ose_htonl(OSE_BUNDLE_HEADER_LEN);
	p += 4;
	memcpy(p, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
	p += OSE_BUNDLE_HEADER_LEN;

	*((int32_t *)p) = ose_htonl(freespace);

	return freespace;
}

int32_t ose_init(ose_bundle bundle,
		 int32_t size,
		 const char * const address)
{
	ose_assert(size % 4 == 0);
	int32_t fs = writeContextMessage(bundle, size, address);
	ose_addToInt32(bundle, -4, size);
	return fs;
}

int32_t ose_pushContextMessage(ose_bundle bundle,
			       int32_t size,
			       const char * const address)
{
	ose_assert(size % 4 == 0);
	int32_t bs1 = ose_readInt32(bundle, -4);
	int32_t bs2 = ose_readInt32(bundle, bs1);
	ose_assert(size <= bs2);
	int32_t fs = writeContextMessage(bundle, size, address);
	ose_addToInt32(bundle, -4, size);
	bs1 = ose_readInt32(bundle, -4);
	bs2 -= size;
	ose_writeInt32(bundle, bs1, bs2);
	return fs;
}

int32_t ose_spaceAvailable(ose_constbundle bundle)
{
	return ose_readInt32(bundle, -8) - ose_readInt32(bundle, -4);
}

ose_bundle ose_enter(ose_bundle bundle, const char * const address)
{
	ose_assert(address);
	char *b = ose_getBundlePtr(bundle);
	int32_t s = ose_readInt32(bundle, -4);
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	while(o < s){
		if(b[4 + o] == address[0]
		   && b[4 + o + 1] == address[1]
		   && b[4 + o + 2] == address[2]
		   && b[4 + o + 3] == address[3]){
			int32_t oo = o + 4 + 4 + 8 + 16;
			ose_bundle bb = ose_makeBundle(b + oo);
			return bb;
		}
		o += ose_readInt32(bundle, o) + 4;
	}
	ose_assert(0 && "address must exist");
	return bundle;
}

ose_bundle ose_exit(ose_bundle bundle)
{
	int32_t o = ose_readInt32(bundle, -16);
	char *b = ose_getBundlePtr(bundle);
	b -= o;
	ose_bundle ret = ose_makeBundle(b);
	return ret;
}

void ose_addToSize(ose_bundle bundle, int32_t amt)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	//ose_assert(amt >= 0);
	int32_t os = ose_readInt32(bundle, -4);
	ose_assert(os >= OSE_BUNDLE_HEADER_LEN);
	int32_t ns1 = os + amt;
	ose_assert(ns1 >= OSE_BUNDLE_HEADER_LEN);
	int32_t ns2 = ose_readInt32(bundle, -8) - ns1;
	ose_assert(ns2 >= 0);
	if(amt < 0){
		ose_writeInt32(bundle, os, 0);
	}
	ose_writeInt32(bundle, -4, ns1);
	ose_writeInt32(bundle, ns1, ns2);
	ose_assert(ose_readInt32(bundle, -4) >= OSE_BUNDLE_HEADER_LEN);
	ose_assert(ose_readInt32(bundle, ose_readInt32(bundle, -4)) >= 0);
}

void ose_incSize(ose_bundle bundle, int32_t amt)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	//ose_assert(amt >= 0);
	int32_t os = ose_readInt32(bundle, -4);
	ose_assert(os >= OSE_BUNDLE_HEADER_LEN);
	int32_t ns1 = os + amt;
	ose_assert(ns1 >= OSE_BUNDLE_HEADER_LEN);
	int32_t ns2 = ose_readInt32(bundle, -8) - ns1;
	ose_assert(ns2 >= 0);
	ose_writeInt32(bundle, -4, ns1);
	ose_writeInt32(bundle, ns1, ns2);
	ose_assert(ose_readInt32(bundle, -4) >= OSE_BUNDLE_HEADER_LEN);
	ose_assert(ose_readInt32(bundle, ose_readInt32(bundle, -4)) >= 0);
}

void ose_decSize(ose_bundle bundle, int32_t amt)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	//ose_assert(amt >= 0);
	int32_t os = ose_readInt32(bundle, -4);
	ose_assert(os >= OSE_BUNDLE_HEADER_LEN);
	int32_t ns1 = os - amt;
	ose_assert(ns1 >= OSE_BUNDLE_HEADER_LEN);
	int32_t ns2 = ose_readInt32(bundle, -8) - ns1;
	ose_assert(ns2 >= 0);
	ose_writeInt32(bundle, os, 0);
	ose_writeInt32(bundle, -4, ns1);
	ose_writeInt32(bundle, ns1, ns2);
	ose_assert(ose_readInt32(bundle, -4) >= OSE_BUNDLE_HEADER_LEN);
	ose_assert(ose_readInt32(bundle, ose_readInt32(bundle, -4)) >= 0);
}

void ose_copyElem(ose_constbundle src, ose_bundle dest)
{
	ose_assert(ose_bundleHasAtLeastNElems(src, 1) == OSETT_TRUE);
	const char * const srcp = ose_getBundlePtr(src);
	const int32_t src_offset = ose_getLastBundleElemOffset(src);
	const int32_t src_elem_size = ose_readInt32(src, src_offset) + 4;
	char *destp = ose_getBundlePtr(dest);
	memcpy(destp + ose_readInt32(dest, -4),
	       srcp + src_offset,
	       src_elem_size);
	ose_incSize(dest, src_elem_size);
}

void ose_moveElem(ose_bundle src, ose_bundle dest)
{
	ose_copyElem(src, dest);
	ose_drop(src);
}

void ose_copyBundle(ose_constbundle src, ose_bundle dest)
{
	ose_assert(ose_isBundle(src) == OSETT_TRUE);
	ose_assert(ose_isBundle(dest) == OSETT_TRUE);
	int32_t ds = ose_readInt32(dest, -4);
	ose_assert(ds >= OSE_BUNDLE_HEADER_LEN);
	int32_t ss = ose_readInt32(src, -4);
	ose_assert(ss >= OSE_BUNDLE_HEADER_LEN);
	ose_addToSize(dest, ss + 4);
	memcpy(ose_getBundlePtr(dest) + ds,
	       ose_getBundlePtr(src) - 4,
	       ss + 4);
}

void ose_appendBundle(ose_bundle src, ose_bundle dest)
{
	ose_assert(ose_isBundle(src) == OSETT_TRUE);
	ose_assert(ose_isBundle(dest) == OSETT_TRUE);
	const char * const sp = ose_getBundlePtr(src);
	char *dp = ose_getBundlePtr(dest);
	const int32_t so = ose_getLastBundleElemOffset(src);
	const int32_t ss = ose_readInt32(src, so);
	const int32_t ds = ose_readInt32(dest, -4);
	if(so >= OSE_BUNDLE_HEADER_LEN){
		if(!strncmp(sp + so + 4, OSE_BUNDLE_ID, OSE_BUNDLE_ID_LEN)){
			ose_addToSize(dest, ss - OSE_BUNDLE_HEADER_LEN);
			memcpy(dp + ds,
			       sp + so + 4 + OSE_BUNDLE_HEADER_LEN,
			       ss - OSE_BUNDLE_HEADER_LEN);
		}else{
			ose_addToSize(dest, ss + 4);
			memcpy(dp + ds,
			       sp + so,
			       ss + 4);
		}
		ose_drop(src);
	}else{
		// if the source is empty, do nothing
		;
	}
}

void ose_replaceBundle(ose_bundle src, ose_bundle dest)
{
	ose_clear(dest);
	ose_appendBundle(src, dest);
}



ose_bundle ose_newBundleFromCBytes(int32_t nbytes, char *bytes)
{
	char *p = bytes;
	ose_assert(p);
	ose_assert(nbytes >= OSE_BUNDLE_HEADER_LEN + sizeof(int32_t));
	while((uintptr_t)p % OSE_CONTEXT_ALIGNMENT){
		p++;
		nbytes--;
	}
	while(nbytes % OSE_CONTEXT_ALIGNMENT){
		nbytes--;
	}
	memset(p, 0, nbytes);
	*((int32_t *)p) = ose_htonl(OSE_BUNDLE_HEADER_LEN);
	p += sizeof(int32_t);
	memcpy(p, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
	ose_bundle bundle = ose_makeBundle(p);
	int32_t n = ose_init(bundle,
			     OSE_CONTEXT_MESSAGE_OVERHEAD
			     + OSE_CONTEXT_STATUS_MESSAGE_SIZE,
			     "/sx");
	n = ose_init(bundle,
		     nbytes - (4 + OSE_BUNDLE_HEADER_LEN
			       + OSE_CONTEXT_MESSAGE_OVERHEAD
			       + OSE_CONTEXT_STATUS_MESSAGE_SIZE),
		     "/cx");
	ose_bundle bb = ose_enter(bundle, "/cx");
	return bb;
}
