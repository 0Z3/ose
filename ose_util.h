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
 * @file ose_util.h
 * @brief Basic utilities for reading and writing parts of an OSC bundle,
 * as well as predicates for type checking.
 */

#ifndef OSE_UTIL_H
#define OSE_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include <stdarg.h>
#include "ose_conf.h"
#include "ose.h"

/**
 * @brief Returns n padded to a multiple of 4. n = 0 => 4.
 * 
 * When n is 0, this function returns 4.
 *
 * @param n Number of bytes.
 * @returns The closest multiple of 4 greater than n.
 */
int32_t ose_pnbytes(int32_t n);
int32_t ose_pstrlen(const char * const s);

ose_bool ose_isAddressChar(char c);
ose_bool ose_isKnownTypetag(char typetag);
ose_bool ose_isStringType(char typetag);
ose_bool ose_isIntegerType(char typetag);
ose_bool ose_isFloatType(char typetag);
ose_bool ose_isNumericType(char typetag);
ose_bool ose_isUnitType(char typetag);
ose_bool ose_isBoolType(char typetag);





/**
 * @brief Determines whether the data pointed to contains a bundle or not.
 *
 * @param bundle The bundle.
 * @returns #OSETT_TRUE if the first 8 bytes of that the bundle points to
 * are \#bundle, #OSETT_FALSE otherwise (including the case where bundle is a 
 * NULL pointer).
 */
ose_bool ose_isBundle(ose_constbundle bundle);





/**
 * @brief Tests whether a bundle contains any bundle elements.
 *
 * @param bundle The bundle.
 * @retuns #OSETT_TRUE if bundle is empty, #OSETT_FALSE otherwise.
 *
 * @note An empty bundle contains no messages, but still has a header. If 
 * bundle points to NULL, or has a size of 0, this function's assertions will
 * fail, since bundle does not point to a bundle.
 */
ose_bool ose_bundleIsEmpty(ose_constbundle bundle);
int32_t ose_getBundleElemCount(ose_constbundle bundle);





/**
 * @brief Returns the number of items in a bundle element.
 *
 * If the bundle element is a message, this function returns the number
 * of type tags (not including the initial comma), and if it is a bundle,
 * it returns the result of #ose_getBundleElemCount applied to that bundle.
 *
 * @param bundle The bundle.
 * @param offset The offset of the element.
 * @returns The number of items in the element.
 */
int32_t ose_getBundleElemElemCount(ose_constbundle bundle, int32_t offset);
ose_bool ose_bundleHasAtLeastNElems(ose_constbundle bundle, int32_t n);
char ose_getBundleElemType(ose_constbundle bundle, int32_t offset);

char ose_readByte(ose_constbundle bundle, int32_t offset);
int32_t ose_writeByte(ose_bundle bundle, int32_t offset, char i);
int32_t ose_readInt32(ose_constbundle bundle, int32_t offset);
int32_t ose_writeInt32(ose_bundle bundle, int32_t offset, int32_t i);
#define ose_addToInt32(b, offset, amt) \
	ose_writeInt32((b), (offset), ose_readInt32((b), (offset)) + (amt))
float ose_readFloat(ose_constbundle bundle, int32_t offset);
int32_t ose_writeFloat(ose_bundle bundle, int32_t offset, float f);
char *ose_readString(ose_bundle bundle, int32_t offset);
int32_t ose_getStringLen(ose_constbundle bundle, int32_t offset);
int32_t ose_getPaddedStringLen(ose_constbundle bundle, int32_t offset);
int32_t ose_writeStringWithLen(ose_bundle bundle,
			       int32_t offset,
			       const char * const s,
			       int32_t slen,
			       int32_t slen_padded);
int32_t ose_writeString(ose_bundle bundle, int32_t offset, const char * const s);
char *ose_readBlob(ose_bundle bundle, int32_t offset);
int32_t ose_readBlobSize(ose_constbundle bundle, int32_t offset);





