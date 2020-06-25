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

/**
 * @file ose_stackops.h
 * @brief Stack operations
 */

#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include <math.h>

#include "ose_conf.h"
#include "ose.h"
#include "ose_context.h"
#include "ose_stackops.h"
#include "ose_util.h"
#include "ose_assert.h"
#include "ose_match.h"

static void ose_drop_impl(ose_bundle bundle, int32_t o, int32_t s);
static void ose_dup_impl(ose_bundle bundle, int32_t o, int32_t s);
static void ose_swap_impl(ose_bundle bundle,
			  int32_t offset_nm1,
			  int32_t size_nm1,
			  int32_t offset_n,
			  int32_t size_n);
static void ose_rot_impl(ose_bundle bundle,
			 int32_t onm2,
			 int32_t snm2,
			 int32_t onm1,
			 int32_t snm1,
			 int32_t on,
			 int32_t sn);
static void ose_notrot_impl(ose_bundle bundle,
			    int32_t onm2,
			    int32_t snm2,
			    int32_t onm1,
			    int32_t snm1,
			    int32_t on,
			    int32_t sn);
static void over(ose_bundle bundle,
		 int32_t onm1,
		 int32_t snm1,
		 int32_t on,
		 int32_t sn);
static void dropArg(ose_bundle bundle);
static void be1(ose_bundle bundle, int32_t *on, int32_t *sn);
static void be2(ose_bundle bundle,
		int32_t *onm1,
		int32_t *snm1,
		int32_t *on,
		int32_t *sn);
static void be3(ose_bundle bundle,
		int32_t *onm2,
		int32_t *snm2,
		int32_t *onm1,
		int32_t *snm1,
		int32_t *on,
		int32_t *sn);
static void be4(ose_bundle bundle,
		int32_t *onm3,
		int32_t *snm3,
		int32_t *onm2,
		int32_t *snm2,
		int32_t *onm1,
		int32_t *snm1,
		int32_t *on,
		int32_t *sn);

/**************************************************
 * C API
 **************************************************/

static void pushInt32(ose_bundle bundle, int32_t i, char typetag)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	ose_assert(typetag != OSETT_ID);
	char *b = ose_getBundlePtr(bundle);
	const int32_t o = ose_readInt32(bundle, -4);
	ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
	const int32_t n = 4 + OSE_ADDRESS_ANONVAL_SIZE + 4 + 4;
	ose_addToSize(bundle, n);
	char *ptr = b + o;
	memset(ptr, 0, n);
	*((int32_t *)ptr) = ose_htonl(n - 4);
	ptr += 4;
	memcpy(ptr, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE);
	ptr += OSE_ADDRESS_ANONVAL_SIZE;
	*ptr++ = OSETT_ID;
	*ptr++ = typetag;
	*ptr++ = '\0';
	*ptr++ = '\0';
	*((int32_t *)ptr) = ose_htonl(i);
	ose_assert(ptr - (b + o) < n);
}

void ose_pushInt32(ose_bundle bundle, int32_t i)
{
	pushInt32(bundle, i, OSETT_INT32);
}

void ose_pushFloat(ose_bundle bundle, float f)
{
	char *p = (char *)&f;
	int32_t i = *((int32_t *)p);
	pushInt32(bundle, i, OSETT_FLOAT);
}

static void pushString(ose_bundle bundle,
		       const char * const s,
		       char typetag)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	ose_assert(s);
	ose_assert(typetag != OSETT_ID);
	char *b = ose_getBundlePtr(bundle);
	const int32_t o = ose_readInt32(bundle, -4);
	ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
	const int32_t sl = strlen(s);
	const int32_t psl = ose_pnbytes(sl);
	const int32_t n = 4 + OSE_ADDRESS_ANONVAL_SIZE + 4 + psl;
	ose_addToSize(bundle, n);
	char *ptr = b + o;
	memset(ptr, 0, n);
	*((int32_t *)ptr) = ose_htonl(n - 4);
	ptr += 4;
	memcpy(ptr, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE);
	ptr += OSE_ADDRESS_ANONVAL_SIZE;
	*ptr++ = OSETT_ID;
	*ptr++ = typetag;
	*ptr++ = '\0';
	*ptr++ = '\0';
	for(int i = 0; i < sl; i++){
		*ptr++ = s[i];
	}
	ose_assert(ptr - (b + o) < n);
}

void ose_pushString(ose_bundle bundle, const char * const s)
{
	pushString(bundle, s, OSETT_STRING);
}

void ose_pushBlob(ose_bundle bundle,
		  int32_t blobsize,
		  const char * const blob)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	ose_assert(blobsize >= 0);
	// blob can be NULL
	char *b = ose_getBundlePtr(bundle);
	const int32_t o = ose_readInt32(bundle, -4);
	ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
	int32_t padded_blobsize = blobsize;
	while(padded_blobsize % 4){
		padded_blobsize++;
	}
	const int32_t n =
		4
		+ OSE_ADDRESS_ANONVAL_SIZE
		+ 4
		+ 4
		+ padded_blobsize;
	ose_addToSize(bundle, n);
	char *ptr = b + o;
	memset(ptr, 0, n);
	*((int32_t *)ptr) = ose_htonl(n - 4);
	ptr += 4;
	memcpy(ptr, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE);
	ptr += OSE_ADDRESS_ANONVAL_SIZE;
	*ptr++ = OSETT_ID;
	*ptr++ = OSETT_BLOB;
	*ptr++ = '\0';
	*ptr++ = '\0';
	*((int32_t *)ptr) = ose_htonl(blobsize);
	ptr += 4;
	if(blobsize){
		ose_assert(blobsize <= n - (ptr - (b + o)));
		if(blob){
			memcpy(ptr, blob, blobsize);
		}else{
			memset(ptr, 0, blobsize);
		}
		ptr += padded_blobsize;
	}
	ose_assert(ptr - (b + o) == n);
}

#ifdef OSE_PROVIDE_TYPE_SYMBOL
void ose_pushSymbol(ose_bundle bundle, const char * const s)
{
	pushString(bundle, s, OSETT_SYMBOL);
}
#endif

#if defined(OSE_PROVIDE_TYPE_DOUBLE) ||		\
	defined(OSE_PROVIDE_TYPE_INT64) ||	\
	defined(OSE_PROVIDE_TYPE_UINT64)
static void pushInt64(ose_bundle bundle, int64_t i, char typetag)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	ose_assert(typetag != OSETT_ID);
	char *b = ose_getBundlePtr(bundle);
	const int32_t o = ose_readInt32(bundle, -4);
	ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
	const int32_t n = 4 + OSE_ADDRESS_ANONVAL_SIZE + 4 + 8;
	ose_addToSize(bundle, n);
	char *ptr = b + o;
	memset(ptr, 0, n);
	*((int32_t *)ptr) = ose_htonl(n - 4);
	ptr += 4;
	memcpy(ptr, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE);
	ptr += OSE_ADDRESS_ANONVAL_SIZE;
	*ptr++ = OSETT_ID;
	*ptr++ = typetag;
	*ptr++ = '\0';
	*ptr++ = '\0';
	*((int64_t *)ptr) = ose_htonll(i);
	ose_assert(ptr - (b + o) < n);
}
#endif

#ifdef OSE_PROVIDE_TYPE_DOUBLE
void ose_pushDouble(ose_bundle bundle, double f)
{
	int64_t i = *((int64_t *)&f);
	pushInt64(bundle, i, OSETT_DOUBLE);
}
#endif

