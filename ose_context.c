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

int32_t ose_spaceAvailable(ose_bundle bundle)
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

void ose_copyBundleElemToDest(ose_bundle src, ose_bundle dest)
{
	ose_assert(ose_bundleHasAtLeastNElems(src, 1) == OSETT_TRUE);
	//ose_bundle src = bundle;
	char *srcp = ose_getBundlePtr(src);
	int32_t src_offset = ose_getLastBundleElemOffset(src);
	int32_t src_elem_size = ose_readInt32(src, src_offset) + 4;
	// ose_bundle ctxt = ose_exit(src);
	// ose_bundle dest = ose_enter(ctxt, dest_addr);
	char *destp = ose_getBundlePtr(dest);
	memcpy(destp + ose_readInt32(dest, -4),
	       srcp + src_offset,
	       src_elem_size);
	ose_addToSize(dest, src_elem_size);
}

void ose_copyBundleElemToDestAddr(ose_bundle src, char *dest_addr)
{
	ose_bundle dest = ose_enter(ose_exit(src), dest_addr);
	ose_copyBundleElemToDest(src, dest);
}

void ose_moveBundleElemToDest(ose_bundle src, ose_bundle dest)
{
	ose_copyBundleElemToDest(src, dest);
	ose_drop(src);
}

void ose_moveBundleElemToDestAddr(ose_bundle src, char *dest_addr)
{
	ose_bundle dest = ose_enter(ose_exit(src), dest_addr);
	ose_moveBundleElemToDest(src, dest);
}

void ose_replaceBundleElemInDest(ose_bundle src, ose_bundle dest)
{
	//ose_bundle src = bundle;
	char *srcp = ose_getBundlePtr(src);
	//ose_bundle dest = ose_enter(ose_exit(bundle), dest_addr);
	char *destp = ose_getBundlePtr(dest);
	
	int32_t src_offset = ose_getLastBundleElemOffset(src);
	int32_t src_size = ose_readInt32(src, src_offset);
	char *src_address = srcp + src_offset + 4;
	
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	int32_t s = ose_readInt32(dest, o);
	int32_t ss = ose_readInt32(dest, -4);
	while(o < ss){
		if(!strcmp(destp + o + 4, src_address)){
			int32_t size_diff = src_size - s;
			int32_t m1offset = o;
			int32_t m2offset = 4 + o + s;
			int32_t rest = ss - m2offset;
			if(size_diff < 0){
				size_diff = -size_diff;
				memmove(destp + m2offset - size_diff,
					destp + m2offset,
					rest);
				memset(destp + ss - size_diff,
				       0,
				       size_diff);
				ose_addToSize(dest, -size_diff);
			}else{
				ose_addToSize(dest, size_diff);
				memset(destp + ss, 0, size_diff);
				memmove(destp + m2offset + size_diff,
					destp + m2offset,
					rest);
				memset(destp + m2offset, 0, size_diff);
			}

			memcpy(destp + m1offset,
			       srcp + src_offset,
			       src_size + 4);
			return;
		}
		o += s + 4;
		s = ose_readInt32(dest, o);
	}
	ose_copyBundleElemToDest(src, dest);
}
				 
void ose_replaceBundleElemInDestAddr(ose_bundle src, char *dest_addr)
{
	ose_bundle dest = ose_enter(ose_exit(src), dest_addr);
	ose_replaceBundleElemInDest(src, dest);
}

ose_bundle ose_newBundleFromCBytes(int32_t nbytes, char *bytes)
{
	char *p = bytes;
	ose_assert(p);
	ose_assert(nbytes >= OSE_BUNDLE_HEADER_LEN + sizeof(int32_t));
	while((uintptr_t)p % 4){
		p++;
		nbytes--;
	}
	while(nbytes % 4){
		nbytes--;
	}
	memset(p, 0, nbytes);
	*((int32_t *)p) = ose_htonl(OSE_BUNDLE_HEADER_LEN);
	p += sizeof(int32_t);
	memcpy(p, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
	ose_bundle bundle = ose_makeBundle(p);
	int32_t n = ose_init(bundle,
			     OSE_CONTEXT_MESSAGE_OVERHEAD + 16,
			     "/sx");
	n = ose_init(bundle,
			     nbytes - (4 + OSE_BUNDLE_HEADER_LEN
				       + OSE_CONTEXT_MESSAGE_OVERHEAD + 16),
			     "/cx");
	ose_bundle bb = ose_enter(bundle, "/cx");
	return bb;
}