/**
 * @brief Get the number of bytes required to pad a blob size to a multiple
 * of 4 bytes.
 *
 * @param n Number of bytes.
 * @returns The number of additional bytes required for n to be a multiple 
 * of 4.
 */
int32_t ose_getBlobPaddingForNBytes(int32_t n);
int32_t ose_getPaddedBlobSize(ose_constbundle bundle, int32_t offset);
char *ose_readBlobPayload(ose_bundle bundle, int32_t offset);
int32_t ose_writeBlob(ose_bundle bundle,
		      int32_t offset,
		      int32_t blobsize,
		      const char * const blob);
int32_t ose_appendByteToBlob(ose_bundle bundle, int32_t offset, char i);
#ifdef OSE_PROVIDE_TYPE_SYMBOL
#define ose_readSymbol(b, o) ose_readString((b), (o))
#define ose_writeSymbol(b, o, s) ose_writeString((b), (o), (s))
#endif
#ifdef OSE_PROVIDE_TYPE_DOUBLE
double ose_readDouble(ose_constbundle bundle, int32_t offset);
int32_t ose_writeDouble(ose_bundle bundle, int32_t offset, double f);
#endif
#ifdef OSE_PROVIDE_TYPE_INT8
#define ose_readInt8(b, o) ose_readInt32((b), (o))
#define ose_writeInt8(b, o, i) ose_writeInt32((b), (o), (i))
#endif
#ifdef OSE_PROVIDE_TYPE_UINT8
#define ose_readUInt8(b, o) ose_readInt32((b), (o))
#define ose_writeUInt8(b, o, i) ose_writeInt32((b), (o), (i))
#endif
#ifdef OSE_PROVIDE_TYPE_UINT32
uint32_t ose_readUInt32(ose_constbundle bundle, int32_t offset);
int32_t ose_writeUInt32(ose_bundle bundle, int32_t offset, uint32_t i);
#endif
#ifdef OSE_PROVIDE_TYPE_INT64
int64_t ose_readInt64(ose_constbundle bundle, int32_t offset);
int32_t ose_writeInt64(ose_bundle bundle, int32_t offset, int64_t i);
#endif
#ifdef OSE_PROVIDE_TYPE_UINT64
uint64_t ose_readUInt64(ose_constbundle bundle, int32_t offset);
int32_t ose_writeUInt64(ose_bundle bundle, int32_t offset, uint64_t i);
#endif
#ifdef OSE_PROVIDE_TYPE_TIMETAG
struct ose_timetag ose_readTimetag(ose_constbundle bundle, int32_t offset);
int32_t ose_writeTimetag(ose_bundle bundle,
			 int32_t offset,
			 uint32_t sec,
			 uint32_t fsec);
#endif
ose_fn ose_readCFn(ose_constbundle bundle, int32_t offset);
int32_t ose_writeCFn(ose_bundle bundle,
		     const int32_t offset,
		     const ose_fn fn);


/**
 * @brief Get the offset in bytes of the last bundle element (i.e.
 * the one furthest away from the bundle header).
 *
 * This function returns the offset in bytes of the last element of a bundle.
 * The offset points to the beginning of the bundle element, i.e. the size
 * field. If the bundle is empty, this function returns the size of the 
 * bundle header.
 *
 * @param bundle The bundle.
 * @returns The offset of the last element in bytes.
 */
int32_t ose_getLastBundleElemOffset(ose_constbundle bundle);
int32_t ose_getBundleElemAddressOffset(ose_constbundle bundle,
				       int32_t elemoffset);
int32_t ose_getBundleElemTTOffset(ose_constbundle bundle, int32_t elemoffset);
int32_t ose_getBundleElemPayloadOffset(ose_constbundle bundle,
				       int32_t elemoffset);

// not implemented or tested yet:
int32_t ose_getFirstOffsetForMatch(ose_constbundle bundle,
				   const char * const addr);
int32_t ose_getFirstOffsetForPMatch(ose_constbundle bundle,
				    const char * const addr);