#ifdef OSE_PROVIDE_TYPE_INT8
void ose_pushInt8(ose_bundle bundle, int8_t i)
{
	pushInt32(bundle, i, OSETT_INT8);
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT8
void ose_pushUInt8(ose_bundle bundle, uint8_t i)
{
	pushInt32(bundle, i, OSETT_UINT8);
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT32
void ose_pushUInt32(ose_bundle bundle, uint32_t i)
{
	pushInt32(bundle, i, OSETT_UINT32);
}
#endif

#ifdef OSE_PROVIDE_TYPE_INT64
void ose_pushInt64(ose_bundle bundle, int64_t i)
{
	pushInt64(bundle, i, OSETT_INT64);
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT64
void ose_pushUInt64(ose_bundle bundle, uint64_t i)
{
	pushInt64(bundle, i, OSETT_UINT64);
}
#endif

#ifdef OSE_PROVIDE_TYPE_TIMETAG
void ose_pushTimetag(ose_bundle bundle, uint32_t sec, uint32_t fsec)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	char *b = ose_getBundlePtr(bundle);
	const int32_t o = ose_readInt32(bundle, -4);
	ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
	const int32_t n = 4 + OSE_ADDRESS_ANONVAL_SIZE + 4 + 4 + 4;
	ose_addToSize(bundle, n);
	char *ptr = b + o;
	*((int32_t *)ptr) = ose_htonl(n - 4);
	ptr += 4;
	memcpy(ptr, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE);
	ptr += OSE_ADDRESS_ANONVAL_SIZE;
	*ptr++ = OSETT_ID;
	*ptr++ = OSETT_TIMETAG;
	*ptr++ = 0;
	*ptr++ = 0;
	*((int32_t *)ptr) = ose_htonl(sec);
	ptr += 4;
	*((int32_t *)ptr) = ose_htonl(fsec);
	ose_assert(ptr - (b + o) < n);
}
#endif

#if defined(OSE_PROVIDE_TYPE_TRUE)		\
	|| defined(OSE_PROVIDE_TYPE_FALSE)	\
	|| defined(OSE_PROVIDE_TYPE_NULL)	\
	|| defined(OSE_PROVIDE_TYPE_INFINITUM)
static void pushUnitType(ose_bundle bundle, char typetag)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	ose_assert(typetag != OSETT_ID);
	char *b = ose_getBundlePtr(bundle);
	const int32_t o = ose_readInt32(bundle, -4);
	ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
	const int32_t n = 4 + OSE_ADDRESS_ANONVAL_SIZE + 4;
	ose_addToSize(bundle, n);
	char *ptr = b + o;
	memset(ptr, 0, n);
	*((int32_t *)ptr) = ose_htonl(n - 4);
	ptr += 4;
	memcpy(ptr, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE);
	ptr += OSE_ADDRESS_ANONVAL_SIZE;
	*ptr++ = OSETT_ID;
	*ptr++ = typetag;
	*ptr++ = '\0';
	*ptr++ = '\0';
	ose_assert(ptr - (b + o) < n);
}
#endif

#ifdef OSE_PROVIDE_TYPE_TRUE
void ose_pushTrue(ose_bundle bundle)
{
	pushUnitType(bundle, OSETT_TRUE);
}
#endif

#ifdef OSE_PROVIDE_TYPE_FALSE
void ose_pushFalse(ose_bundle bundle)
{
	pushUnitType(bundle, OSETT_FALSE);	
}
#endif

#ifdef OSE_PROVIDE_TYPE_NULL
void ose_pushNull(ose_bundle bundle)
{
	pushUnitType(bundle, OSETT_NULL);	
}
#endif

#ifdef OSE_PROVIDE_TYPE_INFINITUM
void ose_pushInfinitum(ose_bundle bundle)
{
	pushUnitType(bundle, OSETT_INFINITUM);	
}
#endif

void ose_pushCFn(ose_bundle bundle, const ose_fn fn)
{
	ose_pushBlob(bundle, OSE_INTPTR2, NULL);
	int32_t o = ose_readInt32(bundle, -4);
	o -= OSE_INTPTR2;
	ose_assert(o > OSE_BUNDLE_HEADER_LEN);
	ose_writeCFn(bundle, o, fn);
}

void ose_pushMessage(ose_bundle bundle,
		     const char * const address,
		     int32_t addresslen,
		     int32_t n, ...)
{
	ose_assert(ose_isBundle(bundle) == OSETT_TRUE);
	const int32_t o = ose_readInt32(bundle, -4);
	ose_assert(o >= OSE_BUNDLE_HEADER_LEN);
	va_list ap;
	va_start(ap, n);
	int32_t s = ose_vwriteMessage(bundle,
				      o,
				      address,
				      addresslen,
				      n,
				      ap);
	va_end(ap);
	ose_addToSize(bundle, s);
}

char *ose_peekAddress(const ose_bundle bundle)
{
	assert(ose_bundleIsEmpty(bundle) == OSETT_FALSE);
	const int32_t o = ose_getLastBundleElemOffset(bundle);
	return ose_getBundlePtr(bundle) + o + 4;
}

char ose_peekMessageArgType(const ose_bundle bundle)
{
	assert(ose_bundleIsEmpty(bundle) == OSETT_FALSE);
	const int32_t o = ose_getLastBundleElemOffset(bundle);
	const int32_t s = ose_readInt32(bundle, o);
	ose_assert(s >= 0);
	if(s <= 8){
		return OSETT_NOTYPETAG;
	}else{
		ose_assert(strcmp(ose_readString(bundle, o + 4),
				  OSE_BUNDLE_ID));
		const int32_t tto = o + 4 + ose_getPaddedStringLen(bundle,
								   o + 4);
		const char *ptr = ose_getBundlePtr(bundle);
		ose_assert(tto - o < s);
		const int32_t len = strlen(ptr + tto);
		ose_assert((tto + len - 1) - o < s);
		return ptr[tto + len - 1];
	}
}

char ose_peekType(const ose_bundle bundle)
{
	assert(ose_bundleIsEmpty(bundle) == OSETT_FALSE);
	const int32_t o = ose_getLastBundleElemOffset(bundle);
	return ose_getBundleElemType(bundle, o);
}

static char *peek(const ose_bundle bundle, char typetag)
{
	assert(ose_bundleIsEmpty(bundle) == OSETT_FALSE);
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	return ose_getBundlePtr(bundle) + lpo;
}

int32_t ose_peekInt32(const ose_bundle bundle)
{
	return ose_ntohl(*((int32_t *)peek(bundle, OSETT_INT32)));
}

float ose_peekFloat(const ose_bundle bundle)
{
	int32_t i = ose_ntohl(*((int32_t *)peek(bundle, OSETT_FLOAT)));
	char *p = (char *)&i;
	return *((float *)p);
}

char *ose_peekString(const ose_bundle bundle)
{
	return peek(bundle, OSETT_STRING);
}

char *ose_peekBlob(const ose_bundle bundle)
{
	return peek(bundle, OSETT_BLOB);
}

#ifdef OSE_PROVIDE_TYPE_SYMBOL
char *ose_peekSymbol(const ose_bundle bundle)
{
	return peek(bundle, OSETT_SYMBOL);
}
#endif

#ifdef OSE_PROVIDE_TYPE_DOUBLE
double ose_peekDouble(const ose_bundle bundle)
{
	int64_t i = ose_ntohll(*((int64_t *)peek(bundle, OSETT_DOUBLE)));
	return *((double *)&i);
}
#endif

#ifdef OSE_PROVIDE_TYPE_INT8
int8_t ose_peekInt8(const ose_bundle bundle)
{
	return ose_ntohl(*((int32_t *)peek(bundle, OSETT_INT8)));
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT8
uint8_t ose_peekUInt8(const ose_bundle bundle)
{
	return ose_ntohl(*((uint32_t *)peek(bundle, OSETT_UINT8)));
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT32
uint32_t ose_peekUInt32(const ose_bundle bundle)
{
	return ose_ntohl(*((uint32_t *)peek(bundle, OSETT_UINT32)));
}
#endif

#ifdef OSE_PROVIDE_TYPE_INT64
int64_t ose_peekInt64(const ose_bundle bundle)
{
	return ose_ntohll(*((int64_t *)peek(bundle, OSETT_INT64)));
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT64
uint64_t ose_peekUInt64(const ose_bundle bundle)
{
	return ose_ntohll(*((uint64_t *)peek(bundle, OSETT_UINT64)));
}
#endif

#ifdef OSE_PROVIDE_TYPE_TIMETAG
struct ose_timetag ose_peekTimetag(const ose_bundle bundle)
{
	struct ose_timetag t =
		*((struct ose_timetag *)peek(bundle, OSETT_TIMETAG));
	t.sec = ose_ntohl(t.sec);
	t.fsec = ose_ntohl(t.fsec);
	return t;
}
#endif

// no peek functions for unit types

int32_t ose_popInt32(ose_bundle bundle)
{
	int32_t i = ose_peekInt32(bundle);
	ose_drop(bundle);
	return i;
}

float ose_popFloat(ose_bundle bundle)
{
	float f = ose_peekFloat(bundle);
	ose_drop(bundle);
	return f;
}

int32_t ose_popString(ose_bundle bundle, char *buf)
{
	char *ptr = ose_peekString(bundle);
	int32_t len = strlen(ptr);
	strncpy(buf, ptr, len);
	ose_drop(bundle);
	return len;
}

int32_t ose_popBlob(ose_bundle bundle, char *buf)
{
	char *ptr = ose_peekBlob(bundle);
	int32_t bloblen = ose_ntohl(*((int32_t *)ptr));
	memcpy(buf, ptr + 4, bloblen);
	ose_drop(bundle);
	return bloblen;
}

#ifdef OSE_PROVIDE_TYPE_SYMBOL
int32_t ose_popSymbol(ose_bundle bundle, char *buf)
{
	char *ptr = ose_peekSymbol(bundle);
	int32_t len = strlen(ptr);
	strncpy(buf, ptr, len);
	ose_drop(bundle);
	return len;
}
#endif

#ifdef OSE_PROVIDE_TYPE_DOUBLE
double ose_popDouble(ose_bundle bundle)
{
	double f = ose_peekDouble(bundle);
	ose_drop(bundle);
	return f;
}
#endif

#ifdef OSE_PROVIDE_TYPE_INT8
int8_t ose_popInt8(ose_bundle bundle)
{
	int8_t i = ose_peekInt8(bundle);
	ose_drop(bundle);
	return i;
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT8
uint8_t ose_popUInt8(ose_bundle bundle)
{
	uint8_t i = ose_peekUInt8(bundle);
	ose_drop(bundle);
	return i;
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT32
uint32_t ose_popUInt32(ose_bundle bundle)
{
	uint32_t i = ose_peekUInt32(bundle);
	ose_drop(bundle);
	return i;
}
#endif

#ifdef OSE_PROVIDE_TYPE_INT64
int64_t ose_popInt64(ose_bundle bundle)
{
	int64_t i = ose_peekInt64(bundle);
	ose_drop(bundle);
	return i;
}
#endif

#ifdef OSE_PROVIDE_TYPE_UINT64
uint64_t ose_popUInt64(ose_bundle bundle)
{
	uint64_t i = ose_peekUInt64(bundle);
	ose_drop(bundle);
	return i;
}
#endif

#ifdef OSE_PROVIDE_TYPE_TIMETAG
struct ose_timetag ose_popTimetag(ose_bundle bundle)
{
	struct ose_timetag t = ose_peekTimetag(bundle);
	ose_drop(bundle);
	return t;
}
#endif

// no pop functions for unit types

/**************************************************
 * Stack Operations
 **************************************************/

void ose_2drop(ose_bundle bundle)
{
	ose_drop(bundle);
	ose_drop(bundle);
}

void ose_2dup(ose_bundle bundle)
{
	ose_swap(bundle);
	ose_dup(bundle);
	ose_rot(bundle);
	ose_dup(bundle);
}

void ose_2over(ose_bundle bundle)
{
	int32_t onm3 = 0;
	int32_t snm3 = 0;
	int32_t onm2 = 0;
	int32_t snm2 = 0;
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be4(bundle, &onm3, &snm3, &onm2, &snm2, &onm1, &snm1, &on, &sn);
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + on + sn + 4, b + onm3, snm3 + snm2 + 8);
}

void ose_2swap(ose_bundle bundle)
{
	int32_t onm3 = 0;
	int32_t snm3 = 0;
	int32_t onm2 = 0;
	int32_t snm2 = 0;
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be4(bundle, &onm3, &snm3, &onm2, &snm2, &onm1, &snm1, &on, &sn);
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + on + sn + 4, b + onm3, snm3 + snm2 + 8);
	memmove(b + onm3, b + onm1, snm3 + snm2 + snm1 + sn + 16);
	memset(b + on + sn + 4, 0, snm3 + snm2 + 8);
}

static void ose_drop_impl(ose_bundle bundle, int32_t o, int32_t s)
{
	char *b = ose_getBundlePtr(bundle);
	memset(b + o, 0, s + 4);
	ose_addToSize(bundle, -(s + 4));
}

void ose_drop(ose_bundle bundle)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 1) == OSETT_TRUE);
	int32_t o = 0;
	int32_t s = 0;
	be1(bundle, &o, &s);
	ose_drop_impl(bundle, o, s);
}

static void ose_dup_impl(ose_bundle bundle, int32_t o, int32_t s)
{
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + o + s + 4, b + o, s + 4);
	ose_addToSize(bundle, s + 4);
}

void ose_dup(ose_bundle bundle)
{
	int32_t o = 0;
	int32_t s = 0;
	be1(bundle, &o, &s);
	ose_dup_impl(bundle, o, s);
}

void ose_nip(ose_bundle bundle)
{
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be2(bundle, &onm1, &snm1, &on, &sn);
	ose_swap_impl(bundle, onm1, snm1, on, sn);
	ose_drop_impl(bundle, onm1 + sn + 4, snm1);
}

