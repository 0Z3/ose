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
 * @file ose.h
 * @brief Global definitions and types.
 */

#ifndef OSE_H
#define OSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "ose_conf.h"

#if defined (__unix__) || ( defined (__APPLE__) && defined (__MACH__) )
#include <arpa/inet.h>
#elif defined (_WIN32) || defined (_WIN64)
#include <Windows.h>
#include <Winsock.h>
#else

#ifndef htonl
#define OSE_NEED_HTONL
int32_t htonl(int32_t x);
#define OSE_BYTE_SWAP32(x) \
	((((x) & 0xff000000) >> 24) | \
	 (((x) & 0x00ff0000) >> 8) | \
	 (((x) & 0x0000ff00) << 8) | \
	 (((x) & 0x000000ff) << 24))
#endif

#ifndef ntohl
#define OSE_NEED_NTOHL
int32_t ntohl(int32_t x);
#define OSE_BYTE_SWAP64(x) \
	((((x) & 0xff00000000000000LL) >> 56) | \
	 (((x) & 0x00ff000000000000LL) >> 40) | \
	 (((x) & 0x0000ff0000000000LL) >> 24) | \
	 (((x) & 0x000000ff00000000LL) >> 8)  | \
	 (((x) & 0x00000000ff000000LL) << 8)  | \
	 (((x) & 0x0000000000ff0000LL) << 24) | \
	 (((x) & 0x000000000000ff00LL) << 40) | \
	 (((x) & 0x00000000000000ffLL) << 56))
#endif
#endif


#define ose_htonl(x) htonl(x)
#define ose_ntohl(x) ntohl(x)
#ifndef htonll
#define ose_htonll(x) ((1==ose_htonl(1))				\
		       ? (x)						\
		       : ((uint64_t)ose_htonl((x) & 0xFFFFFFFF) << 32)	\
		       | ose_htonl((x) >> 32))
#else
#define ose_htonll(x) htonll(x)
#endif
#ifndef ntohll
#define ose_ntohll(x) ((1==ose_ntohl(1))				\
		       ? (x)						\
		       : ((uint64_t)ose_ntohl((x) & 0xFFFFFFFF) << 32)	\
		       | ose_ntohl((x) >> 32))
#else
#define ose_ntohll(x) ntohll(x)
#endif

#ifdef OSE_SAFE_BUNDLE
typedef struct _ose_bundle
{
	char *b;
} ose_bundle;
#define ose_makeBundle(charptr) (ose_bundle){charptr}
#define ose_makeConstBundle(charptr) (ose_bundle){(char *)charptr}
#define ose_getBundlePtr(bundle) (bundle).b
#define ose_constbundle ose_bundle
#else
typedef char* ose_bundle;
#define ose_makeBundle(charptr) charptr
#define ose_makeConstBundle(charptr) charptr
#define ose_getBundlePtr(bundle) (bundle)
#define ose_constbundle const char * const
#endif

typedef char ose_bool;
typedef void (*ose_fn)(ose_bundle);
#define OSE_INTPTR2 128

#define OSE_BUNDLE_ID "#bundle\0"
#define OSE_BUNDLE_ID_LEN 8
#define OSE_TIMETAG_NULL "\0\0\0\0\0\0\0\0"
#define OSE_TIMETAG_LEN 8
#define OSE_BUNDLE_HEADER OSE_BUNDLE_ID OSE_TIMETAG_NULL
#define OSE_BUNDLE_HEADER_LEN (OSE_BUNDLE_ID_LEN + OSE_TIMETAG_LEN)

#define OSE_EMPTY_TYPETAG_STRING ",\0\0\0"
#define OSE_EMPTY_TYPETAG_STRING_LEN 4

// OSC 1.0 types
#define OSETT_ID ','
#define OSETT_INT32 'i'
#define OSETT_FLOAT 'f'
#define OSETT_STRING 's'
#define OSETT_BLOB 'b'

// extended types
#ifdef OSE_PROVIDE_TYPE_SYMBOL
#define OSETT_SYMBOL 'S'
#endif
#ifdef OSE_PROVIDE_TYPE_DOUBLE
#define OSETT_DOUBLE 'd'
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
#define OSETT_INT8 'c'
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
#define OSETT_UINT8 'C'
#endif
#ifdef OSE_PROVIDE_TYPE_INT16
#define OSETT_INT16 'u'
#endif
#ifdef OSE_PROVIDE_TYPE_UINT16
#define OSETT_UINT16 'U'
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
#define OSETT_UINT32 'k'
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
#define OSETT_INT64 'h'
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
#define OSETT_UINT64 'H'
#endif
#ifdef OSE_PROVIDE_TYPE_TIMETAG
struct ose_timetag
{
	uint32_t sec;
	uint32_t fsec;
};
#define OSETT_TIMETAG 't'
#endif
#ifdef OSE_PROVIDE_TYPE_TRUE
#define OSETT_TRUE 'T'
#endif
#ifdef OSE_PROVIDE_TYPE_FALSE
#define OSETT_FALSE 'F'
#endif
#ifdef OSE_PROVIDE_TYPE_NULL
#define OSETT_NULL 'N'
#endif
#ifdef OSE_PROVIDE_TYPE_INFINITUM
#define OSETT_INFINITUM 'I'
#endif
#ifdef OSE_PROVIDE_TYPE_CFUNCTION
#define OSETT_CFUNCTION 'l'
#endif

// non-osc data types
// these are never written into a bundle
// (unless they are defined above)
#ifndef OSETT_TRUE
#define OSETT_TRUE 'T'
#endif
#ifndef OSETT_FALSE
#define OSETT_FALSE 'F'
#endif
#define OSETT_NOTYPETAG 0
#define OSETT_BUNDLE '|'
#define OSETT_MESSAGE '-'
#ifndef OSETT_CFUNCTION
#define OSETT_CFUNCTION 'l'
#endif

#ifndef OSE_USER_ADDRESS_ANONVAL
#define OSE_ADDRESS_ANONVAL "\0\0\0\0"
#define OSE_ADDRESS_ANONVAL_LEN 0
#define OSE_ADDRESS_ANONVAL_SIZE 4
#endif

#define OSE_ADDRESS_MIN_LEN 0
#define OSE_ADDRESS_MIN_PLEN 4

#ifdef OSE_DEBUG
#include <stdio.h>
void p(ose_bundle bundle, const char * const str);
void e(ose_bundle bundle, const char * const str);
#endif

#ifdef __cplusplus
}
#endif

#endif