/**
 * @brief Calculate the size of a typed OSC datum.
 *
 * Returns the full size that a given datum occupies, including
 * any padding, size field, etc. 
 *
 * String sizes include any NULL padding (a string of 4 bytes will return
 * a value of 8), and blobs include their size field and any NULL padding
 * (a blob of size 0 will return a value of 4, and a blob of size 1 will 
 * return a value of 8.
 *
 * @param typetag The type tag of the data.
 * @param ptr A pointer to the location where the data is. This parameter
 * is necessary for variable width types such as strings and blobs.
 */
int32_t ose_getTypedDatumSize(char typetag, const char * const ptr);





/**
 * @brief Calculate the size of a typed OSC datum.
 *
 * Returns the full size that a given datum occupies, including
 * any padding, size field, etc. 
 *
 * String sizes include any NULL padding (a string of 4 bytes will return
 * a value of 8), and blobs include their size field and any NULL padding
 * (a blob of size 0 will return a value of 4, and a blob of size 1 will 
 * return a value of 8.
 *
 * @param bundle The bundle.
 * @param typetag The type tag of the data.
 * @param payload_offset The offset in bytes from the beginning of the 
 * bundle to where the datum is.
 */
int32_t ose_getPayloadItemSize(ose_constbundle bundle,
			       char typetag,
			       int32_t payload_offset);





/**
 * @brief Calculate the length of a typed OSC datum.
 *
 * Returns the length of an OSC datum, not including padding or NULL
 * bytes, or size fields for types that require them.
 *
 * @param typetag The type tag of the data.
 * @param ptr A pointer to the location where the data is. This parameter
 * is necessary for variable width types such as strings and blobs.
 */
int32_t ose_getTypedDatumLength(char typetag, const char * const ptr);





/**
 * @brief Calculate the length of a typed OSC datum.
 *
 * Returns the length of an OSC datum, not including padding or NULL
 * bytes, or size fields for types that require them.
 *
 * String sizes include any NULL padding (a string of 4 bytes will return
 * a value of 8), and blobs include their size field and any NULL padding
 * (a blob of size 0 will return a value of 4, and a blob of size 1 will 
 * return a value of 8.
 *
 * @param bundle The bundle.
 * @param typetag The type tag of the data.
 * @param payload_offset The offset in bytes from the beginning of the 
 * bundle to where the datum is.
 */
int32_t ose_getPayloadItemLength(ose_constbundle bundle,
				 char typetag,
				 int32_t payload_offset);





/**
 * @brief Get the offset of the nth payload item of a bundle element.
 *
 * This function gives the offset in bytes of the nth item of the 
 * bundle element at offset o. In addition, it also provides the 
 * offset of the nth typetag, the total number of typetags, and 
 * the offsets of the first typetag and first payload item. 
 *
 * If the element is not a message, the results are undefined.
 *
 * This function counts from the end furthest from the beginning of the
 * element, with n=1 being the last item of the element. 
 *
 * The number of typetags includes the leading comma of the typetag string.
 *
 * When n=1, lto and lpo contain the offsets of the typetag and payload
 * items furthest from the beginning of the bundle element.
 *
 * When n is equal to the number of typetags including the leading comma,
 * to == lto and po == lpo.
 *
 * @param bundle The bundle.
 * @param n The index of the item to get.
 * @param o The offset of the bundle element relative to be start of the bundle.
 * @param _to The offset of the first typetag in the bundle element.
 * @param _ntt The number of typetags in the element.
 * @param _lto The offset of the nth typetag.
 * @param _po The offset of the first payload item.
 * @param _lpo The offset of the nth payload item.
 */
void ose_getNthPayloadItem(ose_constbundle bundle,
			   int32_t n,
			   int32_t o,
			   int32_t *_to,
			   int32_t *_ntt,
			   int32_t *_lto,
			   int32_t *_po,
			   int32_t *_lpo);