static void ose_notrot_impl(ose_bundle bundle,
		   int32_t onm2,
		   int32_t snm2,
		   int32_t onm1,
		   int32_t snm1,
		   int32_t on,
		   int32_t sn)
{
	char *b = ose_getBundlePtr(bundle);
	memmove(b + onm2 + sn + 4, b + onm2, snm2 + snm1 + sn + 12);
	memcpy(b + onm2, b + on + sn + 4, sn + 4);
	memset(b + on + sn + 4, 0, sn + 4);
}

void ose_notrot(ose_bundle bundle)
{

	int32_t onm2 = 0;
	int32_t snm2 = 0;
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be3(bundle, &onm2, &snm2, &onm1, &snm1, &on, &sn);
	ose_notrot_impl(bundle, onm2, snm2, onm1, snm1, on, sn);
}

static void over(ose_bundle bundle,
		 int32_t onm1,
		 int32_t snm1,
		 int32_t on,
		 int32_t sn)
{
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + on + sn + 4, b + onm1, snm1 + 4);
	ose_addToSize(bundle, snm1 + 4);
}

void ose_over(ose_bundle bundle)
{
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be2(bundle, &onm1, &snm1, &on, &sn);
	over(bundle, onm1, snm1, on, sn);
}

static void pick(ose_bundle bundle, int32_t *_o1, int32_t *_o2, int32_t *_s)
{
	int32_t i = ose_popInt32(bundle);
        int32_t n = 0;
	int32_t s = ose_readInt32(bundle, -4);
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	while(o < s){
		n++;
		o += ose_readInt32(bundle, o) + 4;
	}
	n--;
	ose_assert(i <= n);
	o = OSE_BUNDLE_HEADER_LEN;
	while(n - i > 0 && o < s){
		n--;
		o += ose_readInt32(bundle, o) + 4;
	}
	int32_t oo = o;
	int32_t ss = ose_readInt32(bundle, oo);
	while(o < s){
		o += ose_readInt32(bundle, o) + 4;
	}
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + o, b + oo, ss + 4);
	*_o1 = o;
	*_o2 = oo;
	*_s = ss;
}

void ose_pick(ose_bundle bundle)
{
	char *b = ose_getBundlePtr(bundle);
	int32_t o1 = 0, o2 = 0, s = 0;
	pick(bundle, &o1, &o2, &s);
	ose_addToSize(bundle, s + 4);
}

void ose_pickBottom(ose_bundle bundle)
{

}

void ose_pickMatch(ose_bundle bundle)
{
	char *addr = ose_peekString(bundle);
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	int32_t s = ose_readInt32(bundle, -4);
	while(o < s){
		if(!strcmp(addr, ose_readString(bundle, o + 4))){
			//ose_drop(bundle);
			char *b = ose_getBundlePtr(bundle);
			s = ose_readInt32(bundle, -4);
			int32_t ss = ose_readInt32(bundle, o);
			memcpy(b + s, b + o, ss + 4);
			ose_addToSize(bundle, ss + 4);
			return;
		}
		o += ose_readInt32(bundle, o) + 4;
	}
}

void ose_roll(ose_bundle bundle)
{
	char *b = ose_getBundlePtr(bundle);
	int32_t o = 0, oo = 0, ss = 0;
	pick(bundle, &o, &oo, &ss);
	memmove(b + oo, b + oo + ose_readInt32(bundle, oo) + 4, o - oo);
	memset(b + o, 0, ss);
}

void ose_rollBottom(ose_bundle bundle)
{
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	int32_t s = ose_readInt32(bundle, -4);
	ose_assert(o < s);
	char *b = ose_getBundlePtr(bundle);
	int32_t ss = ose_readInt32(bundle, o);
	memcpy(b + s, b + o, ss + 4);
	memmove(b + o, b + o + ss + 4, s);
	memset(b + s, 0, ss + 4);
}

void ose_rollMatch(ose_bundle bundle)
{

}

static void ose_rot_impl(ose_bundle bundle,
		int32_t onm2,
		int32_t snm2,
		int32_t onm1,
		int32_t snm1,
		int32_t on,
		int32_t sn)
{
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + on + sn + 4, b + onm2, snm2 + 4);
	memmove(b + onm2, b + onm1, snm2 + snm1 + sn + 12);
	memset(b + on + sn + 4, 0, snm2 + 4);
}

void ose_rot(ose_bundle bundle)
{
	ose_assert(ose_getBundleElemCount(bundle) >= 3);
	int32_t onm2 = 0;
	int32_t snm2 = 0;
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be3(bundle, &onm2, &snm2, &onm1, &snm1, &on, &sn);
	ose_rot_impl(bundle, onm2, snm2, onm1, snm1, on, sn);
}

static void ose_swap_impl(ose_bundle bundle,
		 int32_t offset_nm1,
		 int32_t size_nm1,
		 int32_t offset_n,
		 int32_t size_n)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 2) == OSETT_TRUE);
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + offset_n + size_n + 4, b + offset_nm1, size_nm1 + 4);
	memmove(b + offset_nm1, b + offset_n, size_nm1 + size_n + 8);
	memset(b + offset_n + size_n + 4, 0, size_nm1 + 4);
}

void ose_swap(ose_bundle bundle)
{
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be2(bundle, &onm1, &snm1, &on, &sn);
	ose_swap_impl(bundle, onm1, snm1, on, sn);
}

void ose_tuck(ose_bundle bundle)
{
	int32_t onm1 = 0;
	int32_t snm1 = 0;
	int32_t on = 0;
	int32_t sn = 0;
	be2(bundle, &onm1, &snm1, &on, &sn);
	ose_swap_impl(bundle, onm1, snm1, on, sn);
	over(bundle, onm1, sn, onm1 + sn + 4, snm1);
}

/**************************************************
 * Grouping / Ungrouping
 **************************************************/

void ose_bundleAll(ose_bundle bundle)
{
	int32_t s = ose_readInt32(bundle, -4);
	char *b = ose_getBundlePtr(bundle);
	memmove(b + OSE_BUNDLE_HEADER_LEN + 4 + OSE_BUNDLE_HEADER_LEN,
		b + OSE_BUNDLE_HEADER_LEN,
		s - OSE_BUNDLE_HEADER_LEN);
	ose_writeInt32(bundle, OSE_BUNDLE_HEADER_LEN, s);
	memcpy(b + OSE_BUNDLE_HEADER_LEN + 4,
	       OSE_BUNDLE_HEADER,
	       OSE_BUNDLE_HEADER_LEN);
	ose_addToSize(bundle, 4 + OSE_BUNDLE_HEADER_LEN);
}

void ose_bundleFromBottom(ose_bundle bundle)
{
	ose_assert(ose_isIntegerType(ose_peekMessageArgType(bundle))
		   == OSETT_TRUE);
	int32_t s = ose_readInt32(bundle, -4);
	char *b = ose_getBundlePtr(bundle);
	int32_t n = ose_popInt32(bundle);
	ose_assert(ose_bundleHasAtLeastNElems(bundle, n) == OSETT_TRUE);
	int32_t oo = OSE_BUNDLE_HEADER_LEN;
	for(int i = 0; i < n; i++){
		int32_t ss = ose_readInt32(bundle, oo);
		oo += ss + 4;
	}
	memmove(b + OSE_BUNDLE_HEADER_LEN + 4 + OSE_BUNDLE_HEADER_LEN,
		b + OSE_BUNDLE_HEADER_LEN,
		s - OSE_BUNDLE_HEADER_LEN);
	ose_writeInt32(bundle, OSE_BUNDLE_HEADER_LEN, oo);
	memcpy(b + OSE_BUNDLE_HEADER_LEN + 4,
	       OSE_BUNDLE_HEADER,
	       OSE_BUNDLE_HEADER_LEN);
	ose_addToSize(bundle, 4 + OSE_BUNDLE_HEADER_LEN);
}

void ose_bundleFromTop(ose_bundle bundle)
{
	ose_assert(ose_isIntegerType(ose_peekMessageArgType(bundle))
		   == OSETT_TRUE);
	char *b = ose_getBundlePtr(bundle);
	int32_t n = ose_popInt32(bundle);
	ose_assert(ose_bundleHasAtLeastNElems(bundle, n) == OSETT_TRUE);
	ose_countElems(bundle);
	int32_t nmsgs = ose_popInt32(bundle);
	ose_assert(n <= nmsgs);
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	while(n < nmsgs){
		int32_t s = ose_readInt32(bundle, o);
		o += s + 4;
		nmsgs--;
	}
	int32_t ss = 0;
	int32_t oo = o;
	while(nmsgs > 0){
		int32_t s = ose_readInt32(bundle, o);
		o += s + 4;
		nmsgs--;
		ss += s + 4;
	}
	memmove(b + oo + 4 + OSE_BUNDLE_HEADER_LEN,
		b + oo,
		ss);
	ose_writeInt32(bundle, oo, ss + OSE_BUNDLE_HEADER_LEN);
	memcpy(b + oo + 4,
	       OSE_BUNDLE_HEADER,
	       OSE_BUNDLE_HEADER_LEN);
	ose_addToSize(bundle, 4 + OSE_BUNDLE_HEADER_LEN);
}

void ose_clear(ose_bundle bundle)
{
	int32_t s = ose_readInt32(bundle, -4);
	memset(ose_getBundlePtr(bundle) + OSE_BUNDLE_HEADER_LEN,
	       0,
	       s - OSE_BUNDLE_HEADER_LEN);
	ose_addToSize(bundle, -(s - OSE_BUNDLE_HEADER_LEN));
}

void ose_clearPayload(ose_bundle bundle)
{

}

void ose_pop(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	char *b = ose_getBundlePtr(bundle);
	switch(ose_getBundleElemType(bundle, o)){
	case OSETT_MESSAGE: {
		int32_t tto, ntt, lto, plo, lpo;
		ose_getNthPayloadItem(bundle, 1, o,
				      &tto, &ntt, &lto, &plo, &lpo);
		switch(ose_readByte(bundle, lto)){
		case OSETT_ID:
			ose_pushString(bundle, b + o + 4);
			ose_swap(bundle);
			ose_drop(bundle);
			break;
		default: {
			int32_t s = ose_readInt32(bundle, o);
			int32_t data_size = s - (lpo - (o + 4));
			char tt = ose_readByte(bundle, lto);
			memmove(b + lpo + 4 + OSE_ADDRESS_ANONVAL_SIZE + 4,
				b + lpo,
				data_size);
			*((int32_t *)(b + lpo)) =
				ose_htonl(OSE_ADDRESS_ANONVAL_SIZE
					  + 4
					  + data_size);
			strncpy(b + lpo + 4,
				OSE_ADDRESS_ANONVAL,
				OSE_ADDRESS_ANONVAL_SIZE);
			b[lpo + 4 + OSE_ADDRESS_ANONVAL_SIZE] = OSETT_ID;
			b[lpo + 4 + OSE_ADDRESS_ANONVAL_SIZE + 1] = tt;
			b[lpo + 4 + OSE_ADDRESS_ANONVAL_SIZE + 2] = 0;
			b[lpo + 4 + OSE_ADDRESS_ANONVAL_SIZE + 3] = 0;
			b[lto] = 0;
			int32_t n = data_size;
			int32_t nn = 0;
			if(ose_pnbytes(ntt) != ose_pnbytes(ntt - 1)){
				nn = 4;
				int32_t tto = lto + 1;
				int32_t x=(lpo
					   + 8
					   + OSE_ADDRESS_ANONVAL_SIZE
					   + data_size);
				memmove(b + tto, b + tto + 4, x);
				memset(b + x - 4, 0, 4);
			}
			ose_addToInt32(bundle, o, -(n + nn));
			ose_addToSize(bundle, 8 + OSE_ADDRESS_ANONVAL_SIZE - nn);
		}
		}
		break;
	}
	case OSETT_BUNDLE: {
		int32_t s = ose_readInt32(bundle, o);
		if(s <= 16){
			ose_addToSize(bundle, -20);
			char *b = b;
			memset(b + o, 0, 20);
		}else{
			int32_t oo = o + 20;
			int32_t ss = ose_readInt32(bundle, oo);
			while((oo + ss + 4) - (o + 4) < s){
				oo += ss + 4;
				ss = ose_readInt32(bundle, oo);
			}
			ose_addToInt32(bundle, o, -(ss + 4));
		}
		break;
	}
	default:
		assert(0 &&
		       "found something that is neither a bundle nor a message");
	}
}

static void popAll_bundle(ose_bundle bundle, int32_t o)
{
	int32_t s = ose_readInt32(bundle, o);
	ose_addToSize(bundle, s - OSE_BUNDLE_HEADER_LEN);
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + o + s + 4,
	       b + o + 4 + OSE_BUNDLE_HEADER_LEN,
	       s - OSE_BUNDLE_HEADER_LEN);
	int32_t o1 = (o + s + 4) - (OSE_BUNDLE_HEADER_LEN + 4);
	ose_writeInt32(bundle, o1, OSE_BUNDLE_HEADER_LEN);
	memcpy(b + o1 + 4, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);

	int32_t o2 = o + s + 4;
	int32_t s2 = ose_readInt32(bundle, o2);
	o1 -= (s2 + 4);
	while(o1 >= o){
		memcpy(b + o1, b + o2, s2 + 4);
		o2 += s2 + 4;
		s2 = ose_readInt32(bundle, o2);
		o1 -= s2 + 4;
	}
	memset(b + o + s + 4, 0, s - OSE_BUNDLE_HEADER_LEN);
	ose_addToSize(bundle, -(s - OSE_BUNDLE_HEADER_LEN));
}

static void popAll_message(ose_bundle bundle, int32_t o)
{
	char *b = ose_getBundlePtr(bundle);
	int32_t s = ose_readInt32(bundle, o);
	int32_t ao = o + 4;
	int32_t as = ose_getPaddedStringLen(bundle, ao);
	int32_t to = ao + as;
	int32_t n = strlen(b + to);
	int32_t po = to + ose_pnbytes(n);
	int32_t ps = s - ((po - o) - 4);
	int32_t nbytes = 8
		+ as + ps + ((OSE_ADDRESS_ANONVAL_SIZE + 8) * (n - 1));
	ose_addToSize(bundle, nbytes);
	to++;
	int32_t oo = o;
	o += (s + 4 + nbytes) - (4 + as + 4);
	ose_writeInt32(bundle, o, as + 4);
	memcpy(b + o + 4, b + ao, as);
	ose_writeByte(bundle, o + 4 + as, OSETT_ID);
	for(int i = 0; i < n - 1; i++){
		char tt = ose_readByte(bundle, to);
		int32_t is = ose_getPayloadItemSize(bundle, tt, po);
		o -= is;
		memcpy(b + o, b + po, is);
		o -= 4;
		ose_writeByte(bundle, o, OSETT_ID);
		ose_writeByte(bundle, o + 1, tt);
#ifdef OSE_USER_ADDRESS_ANONVAL
		o -= OSE_ADDRESS_ANONVAL_SIZE;
		memcpy(b + o, OSE_ADDRESS_ANONVAL, OSE_ADDRESS_ANONVAL_SIZE);
		o -= 4;
#else
		o -= (4 + OSE_ADDRESS_ANONVAL_SIZE);
#endif
		ose_writeInt32(bundle, o, OSE_ADDRESS_ANONVAL_SIZE + 4 + is);
		to++;
		po += is;
	}
	memmove(b + oo, b + oo + s + 4, nbytes);
	memset(b + oo + s + 4 + nbytes, 0, s + 4);
	ose_addToSize(bundle, -(s + 4));
}

void ose_popAll(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	if(ose_getBundleElemType(bundle, o) == OSETT_BUNDLE){
		popAll_bundle(bundle, o);
	}else{
		popAll_message(bundle, o);		
	}
}

void ose_popAllDrop(ose_bundle bundle)
{
	ose_popAll(bundle);
	ose_drop(bundle);
}

void ose_popAllBundle(ose_bundle bundle)
{
	ose_pushBundle(bundle);
	ose_swap(bundle);
	int32_t onm1, snm1, on, sn;
	be2(bundle, &onm1, &snm1, &on, &sn);
	ose_popAll(bundle);
	int32_t bs = ose_readInt32(bundle, -4) - onm1 - 4;
	ose_writeInt32(bundle, onm1, bs);
}

void ose_popAllDropBundle(ose_bundle bundle)
{
	ose_pushBundle(bundle);
	ose_swap(bundle);
	int32_t onm1, snm1, on, sn;
	be2(bundle, &onm1, &snm1, &on, &sn);
	ose_popAllDrop(bundle);
	int32_t bs = ose_readInt32(bundle, -4) - onm1 - 4;
	ose_writeInt32(bundle, onm1, bs);
}

void ose_push(ose_bundle bundle)
{
	int32_t s = ose_readInt32(bundle, -4);
	char *b = ose_getBundlePtr(bundle);
	if(s <= 16){
		ose_writeInt32(bundle, 16, OSE_BUNDLE_HEADER_LEN);
		memcpy(b + 20, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
		ose_addToSize(bundle, 4 + OSE_BUNDLE_HEADER_LEN);
	}else if(s == 16 + ose_readInt32(bundle, 16) + 4){
		memmove(b + 36, b + 16, s - 16);
		ose_writeInt32(bundle, 16, s);
		memcpy(b + 20, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
		ose_addToSize(bundle, 20);
	}else{
		int32_t o2 = 16;
		int32_t s2 = ose_readInt32(bundle, o2);
		int32_t o1 = o2;
		int32_t s1 = s2;
		while(o2 + s2 + 4 < s){
			o1 = o2;
			s1 = s2;
			o2 += s2 + 4;
			s2 = ose_readInt32(bundle, o2);
		}
		char t1 = ose_getBundleElemType(bundle, o1);
		char t2 = ose_getBundleElemType(bundle, o2);
		if(t1 == OSETT_BUNDLE){
			ose_addToInt32(bundle, o1, s2 + 4);
		}else if(t1 == OSETT_MESSAGE){
			if(t2 == OSETT_BUNDLE){
				int32_t tto1 =
					o1 + 4 +
					ose_getPaddedStringLen(bundle, o1 + 4);
				int32_t plo1 = tto1 +
					ose_getPaddedStringLen(bundle, tto1);
				int32_t ntt1 = strlen(b + tto1);
				if(ose_pnbytes(ntt1) != ose_pnbytes(ntt1 + 1)){
					memmove(b + plo1 + 4,
						b + plo1,
						(s1 - (plo1 - (o1 + 4))) + (s2 + 4));
					memset(b + plo1, 0, 4);
					ose_addToInt32(bundle, o1, 4);
					ose_addToSize(bundle, 4);
					o2 += 4;
					plo1 += 4;
				}
				ose_writeByte(bundle, tto1 + ntt1, OSETT_BLOB);
				int32_t bloblen = ose_readInt32(bundle, o2);
				// no need to pad since it's already
				// 4-byte aligned
				int32_t pbloblen = bloblen;
				ose_addToInt32(bundle, o1, pbloblen + 4);
				if(pbloblen > bloblen){
					ose_addToSize(bundle, 4);
				}
			}else if(t2 == OSETT_MESSAGE){
				int32_t o3 = o2 + s2 + 4;
				int32_t tto1 =
					o1 + 4 +
					ose_getPaddedStringLen(bundle, o1 + 4);
				int32_t ntt1 = strlen(b + tto1);
				int32_t plo1 = tto1 + ose_pnbytes(ntt1);
				int32_t tto2 =
					o2 + 4 +
					ose_getPaddedStringLen(bundle, o2 + 4);
				int32_t ntt2 = strlen(b + tto2);
				int32_t plo2 = tto2 + ose_pnbytes(ntt2);
				int32_t oo = o3 + 4;
				memcpy(b + oo, b + o1 + 4, plo1 - (o1 + 4));
				oo += (tto1 - o1) - 4 + ntt1;
				memcpy(b + oo, b + tto2 + 1, ntt2 - 1);
				oo += ntt2 - 1;
				oo = ose_pnbytes(oo);
				memcpy(b + oo, b + plo1, s1 - ((plo1 - o1) - 4));
				oo += s1 - ((plo1 - o1) - 4);
				memcpy(b + oo, b + plo2, s2 - ((plo2 - o2) - 4));
				oo += s2 - ((plo2 - o2) - 4);
				int32_t s3 = (oo - o3) - 4;
				ose_writeInt32(bundle, o3, s3);
				memmove(b + o1, b + o3, s3 + 4);
				memset(b + o1 + s3 + 4, 0, s2 + s1 + 8);
				ose_addToSize(bundle,
					      (s3 + 4) - (s1 + 4 + s2 + 4));
			}else{
				ose_assert(0 && "found something that is neither \a bundle nor a message");
			}
		}else{
			ose_assert(0 && "found something that is neither a bundle nor a message");
		}		
	}
}

void ose_unpack(ose_bundle bundle)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 1) == OSETT_TRUE);
	int32_t o = ose_getLastBundleElemOffset(bundle);
	if(ose_getBundleElemType(bundle, o) == OSETT_BUNDLE){
		ose_writeInt32(bundle, o, OSE_BUNDLE_HEADER_LEN);
	}else{
		char *b = ose_getBundlePtr(bundle);
		int32_t o = 0, s = 0;
		be1(bundle, &o, &s);
		int32_t to = o + 4;
		to += ose_pstrlen(b + to);
		int32_t po = to + ose_pstrlen(b + to);
		to++;
		ose_pushMessage(bundle, b + o + 4, strlen(b + o + 4), 0);
		char tt = ose_readByte(bundle, to);
		while(tt){
			int32_t ps = ose_getPayloadItemSize(bundle, tt, po);
			switch(tt){
			case OSETT_INT32: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_INT32,
						ose_readInt32(bundle, po));
				break;
			}
			case OSETT_FLOAT: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_FLOAT,
						ose_readFloat(bundle, po));
				break;
			}
			case OSETT_STRING: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_STRING,
						ose_readString(bundle, po));
				break;
			}
			case OSETT_BLOB: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_BLOB,
						ose_readBlobSize(bundle, po),
						b + po + 4);
				break;
			}