/**
 * @brief Push a message with arguments onto the stack.
 * @f$\textrm{PUSHMESSAGE}(S, A, t_1, v^\tau_1, \ldots, t_N, v^\tau_N) 
 * \Rightarrow S'({E_1^{msg}}'(I^\tau_1, \ldots, I^\tau_N))@f$
 *
 * This function performs no checks to ensure that there is enough 
 * available memory
 * 
 * @param bundle The bundle.
 * @param offset The offset into the bundle where the message will be written.
 * @param address A C string that will be copied to the address of the message.
 * @param addresslen The length in bytes of the address, 
 * 	  not ncluding the NULL byte.
 * @param n The number of OSC arguments for the message.
 * @param ... The argument list, which consists of a type tag followed 
 *        by 0 or more values, depending on the type (see table below).
 *
 * <table>
 * <tr><th>Type Tag</th><th>Number of Args</th><th>Arg Types</th><th>Arg Desc</th></tr>
 * <tr><td>OSETT_INT32</td><td>1</td><td><pre>int32_t</pre></td><td></td></tr>
 * <tr><td>OSETT_FLOAT</td><td>1</td><td><pre>double</pre></td><td></td></tr>
 * <tr><td>OSETT_STRING</td><td>1</td><td><pre>char*</pre></td><td></td></tr>
 * <tr><td>OSETT_BLOB</td><td>2</td><td><pre>int32_t, char*</pre></td><td>blob size, blob ptr</td></tr>
 * <tr><td>OSETT_SYMBOL</td><td>1</td><td><pre>char*</pre></td><td></td></tr>
 * <tr><td>OSETT_DOUBLE</td><td>1</td><td><pre>double</pre></td><td></td></tr>
 * <tr><td>OSETT_INT8</td><td>1</td><td><pre>int</pre></td><td></td></tr>
 * <tr><td>OSETT_UINT8</td><td>1</td><td><pre>int</pre></td><td></td></tr>
 * <tr><td>OSETT_UINT32</td><td>1</td><td><pre>uint32_t</pre></td><td></td></tr>
 * <tr><td>OSETT_INT64</td><td>1</td><td><pre>int64_t</pre></td><td></td></tr>
 * <tr><td>OSETT_UINT64</td><td>1</td><td><pre>uint64_t</pre></td><td></td></tr>
 * <tr><td>OSETT_TIMETAG</td><td>2</td><td><pre>int32_t, int32_t</pre></td><td>seconds, fractions of a second</td></tr>
 * <tr><td>OSETT_TRUE</td><td>0</td><td><pre></pre></td><td></td></tr>
 * <tr><td>OSETT_FALSE</td><td>0</td><td><pre></pre></td><td></td></tr>
 * <tr><td>OSETT_NULL</td><td>0</td><td><pre></pre></td><td></td></tr>
 * <tr><td>OSETT_INFINITUM</td><td>0</td><td><pre></pre></td><td></td></tr>
 * <tr><td>custom type tag</td><td>2</td><td><pre>int32_t, char*</pre></td><td>size, ptr (same as blob)</td></tr>
 * </table>
 *
 * @note This function checks to ensure there is enough available memory only when
 * assertions are enabled (i.e. when compiled with OSE_CONF_DEBUG defined.
 * If running out of memory is a possibility, the caller should ensure that 
 * there is enough free memory available using #ose_spaceAvailable.
 *
 * Example:
 * @code{.c}
 * ose_pushMessage(bundle, "/foo", 
 * 		   4,
 * 		   4,
 *                 OSETT_INT32, 33,
 *                 OSETT_FLOAT, 33.3,
 *                 OSETT_STRING, THIRTY-THREE,
 *                 OSETT_BLOB, 3, (char []){3, 3, 3});
 * @endcode
 */
int32_t ose_writeMessage(ose_bundle bundle,
			 int32_t offset,
			 const char * const address,
			 int32_t addresslen,
			 int32_t n, ...);





/**
 * @brief see #ose_writeMessage.
 */
int32_t ose_vwriteMessage(ose_bundle bundle,
			  int32_t offset,
			  const char * const address,
			  int32_t addresslen,
			  int32_t n,
			  va_list ap);

#ifdef __cplusplus
}
#endif

#endif