#ifdef OSE_PROVIDE_TYPE_SYMBOL
			case OSETT_SYMBOL: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_SYMBOL,
						ose_readString(bundle, po));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_DOUBLE
			case OSETT_DOUBLE: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_DOUBLE,
						ose_readInt32(bundle, po));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
			case OSETT_INT8: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_INT8,
						ose_readInt32(bundle, po));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
			case OSETT_UINT8: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_UINT8,
						ose_readInt32(bundle, po));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
			case OSETT_UINT32: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_UINT32,
						ose_readInt32(bundle, po));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
			case OSETT_INT64: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_INT64,
						ose_readInt32(bundle, po));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
			case OSETT_UINT64: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_UINT64,
						ose_readInt32(bundle, po));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_TIMETAG
			case OSETT_TIMETAG: {
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						OSETT_TIMETAG,
						ose_readInt32(bundle, po),
						ose_readInt32(bundle, po + 4));
				break;
			}
#endif
#ifdef OSE_PROVIDE_TYPE_TRUE
			case OSETT_TRUE:
#endif
#ifdef OSE_PROVIDE_TYPE_TRUE
			case OSETT_FALSE:
#endif
#ifdef OSE_PROVIDE_TYPE_TRUE
			case OSETT_NULL:
#endif
#ifdef OSE_PROVIDE_TYPE_TRUE
			case OSETT_INFINITUM:
#endif
#if defined(OSE_PROVIDE_TYPE_TRUE) ||		\
	defined(OSE_PROVIDE_TYPE_TRUE) ||	\
	defined(OSE_PROVIDE_TYPE_TRUE) ||	\
	defined(OSE_PROVIDE_TYPE_TRUE) 
				ose_pushMessage(bundle,
						OSE_ADDRESS_ANONVAL,
						OSE_ADDRESS_ANONVAL_LEN,
						1,
						tt);
				break;
#endif
			}
			to++;
			tt = ose_readByte(bundle, to);
			po += ps;
		}
		int32_t bs = ose_readInt32(bundle, -4);
		memcpy(b + o, b + o + s + 4, bs - (o + s + 4));
		memset(b + (bs - (s + 4)), 0, s + 4);
		ose_addToSize(bundle, -(s + 4));
	}
}

void ose_unpackDrop(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t n = ose_getBundleElemElemCount(bundle, o);
	ose_unpack(bundle);
	ose_pushInt32(bundle, n);
	ose_roll(bundle);
	ose_drop(bundle);
}

void ose_unpackBundle(ose_bundle bundle)
{

}

void ose_unpackDropBundle(ose_bundle bundle)
{

}

/**************************************************
 * Queries
 **************************************************/

void ose_countElems(ose_bundle bundle)
{
	int32_t s = ose_readInt32(bundle, -4);
	int32_t o = OSE_BUNDLE_HEADER_LEN;
	int32_t n = 0;
	while(o < s){
		n++;
		o += ose_readInt32(bundle, o) + 4;
	}
	ose_pushInt32(bundle, n);
}

void ose_countItems(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t n = 0;
	char t = ose_getBundleElemType(bundle, o);
	if(t == OSETT_BUNDLE){
		int32_t oo = OSE_BUNDLE_HEADER_LEN + 4;
		int32_t ss = ose_readInt32(bundle, o);
		while(oo < ss){
			n++;
			oo += ose_readInt32(bundle, o + oo) + 4;
		}
	}else if(t == OSETT_MESSAGE){
		int32_t to = o + 4 + ose_getPaddedStringLen(bundle, o + 4);
		n = strlen(ose_getBundlePtr(bundle) + to) - 1;
	}else{
		ose_assert(0 &&
			   "found something that is neither a bundle nor a message");
	}
	ose_pushInt32(bundle, n);
}

void ose_lengthAddress(ose_bundle bundle)
{

}

void ose_lengthTT(ose_bundle bundle)
{

}

void ose_lengthItem(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	char tt = ose_readByte(bundle, lto);
        int32_t ps = ose_getPayloadItemLength(bundle, tt, po);
	ose_pushInt32(bundle, ps);
}

void ose_lengthsItems(ose_bundle bundle)
{

}

void ose_sizeAddress(ose_bundle bundle)
{

}

void ose_sizeElem(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t n = ose_getBundleElemElemCount(bundle, o);
	ose_pushInt32(bundle, n);
}

void ose_sizeItem(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	int32_t s = ose_getPayloadItemSize(bundle,
					   ose_readByte(bundle, lto),
					   lpo);
	ose_pushInt32(bundle, s);
}

void ose_sizePayload(ose_bundle bundle)
{

}

void ose_sizesElems(ose_bundle bundle)
{

}

void ose_sizesItems(ose_bundle bundle)
{

}

void ose_sizeTT(ose_bundle bundle)
{

}

/**************************************************
 * Operations on Bundle Elements and Items
 **************************************************/

void ose_blobToElem(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	ose_assert(ose_readByte(bundle, lto) == OSETT_BLOB);
	ose_writeByte(bundle, lto, 0);
	int32_t ps = ose_readInt32(bundle, lpo);
	while(ps % 4){
		ps++;
	}
	ose_addToInt32(bundle, o, -(ps + 4));
	ose_writeInt32(bundle, lpo, ps);
	ose_nip(bundle);
}

static void blobToType(ose_bundle bundle, char typetag)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	ose_assert(ose_readByte(bundle, lto) == OSETT_BLOB);
	ose_writeByte(bundle, lto, typetag);
	int32_t bs = ose_readInt32(bundle, lpo);
	int32_t pbs = ose_getPaddedBlobSize(bundle, lpo);
	char *b = ose_getBundlePtr(bundle);
	memmove(b + lpo, b + lpo + 4, pbs);
	memset(b + o + ose_readInt32(bundle, o), 0, 4);
	ose_addToInt32(bundle, o, -4);
	ose_addToSize(bundle, -4);
}

void ose_blobToType(ose_bundle bundle)
{
	blobToType(bundle, ose_popInt32(bundle));
}

void ose_concatenateBlobs(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 2, o, &to, &ntt, &lto, &po, &lpo);
	ose_assert(ose_readByte(bundle, lto) == OSETT_BLOB);
	ose_assert(ose_readByte(bundle, lto + 1) == OSETT_BLOB);
	
	int32_t blob2_offset = lpo;
	int32_t blob2_size = ose_readInt32(bundle, blob2_offset);
	int32_t blob2_psize = blob2_size
		+ ose_getBlobPaddingForNBytes(blob2_size);
	
	int32_t blob1_offset = blob2_offset + 4 + blob2_psize;
	int32_t blob1_size = ose_readInt32(bundle, blob1_offset);
	int32_t blob1_psize = blob1_size
		+ ose_getBlobPaddingForNBytes(blob1_size);

	char *b = ose_getBundlePtr(bundle);
	memmove(b + blob2_offset + 4 + blob2_size,
		b + blob1_offset + 4,
		blob1_psize);

	int32_t newblob_size = blob2_size + blob1_size;
	int32_t newblob_psize = newblob_size
		+ ose_getBlobPaddingForNBytes(newblob_size);
	ose_writeInt32(bundle, blob2_offset, newblob_size);
	ose_writeInt32(bundle,
		       blob2_offset + 4 + newblob_psize,
		       0);
	ose_pop(bundle);
	ose_drop(bundle);
}

void ose_concatenateStrings(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, to2, po, po2;
	ose_getNthPayloadItem(bundle, 2, o, &to, &ntt, &to2, &po, &po2);
	int32_t po1 = po2 + ose_getPayloadItemSize(bundle,
						   ose_readByte(bundle, to2),
						   po2);
	int32_t to1 = to2 + 1;
	ose_assert(ose_isStringType(ose_readByte(bundle, to2)) == OSETT_TRUE &&
		   ose_isStringType(ose_readByte(bundle, to1)) == OSETT_TRUE);
	char *b = ose_getBundlePtr(bundle);
	int32_t s2len = strlen(b + po2);
	int32_t s1len = strlen(b + po1);
	memmove(b + po2 + s2len, b + po1, s1len);
	int32_t news2len = s2len + s1len;
	memset(b + po2 + news2len, 0, po1 - (po2 + s2len));
	int32_t oldsize = ose_readInt32(bundle, o);
	int32_t newsize = ((oldsize -
			    (ose_pnbytes(s2len) + ose_pnbytes(s1len))) +
			   (ose_pnbytes(news2len)));
	ose_writeByte(bundle, to1, 0);	
	if(ose_pnbytes(ntt) != ose_pnbytes(ntt - 1)){
		memmove(b + to1 + 1, b + to1 + 5, newsize - (po - (o + 4)));
		memset(b + o + newsize + 4, 0, 4);
		newsize -= 4;
	}
	ose_writeInt32(bundle, o, newsize);
	ose_addToSize(bundle, newsize - oldsize);
}

void ose_copyAddressToString(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	ose_assert(ose_getBundleElemType(bundle, o) == OSETT_MESSAGE);
	ose_pushString(bundle, ose_readString(bundle, o + 4));
}

void ose_copyPayloadToBlob(ose_bundle bundle)
{

}

// take a string at the top of the stack (top arg of the top message) and
// make it the address of the message:

// #bundle
//  ...
//  /foo ... "/bar"
//  toAddress

// |
// V

// #bundle
//  ...
//  /bar ...
static void swapStringToAddress(ose_bundle bundle)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 1) == OSETT_TRUE);
	int32_t so = ose_getLastBundleElemOffset(bundle);
	int32_t s = ose_readInt32(bundle, so);
	int32_t ao = so + 4;
	int32_t len1 = ose_getPaddedStringLen(bundle, so + 4);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, so, &to, &ntt, &lto, &po, &lpo);
	ose_assert(ose_isStringType(ose_readByte(bundle, lto)) == OSETT_TRUE);
	int32_t len2 = ose_getPaddedStringLen(bundle, lpo);
	char *b = ose_getBundlePtr(bundle);
	int32_t o = so + 4;

	o /= 4;
	s /= 4;
	len1 /= 4;
	len2 /= 4;
	int32_t *bb = (int32_t *)b;
	for(int i = 0; i < s / 2; i++){
		int32_t c = __builtin_bswap32(bb[o + i]);
		bb[o + i] = __builtin_bswap32(bb[(o + s - 1) - i]);
		bb[(o + s - 1) - i] = c;
	}
	if(s % 2){
		bb[o + s / 2] = __builtin_bswap32(bb[o + s / 2]);
	}
	for(int i = 0; i < len2 / 2; i++){
		int32_t c = __builtin_bswap32(bb[o + i]);
		bb[o + i] = __builtin_bswap32(bb[(o + len2 - 1) - i]);
		bb[(o + len2 - 1) - i] = c;
	}
	if(len2 % 2){
		bb[o + len2 / 2] = __builtin_bswap32(bb[o + len2 / 2]);
	}
	int32_t len3 = s - (len1 + len2);
	for(int i = 0; i < len3 / 2; i++){
		int32_t c = __builtin_bswap32(bb[o + len2 + i]);
		bb[o + len2 + i] = __builtin_bswap32(bb[(o + len2 + len3 - 1) - i]);
		bb[(o + len2 + len3 - 1) - i] = c;
	}
	if(len3 % 2){
		bb[o + len2 + len3 / 2] = __builtin_bswap32(bb[o + len2 + len3 / 2]);
	}
	for(int i = 0; i < len1 / 2; i++){
		int32_t c = __builtin_bswap32(bb[o + len2 + len3 + i]);
		bb[o + len2 + len3 + i] = __builtin_bswap32(bb[(o + len2 + len3 + len1 - 1) - i]);
		bb[(o + len2 + len3 + len1 - 1) - i] = c;
	}
	if(len1 % 2){
		bb[o + len2 + len3 + len1 / 2] = __builtin_bswap32(bb[o + len2 + len3 + len1 / 2]);
	}

	
	// int32_t old_alen = to - ao;
	// //char *b = ose_getBundlePtr(bundle);
	// to = lto;
	// po = lpo;
	// int32_t new_alen = ose_getPaddedStringLen(bundle, po);
	// int32_t omn = old_alen - new_alen;
	// int32_t nmo = new_alen - old_alen;
	// if(new_alen == old_alen){
	// 	memcpy(b + ao, b + po, new_alen);
	// }else if(new_alen < old_alen){
	// 	memcpy(b + ao, b + po, new_alen);
	// 	memmove(b + ao + new_alen, b + ao + old_alen, s - old_alen);
	// 	memset(b + s + + 4 - (omn), 0, omn);
	// 	ose_addToInt32(bundle, so, nmo);
	// 	ose_addToSize(bundle, nmo);
	// }else{
	// 	memcpy(b + ao + new_alen, b + ao + old_alen, s - old_alen);
	// 	memcpy(b + ao, b + po + (nmo), new_alen);
	// 	ose_addToInt32(bundle, so, nmo);
	// 	ose_addToSize(bundle, nmo);
	// }
}

void ose_swapStringToAddress(ose_bundle bundle)
{
	swapStringToAddress(bundle);
}

void ose_copyTTToBlob(ose_bundle bundle)
{

}

static void decatenateBlobFromEnd(ose_bundle bundle, int32_t n)
{
	ose_assert(n >= 0);
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	ose_assert(ose_readByte(bundle, lto) == OSETT_BLOB);

	int32_t old_blob1_size = ose_readInt32(bundle, lpo);
	int32_t new_blob1_size = old_blob1_size - n;

	int32_t blob2_size = n;

	ose_pushBlob(bundle, new_blob1_size, NULL);
	int32_t blob1_offset = ose_getLastBundleElemOffset(bundle)
		+ OSE_ADDRESS_ANONVAL_SIZE + 4 + 4;
	ose_pushBlob(bundle, blob2_size, NULL);
	int32_t blob2_offset = ose_getLastBundleElemOffset(bundle)
		+ OSE_ADDRESS_ANONVAL_SIZE + 4 + 4;

	char *b = ose_getBundlePtr(bundle);
	memcpy(b + blob1_offset + 4,
	       b + lpo + 4,
	       new_blob1_size);
	memcpy(b + blob2_offset + 4,
	       b + lpo + 4 + new_blob1_size,
	       blob2_size);
	ose_push(bundle);
	ose_nip(bundle);
}

void ose_decatenateBlobFromEnd(ose_bundle bundle)
{
	ose_rassert(ose_peekType(bundle) == OSETT_MESSAGE, 1);
	ose_rassert(ose_peekMessageArgType(bundle) == OSETT_INT32, 1);
	int32_t n = ose_popInt32(bundle);
	ose_rassert(ose_peekMessageArgType(bundle) == OSETT_BLOB, 1);
	decatenateBlobFromEnd(bundle, n);
}

void ose_decatenateBlobFromStart(ose_bundle bundle)
{
	ose_rassert(ose_peekType(bundle) == OSETT_MESSAGE, 1);
	ose_rassert(ose_peekMessageArgType(bundle) == OSETT_INT32, 1);
	int32_t n = ose_popInt32(bundle);
	ose_rassert(ose_peekMessageArgType(bundle) == OSETT_BLOB, 1);
	int32_t bloblen = ose_ntohl(*((int32_t *)ose_peekBlob(bundle)));
	n = bloblen - n;
	decatenateBlobFromEnd(bundle, n);
}

static void decatenateStringFromEnd(ose_bundle bundle, int32_t n)
{
	ose_assert(n >= 0);
	ose_pushString(bundle, "");
	ose_push(bundle);
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 2, o, &to, &ntt, &lto, &po, &lpo);
	to = lto;
	po = lpo;
	int32_t len = ose_getStringLen(bundle, po);
	int32_t plen = ose_pnbytes(len);
	char *b = ose_getBundlePtr(bundle);
	int32_t src = po + (len - n);
	int32_t dest = ose_pnbytes(po + (len - n));
	memmove(b + dest, b + src, n);
	memset(b + src, 0, dest - src);
	int32_t d = (ose_pnbytes(len - n) + ose_pnbytes(n)) - (plen + 4);
	ose_addToInt32(bundle, o, d);
	ose_addToSize(bundle, d);
}

void ose_decatenateStringFromEnd(ose_bundle bundle)
{
	ose_rassert(ose_peekType(bundle) == OSETT_MESSAGE, 1);
	ose_rassert(ose_peekMessageArgType(bundle) == OSETT_INT32, 1);
	int32_t n = ose_popInt32(bundle);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(bundle)) == OSETT_TRUE, 1);
	decatenateStringFromEnd(bundle, n);
}

void ose_decatenateStringFromStart(ose_bundle bundle)
{
	ose_rassert(ose_peekType(bundle) == OSETT_MESSAGE, 1);
	ose_rassert(ose_peekMessageArgType(bundle) == OSETT_INT32, 1);
	int32_t n = ose_popInt32(bundle);
	ose_rassert(ose_isStringType(ose_peekMessageArgType(bundle)) == OSETT_TRUE, 1);
	int32_t stringlen = strlen(ose_peekString(bundle));
	n = stringlen - n;
	decatenateStringFromEnd(bundle, n);
}

void ose_elemToBlob(ose_bundle bundle)
{
	ose_pushString(bundle, OSE_ADDRESS_ANONVAL);
	ose_moveStringToAddress(bundle);
	ose_swap(bundle);
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t s = ose_readInt32(bundle, o);
	ose_writeByte(bundle, o - 3, OSETT_BLOB);
	ose_addToInt32(bundle, o - (8 + OSE_ADDRESS_ANONVAL_SIZE), s + 4);
}

void ose_itemToBlob(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t s = ose_readInt32(bundle, o);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	ose_addToSize(bundle, 4);
	ose_writeByte(bundle, lto, OSETT_BLOB);
	int32_t datasize = s - (lpo - (o + 4));
	char *b = ose_getBundlePtr(bundle);
	memmove(b + lpo + 4,
		b + lpo,
		datasize);
	ose_writeInt32(bundle, lpo, datasize);
	ose_addToInt32(bundle, o, 4);
}

void ose_joinStrings(ose_bundle bundle)
{
	ose_rassert(ose_bundleHasAtLeastNElems(bundle, 3) == OSETT_TRUE, 1);
	int32_t onm2, snm2, onm1, snm1, on, sn;
	be3(bundle, &onm2, &snm2, &onm1, &snm1, &on, &sn);
	int32_t tonm2, nttnm2, ltonm2, ponm2, lponm2;
	int32_t tonm1, nttnm1, ltonm1, ponm1, lponm1;
	int32_t ton, nttn, lton, pon, lpon;
	ose_getNthPayloadItem(bundle, 1, onm2,
			      &tonm2, &nttnm2, &ltonm2, &ponm2, &lponm2);
	ose_getNthPayloadItem(bundle, 1, onm1,
			      &tonm1, &nttnm1, &ltonm1, &ponm1, &lponm1);
	ose_getNthPayloadItem(bundle, 1, on,
			      &ton, &nttn, &lton, &pon, &lpon);
	ose_rassert(ose_isStringType(ose_readByte(bundle, ltonm2)) == OSETT_TRUE, 1);
	ose_rassert(ose_isStringType(ose_readByte(bundle, ltonm1)) == OSETT_TRUE, 1);
	ose_rassert(ose_isStringType(ose_readByte(bundle, lton)) == OSETT_TRUE, 1);
	ose_rassert(nttnm1 == 2, 1);
	ose_rassert(nttn == 2, 1);
	ose_swap(bundle);
	ose_push(bundle);
	ose_push(bundle);
	ose_concatenateStrings(bundle);
	ose_concatenateStrings(bundle);
}

void ose_moveStringToAddress(ose_bundle bundle)
{
	swapStringToAddress(bundle);
	dropArg(bundle);
}

void ose_splitStringFromEnd(ose_bundle bundle)
{
	int32_t onm1, snm1, on, sn;
	be2(bundle, &onm1, &snm1, &on, &sn);
	int32_t nm1_to, nm1_ntt, nm1_lto, nm1_po, nm1_lpo;
	ose_getNthPayloadItem(bundle, 1, onm1,
			      &nm1_to,
			      &nm1_ntt,
			      &nm1_lto,
			      &nm1_po,
			      &nm1_lpo);
	int32_t n_to, n_ntt, n_lto, n_po, n_lpo;
	ose_getNthPayloadItem(bundle, 1, on,
			      &n_to,
			      &n_ntt,
			      &n_lto,
			      &n_po,
			      &n_lpo);
        onm1 = nm1_lpo;
	on = n_lpo;
	ose_over(bundle);
	//ose_swap(bundle);
	char *b = ose_getBundlePtr(bundle);
	char *str = b + onm1;
	char *sep = b + on;
	char *s = ose_peekString(bundle);
	int32_t slen = strlen(s);
	char *ltok = str;
	char *tok = NULL;
	while(ltok - str < slen && (ltok = strstr(ltok, sep))){
		tok = ltok;
		ltok++;
	}
	if(!tok){
		ose_drop(bundle);
		return;
	}
	int32_t n = tok - str;
	if(n == 0){
		n = 1;
	}
	ose_pushInt32(bundle, slen - n);
	ose_decatenateStringFromEnd(bundle);
	//slen -= n;
	ose_rot(bundle);
	ose_drop(bundle);
	ose_pop(bundle);
	ose_swap(bundle);
	ose_rot(bundle);
}

void ose_splitStringFromStart(ose_bundle bundle)
{
	int32_t onm1, snm1, on, sn;
	be2(bundle, &onm1, &snm1, &on, &sn);
	int32_t nm1_to, nm1_ntt, nm1_lto, nm1_po, nm1_lpo;
	ose_getNthPayloadItem(bundle, 1, onm1,
			      &nm1_to,
			      &nm1_ntt,
			      &nm1_lto,
			      &nm1_po,
			      &nm1_lpo);
	int32_t n_to, n_ntt, n_lto, n_po, n_lpo;
	ose_getNthPayloadItem(bundle, 1, on,
			      &n_to,
			      &n_ntt,
			      &n_lto,
			      &n_po,
			      &n_lpo);
        onm1 = nm1_lpo;
	on = n_lpo;
	ose_over(bundle);
	//ose_swap(bundle);
	char *b = ose_getBundlePtr(bundle);
	char *str = b + onm1;
	char *sep = b + on;
	char *s = ose_peekString(bundle);
	int32_t slen = strlen(s);
	char *ltok = str;
	char *tok = strstr(str, sep);
	if(!tok){
		ose_drop(bundle);
		return;
	}
	int32_t n = tok - ltok;
	if(n == 0){
		ltok = tok;
		tok = strstr(tok + 1, sep);
		if(!tok){
			n = 1;
		}else{
			n = tok - ltok;
		}
	}
	ose_pushInt32(bundle, slen - n);
	ose_decatenateStringFromEnd(bundle);
	//slen -= n;
	ose_rot(bundle);
	ose_drop(bundle);
	ose_pop(bundle);
	ose_rot(bundle);
}

static void swap4Bytes(ose_bundle bundle, int32_t o)
{
	char *b = ose_getBundlePtr(bundle);
	char c = b[o - 1];
	b[o - 1] = b[o - 4];
	b[o - 4] = c;
	c = b[o - 2];
	b[o - 2] = b[o - 3];
	b[o - 3] = c;
}

void ose_swap4Bytes(ose_bundle bundle)
{
	swap4Bytes(bundle, ose_readInt32(bundle, -4));
}

static void swap8Bytes(ose_bundle bundle, int32_t o)
{
	char *b = ose_getBundlePtr(bundle);
	char c = b[o - 1];
	b[o - 1] = b[o - 8];
	b[o - 8] = c;
	c = b[o - 2];
	b[o - 2] = b[o - 7];
	b[o - 7] = c;
	c = b[o - 3];
	b[o - 3] = b[o - 6];
	b[o - 6] = c;
	c = b[o - 4];
	b[o - 4] = b[o - 5];
	b[o - 5] = c;
}

void ose_swap8Bytes(ose_bundle bundle)
{
	swap8Bytes(bundle, ose_readInt32(bundle, -4));
}

static void swapNBytes(ose_bundle bundle, int32_t o, int32_t n)
{
	ose_assert(n >= 0);
	char *b = ose_getBundlePtr(bundle);
	for(int i = 1; i <= n; i++){
		char c = b[o - i];
		b[o - i] = b[o - (n - i - 1)];
		b[o - (n - i - 1)] = c;
	}
}

void ose_swapNBytes(ose_bundle bundle)
{
	swapNBytes(bundle, ose_readInt32(bundle, -4), ose_popInt32(bundle));
}

void ose_trimStringEnd(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	ose_assert(ose_isStringType(ose_readByte(bundle, lto)) == OSETT_TRUE);
	int32_t s = ose_getStringLen(bundle, lpo);
	char *p = ose_readString(bundle, lpo);
	int32_t i = s - 1;
	while(i >= 0){
		if(p[i] <= 32 || p[i] >= 127){
			p[i] = 0;
		}else{
			break;
		}
		i--;
	}
	int32_t d = -(ose_pnbytes(s) - ose_pnbytes(i));
	ose_addToInt32(bundle, o, d);
	ose_addToSize(bundle, d);
}

void ose_trimStringStart(ose_bundle bundle)
{
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	ose_assert(ose_isStringType(ose_readByte(bundle, lto)) == OSETT_TRUE);
	int32_t s = ose_getStringLen(bundle, lpo);
	char *p = ose_readString(bundle, lpo);
	int32_t i = 0;
	while(i < s){
		if(p[i] <= 32 || p[i] >= 127){
			;
		}else{
			break;
		}
		i++;
	}
	int32_t d = -(ose_pnbytes(s) - ose_pnbytes(i));
	memmove(p, p + i, s - i);
	memset(p + (s - i), 0, s - i);
	ose_addToInt32(bundle, o, d);
	ose_addToSize(bundle, d);
}

void ose_match(ose_bundle bundle)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 2) == OSETT_TRUE);
	int32_t onm1, snm1, on, sn;
	int32_t tonm1, nttnm1, ltonm1, ponm1, lponm1;
	int32_t ton, nttn, lton, pon, lpon;
	be2(bundle, &onm1, &snm1, &on, &sn);
	ose_getNthPayloadItem(bundle, 1,
			      onm1,
			      &tonm1,
			      &nttnm1,
			      &ltonm1,
			      &ponm1,
			      &lponm1);
	ose_getNthPayloadItem(bundle, 1,
			      on,
			      &ton,
			      &nttn,
			      &lton,
			      &pon,
			      &lpon);
	ose_assert(ose_isStringType(ose_readByte(bundle, ltonm1)) == OSETT_TRUE
		   && ose_isStringType(ose_readByte(bundle, lton)) == OSETT_TRUE);
	char *b = ose_getBundlePtr(bundle);
	if(strcmp(b + lponm1, b + lpon)){
		ose_pushInt32(bundle, 0);
	}else{
		ose_pushInt32(bundle, 1);
	}
}

void ose_pmatch(ose_bundle bundle)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 2) == OSETT_TRUE);
	int32_t onm1, snm1, on, sn;
	int32_t tonm1, nttnm1, ltonm1, ponm1, lponm1;
	int32_t ton, nttn, lton, pon, lpon;
	be2(bundle, &onm1, &snm1, &on, &sn);
	ose_getNthPayloadItem(bundle, 1,
			      onm1,
			      &tonm1,
			      &nttnm1,
			      &ltonm1,
			      &ponm1,
			      &lponm1);
	ose_getNthPayloadItem(bundle, 1,
			      on,
			      &ton,
			      &nttn,
			      &lton,
			      &pon,
			      &lpon);
	ose_assert(ose_isStringType(ose_readByte(bundle, ltonm1)) == OSETT_TRUE
		   && ose_isStringType(ose_readByte(bundle, lton)) == OSETT_TRUE);
	char *b = ose_getBundlePtr(bundle);
	int po = 0, ao = 0;
	int r = ose_match_pattern(b + lponm1, b + lpon, &po, &ao);
	ose_drop(bundle);
	ose_pushInt32(bundle, strlen(ose_peekString(bundle)) - po);
	ose_decatenateStringFromEnd(bundle);
	ose_pop(bundle);
	ose_swap(bundle);
	ose_pushInt32(bundle, (r & OSE_MATCH_PATTERN_COMPLETE) != 0);
	ose_pushInt32(bundle, (r & OSE_MATCH_ADDRESS_COMPLETE) != 0);
}

/**************************************************
 * Creatio Ex Nihilo
 **************************************************/

void ose_makeBlob(ose_bundle bundle)
{
	char t = ose_peekMessageArgType(bundle);
	ose_assert(t == OSETT_INT32);
	int32_t s = ose_popInt32(bundle);
	int32_t sp = s;
	if(sp <= 0){
		sp = 1;
	}
	while(sp % 4){
		sp++;
	}
	int32_t o = ose_readInt32(bundle, -4);
	ose_pushBlob(bundle, s, NULL);
}

void ose_pushBundle(ose_bundle bundle)
{
	int32_t wp = ose_readInt32(bundle, -4);
	ose_writeInt32(bundle, wp, OSE_BUNDLE_HEADER_LEN);
	char *b = ose_getBundlePtr(bundle);
	memcpy(b + wp + 4, OSE_BUNDLE_HEADER, OSE_BUNDLE_HEADER_LEN);
	ose_addToSize(bundle, 4 + OSE_BUNDLE_HEADER_LEN);
}

/**************************************************
 * arithmetic
 **************************************************/

void ose_add(ose_bundle bundle)
{
	ose_countElems(bundle);
	int32_t n = ose_popInt32(bundle);
	ose_rassert(n >= 2, 1);
	char t1 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t1) == OSETT_TRUE);
	ose_swap(bundle);
	char t2 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t2) == OSETT_TRUE);
	ose_assert(t1 == t2);
	switch(t1){
	case OSETT_INT32: {
		int32_t v2 = ose_popInt32(bundle);
		int32_t v1 = ose_popInt32(bundle);
		ose_pushInt32(bundle, v1 + v2);
	}
		break;
	case OSETT_FLOAT: {
		float v2 = ose_popFloat(bundle);
		float v1 = ose_popFloat(bundle);
		ose_pushFloat(bundle, v1 + v2);
	}
		break;
#ifdef OSE_PROVIDE_TYPE_DOUBLE
	case OSETT_DOUBLE: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
	case OSETT_INT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
	case OSETT_UINT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
	case OSETT_UINT32: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
	case OSETT_INT64: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
	case OSETT_UINT64: {

	}
		break;
#endif
	}
}

void ose_sub(ose_bundle bundle)
{
	char t1 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t1) == OSETT_TRUE);
	ose_swap(bundle);
	char t2 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t2) == OSETT_TRUE);
	ose_assert(t1 == t2);
	switch(t1){
	case OSETT_INT32: {
		int32_t v2 = ose_popInt32(bundle);
		int32_t v1 = ose_popInt32(bundle);
		ose_pushInt32(bundle, v1 - v2);
	}
		break;
	case OSETT_FLOAT: {
		float v2 = ose_popFloat(bundle);
		float v1 = ose_popFloat(bundle);
		ose_pushFloat(bundle, v1 - v2);
	}
		break;
#ifdef OSE_PROVIDE_TYPE_DOUBLE
	case OSETT_DOUBLE: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
	case OSETT_INT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
	case OSETT_UINT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
	case OSETT_UINT32: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
	case OSETT_INT64: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
	case OSETT_UINT64: {

	}
		break;
#endif
	}
}

void ose_mul(ose_bundle bundle)
{
	char t1 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t1) == OSETT_TRUE);
	ose_swap(bundle);
	char t2 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t2) == OSETT_TRUE);
	ose_assert(t1 == t2);
	switch(t1){
	case OSETT_INT32: {
		int32_t v2 = ose_popInt32(bundle);
		int32_t v1 = ose_popInt32(bundle);
		ose_pushInt32(bundle, v1 * v2);
	}
		break;
	case OSETT_FLOAT: {
		float v2 = ose_popFloat(bundle);
		float v1 = ose_popFloat(bundle);
		ose_pushFloat(bundle, v1 * v2);
	}
		break;
#ifdef OSE_PROVIDE_TYPE_DOUBLE
	case OSETT_DOUBLE: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
	case OSETT_INT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
	case OSETT_UINT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
	case OSETT_UINT32: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
	case OSETT_INT64: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
	case OSETT_UINT64: {

	}
		break;
#endif
	}
}

void ose_div(ose_bundle bundle)
{
	char t1 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t1) == OSETT_TRUE);
	ose_swap(bundle);
	char t2 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t2) == OSETT_TRUE);
	ose_assert(t1 == t2);
	switch(t1){
	case OSETT_INT32: {
		int32_t v2 = ose_popInt32(bundle);
		int32_t v1 = ose_popInt32(bundle);
		ose_pushInt32(bundle, v1 / v2);
	}
		break;
	case OSETT_FLOAT: {
		float v2 = ose_popFloat(bundle);
		float v1 = ose_popFloat(bundle);
		ose_pushFloat(bundle, v1 / v2);
	}
		break;
#ifdef OSE_PROVIDE_TYPE_DOUBLE
	case OSETT_DOUBLE: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
	case OSETT_INT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
	case OSETT_UINT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
	case OSETT_UINT32: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
	case OSETT_INT64: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
	case OSETT_UINT64: {

	}
		break;
#endif
	}
}

void ose_mod(ose_bundle bundle)
{
	char t1 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t1) == OSETT_TRUE);
	ose_swap(bundle);
	char t2 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t2) == OSETT_TRUE);
	ose_assert(t1 == t2);
	switch(t1){
	case OSETT_INT32: {
		int32_t v2 = ose_popInt32(bundle);
		int32_t v1 = ose_popInt32(bundle);
		ose_pushInt32(bundle, v1 % v2);
	}
		break;
	case OSETT_FLOAT: {
		float v2 = ose_popFloat(bundle);
		float v1 = ose_popFloat(bundle);
		ose_pushFloat(bundle, fmodf(v1, v2));
	}
		break;
#ifdef OSE_PROVIDE_TYPE_DOUBLE
	case OSETT_DOUBLE: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
	case OSETT_INT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
	case OSETT_UINT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
	case OSETT_UINT32: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
	case OSETT_INT64: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
	case OSETT_UINT64: {

	}
		break;
#endif
	}
}

void ose_pow(ose_bundle bundle)
{
	char t1 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t1) == OSETT_TRUE);
	ose_swap(bundle);
	char t2 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t2) == OSETT_TRUE);
	ose_assert(t1 == t2);
	switch(t1){
	case OSETT_INT32: {
		int32_t v2 = ose_popInt32(bundle);
		int32_t v1 = ose_popInt32(bundle);
		ose_pushInt32(bundle, (int32_t)pow(v1, v2));
	}
		break;
	case OSETT_FLOAT: {
		float v2 = ose_popFloat(bundle);
		float v1 = ose_popFloat(bundle);
		ose_pushFloat(bundle, powf(v1, v2));
	}
		break;
#ifdef OSE_PROVIDE_TYPE_DOUBLE
	case OSETT_DOUBLE: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
	case OSETT_INT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
	case OSETT_UINT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
	case OSETT_UINT32: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
	case OSETT_INT64: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
	case OSETT_UINT64: {

	}
		break;
#endif
	}
}

void ose_neg(ose_bundle bundle)
{
	char t1 = ose_peekMessageArgType(bundle);
	ose_assert(ose_isNumericType(t1) == OSETT_TRUE);
	switch(t1){
	case OSETT_INT32: {
		int32_t v1 = ose_popInt32(bundle);
		ose_pushInt32(bundle, -v1);
	}
		break;
	case OSETT_FLOAT: {
		float v1 = ose_popFloat(bundle);
		ose_pushFloat(bundle, -v1);
	}
		break;
#ifdef OSE_PROVIDE_TYPE_DOUBLE
	case OSETT_DOUBLE: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
	case OSETT_INT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
	case OSETT_UINT8: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
	case OSETT_UINT32: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
	case OSETT_INT64: {

	}
		break;
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
	case OSETT_UINT64: {

	}
		break;
#endif
	}
}

void ose_eql(ose_bundle bundle)
{
	int32_t onm1, on, snm1, sn;
	be2(bundle, &onm1, &snm1, &on, &sn);
	int32_t tonm1 = onm1 + 4 + ose_getPaddedStringLen(bundle, onm1 + 4);
	int32_t ton = on + 4 + ose_getPaddedStringLen(bundle, on + 4);
	char *b = ose_getBundlePtr(bundle);
	int32_t lnm1 = snm1 - (tonm1 - (onm1 + 4));
	int32_t ln = sn - (ton - (on + 4));
	if(lnm1 != ln){
		ose_pushInt32(bundle, 0);
		return;
	}
	if(!memcmp(b + tonm1, b + ton, ln)){
		ose_pushInt32(bundle, 1);
	}else{
		ose_pushInt32(bundle, 0);
	}
	ose_swap(bundle);
	ose_drop(bundle);
	ose_swap(bundle);
	ose_drop(bundle);
}

void ose_lte(ose_bundle bundle)
{

}

void ose_lt(ose_bundle bundle)
{

}

void ose_and(ose_bundle bundle)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 2) == OSETT_TRUE);
	int32_t i1 = ose_popInt32(bundle);
	int32_t i2 = ose_popInt32(bundle);
	ose_pushInt32(bundle, i1 && i2);
}

void ose_or(ose_bundle bundle)
{
	ose_assert(ose_bundleHasAtLeastNElems(bundle, 2) == OSETT_TRUE);
	int32_t i1 = ose_popInt32(bundle);
	int32_t i2 = ose_popInt32(bundle);
	ose_pushInt32(bundle, i1 || i2);
}

/**************************************************
 * helper functions
 **************************************************/
static void be1(ose_bundle bundle, int32_t *on, int32_t *sn)
{
	int32_t s = ose_readInt32(bundle, -4);
       	int32_t o1 = OSE_BUNDLE_HEADER_LEN;
	int32_t s1 = ose_readInt32(bundle, o1);
	while(o1 + s1 + 4 < s){
		o1 += s1 + 4;
		s1 = ose_readInt32(bundle, o1);
	}
	*on = o1;
	*sn = s1;
}

static void be2(ose_bundle bundle,
		int32_t *onm1,
		int32_t *snm1,
		int32_t *on,
		int32_t *sn)
{
	int32_t s = ose_readInt32(bundle, -4);
       	int32_t o1 = OSE_BUNDLE_HEADER_LEN;
	int32_t o2 = OSE_BUNDLE_HEADER_LEN;
	int32_t s1 = ose_readInt32(bundle, o1);
	int32_t s2 = s1;
	while(o1 + s1 + 4 < s){
		o2 = o1;
		o1 += s1 + 4;
		s2 = s1;
		s1 = ose_readInt32(bundle, o1);
	}
	*onm1 = o2;
	*snm1 = s2;
	*on = o1;
	*sn = s1;
}

static void be3(ose_bundle bundle,
		int32_t *onm2,
		int32_t *snm2,
		int32_t *onm1,
		int32_t *snm1,
		int32_t *on,
		int32_t *sn)
{
	int32_t s = ose_readInt32(bundle, -4);
       	int32_t o1 = OSE_BUNDLE_HEADER_LEN;
	int32_t s1 = ose_readInt32(bundle, o1);
	int32_t o2 = o1 + s1 + 4;
	int32_t s2 = ose_readInt32(bundle, o2);
	int32_t o3 = o2 + s2 + 4;
	int32_t s3 = ose_readInt32(bundle, o3);
	while(o3 + s3 + 4 < s){
		o1 = o2;
		s1 = ose_readInt32(bundle, o1);
		o2 = o3;
		s2 = ose_readInt32(bundle, o2);
		o3 += s3 + 4;
		s3 = ose_readInt32(bundle, o3);
	}
	*onm2 = o1;
	*snm2 = s1;
	*onm1 = o2;
	*snm1 = s2;
	*on = o3;
	*sn = s3;
}

static void be4(ose_bundle bundle,
		int32_t *onm3,
		int32_t *snm3,
		int32_t *onm2,
		int32_t *snm2,
		int32_t *onm1,
		int32_t *snm1,
		int32_t *on,
		int32_t *sn)
{
	int32_t s = ose_readInt32(bundle, -4);
       	int32_t o1 = OSE_BUNDLE_HEADER_LEN;
	int32_t s1 = ose_readInt32(bundle, o1);
	int32_t o2 = o1 + s1 + 4;
	int32_t s2 = ose_readInt32(bundle, o2);
	int32_t o3 = o2 + s2 + 4;
	int32_t s3 = ose_readInt32(bundle, o3);
	int32_t o4 = o3 + s3 + 4;
	int32_t s4 = ose_readInt32(bundle, o4);
	while(o3 + s3 + 4 < s){
		o1 = o2;
		s1 = ose_readInt32(bundle, o1);
		o2 = o3;
		s2 = ose_readInt32(bundle, o2);
		o3 += s3 + 4;
		s3 = ose_readInt32(bundle, o3);
		o4 += s4 + 4;
		s4 = ose_readInt32(bundle, o4);
	}
	*onm3 = o1;
	*snm3 = s1;
	*onm2 = o2;
	*snm2 = s2;
	*onm1 = o3;
	*snm1 = s3;
	*on = o4;
	*sn = s4;
}

static void dropArg(ose_bundle bundle)
{
	char *b = ose_getBundlePtr(bundle);
	int32_t o = ose_getLastBundleElemOffset(bundle);
	int32_t to, ntt, lto, po, lpo;
	ose_getNthPayloadItem(bundle, 1, o, &to, &ntt, &lto, &po, &lpo);
	int32_t s = ose_readInt32(bundle, o) - (lpo - (o + 4));
	memset(b + lpo, 0, s);
	ose_writeByte(bundle, lto, 0);
	if(ose_pnbytes(ntt) != ose_pnbytes(ntt - 1)){
		memmove(b + to + ntt,
			b + to + ntt + 4,
			(lpo + s) - (to + ntt + 4));
	        s += 4;
		lpo -= 4;
	}
	if(s){
		memset(b + lpo, 0, s);
	}
	ose_addToInt32(bundle, o, -s);
	ose_addToSize(bundle, -s);
}
