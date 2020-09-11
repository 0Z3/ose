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
 * @file ose_context.h
 * @brief This file contains utility functions for constructing and 
 * manipulating the structure of an OSE bundle.
 * 
 * An OSE bundle is an OSC bundle with a particular structure designed
 * to support the permissive, but safe manipulation of its contents. 
 * 
 * OSE provides a powerful collection of functions for manipulating the
 * contents of an OSC bundle. At the same time, it imposes few checks to 
 * ensure that operations on the bundle haven't left it in a corrupt state
 * that an OSC application wouldn't be able to handle. This lack of checks
 * is by design, making it possible to use OSE to extend OSC to suit 
 * application specific needs.
 * 
 * An OSE bundle is constructed by passing a block of memory to the 
 * function #ose_newBundleFromCBytes,
 * @code{.c}
 * int32_t n = 66536;
 * char b[n];
 * ose_bundle bundle = ose_newBundleFromCBytes(n, b);
 * @endcode
 * The bundle that is returned is actually contained in a blob of a fixed size,
 * which itself is one argument of a message, inside of an inclosing bundle:
 * @f$B(E^{msg}(A_{/cx} \vert I_1^{i}, I_2^{i}, I_3^{i}, 
 * I_4^{b}, I_5^{b}))@f$,
 * where @f$B@f$ is the outermost bundle, the world, @f$E^{msg}@f$ is a 
 * bundle element of type message, with address @f$A_{/cx} = /cx@f$ 
 * and containing three ints and two blobs. The three ints, 
 * @f$I_1^{i}, I_2^{i}, I_3^{i}@f$, are used by
 * the system to store offsets, while @f$I_4^{blob}@f$ contains your bundle,
 * and @f$I_5^{blob}@f$ is simply padding to ensure that the message 
 * stays the same size as your bundle changes.
 * 
 * Note that the contents of the top level bundle that contains the 
 * @f$/cx@f$ message should never be manipulated directly.
 *
 * The functions in this file may be used to similarly partition your bundle
 * into sections of a static, fixed size if necessary. For example, the 
 * OSE VM consists of six bundles: input, stack, environment, control,
 * dump, and output. 
 * @f[I_4^{blob} = B\left(
 * \begin{array}{l}
 * E^{msg}_1(A_{/i} \vert I_1^{i}, I_2^{i}, I_3^{i}, I_4^{b}, I_5^{b}),\\
 * E^{msg}_2(A_{/s} \vert I_1^{i}, I_2^{i}, I_3^{i}, I_4^{b}, I_5^{b}),\\
 * E^{msg}_3(A_{/e} \vert I_1^{i}, I_2^{i}, I_3^{i}, I_4^{b}, I_5^{b}),\\
 * E^{msg}_4(A_{/c} \vert I_1^{i}, I_2^{i}, I_3^{i}, I_4^{b}, I_5^{b}),\\
 * E^{msg}_5(A_{/d} \vert I_1^{i}, I_2^{i}, I_3^{i}, I_4^{b}, I_5^{b}),\\
 * E^{msg}_6(A_{/o} \vert I_1^{i}, I_2^{i}, I_3^{i}, I_4^{b}, I_5^{b}),
 * \end{array}
 * \right)
 * @f]
 * 
 * The code below referst to these types of messages as "context messages".
 */

#ifndef OSE_CONTEXT_H
#define OSE_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "ose_conf.h"
#include "ose.h"

/**
 * @brief Bytes passed to #ose_newBundleFromCBytes will be aligned to a boundary
 * that is a multiple of this number of bytes.
 */
#define OSE_CONTEXT_ALIGNMENT 4




/**
 * @brief The number of bytes between the beginning of a context message
 * 	  and the bundle (blob).
 */
#define OSE_CONTEXT_BUNDLE_OFFSET				\
	4			/* size */			\
	+ 4			/* padded address len */	\
	+ 8			/* padded typetag str */	\
	+ 4			/* int */			\
	+ 4			/* int */			\
	+ 4			/* int */			\
	+ 4			/* blob size */




/** 
 * @brief The overhead in bytes of a context message.
 */
#define OSE_CONTEXT_MESSAGE_OVERHEAD			\
	OSE_CONTEXT_BUNDLE_OFFSET			\
	+ OSE_BUNDLE_HEADER_LEN	/* bundle header */	\
	+ 4			/* blob size */




/**
 * @brief The size of the status message.
 */
#define OSE_CONTEXT_STATUS_MESSAGE_SIZE 16




/**
 * @brief Maximum overhead used by the system.
 */
#define OSE_CONTEXT_MAX_OVERHEAD					\
	OSE_CONTEXT_ALIGNMENT						\
	* 4								\
	+ OSE_BUNDLE_HEADER_LEN						\
	+ OSE_CONTEXT_MESSAGE_OVERHEAD + OSE_CONTEXT_STATUS_MESSAGE_SIZE \
	+ OSE_CONTEXT_MESSAGE_OVERHEAD



/**
 * @brief Initializes a newly created bundle with a context message.
 *
 * @param bundle The bundle.
 * @param size The number of bytes available for this context message
 * @param address The address of this context message.
 * @returns The size available for the bundle minus 
 * 	    the ovehead of the message.
 */
int32_t ose_init(ose_bundle bundle,
		 int32_t size,
		 const char * const address);





/**
 * @brief Pushes a new context message onto the current bundle.
 *
 * @param bundle The bundle.
 * @param size The number of bytes available for this context message
 * @param address The address of this context message.
 * @returns The size available for the bundle minus 
 * 	    the ovehead of the message.
 */
int32_t ose_pushContextMessage(ose_bundle bundle,
			       int32_t size,
			       const char * const address);





/**
 * @brief Returns the number of unused bytes in the bundle.
 * 
 * @param bundle The bundle.
 * @returns The number of unused bytes in the bundle.
 */
int32_t ose_spaceAvailable(ose_constbundle bundle);





/**
 * @brief Enter a new bundle context.
 *
 * @param bundle The current bundle.
 * @param address The address of the context message containing
 *        the bundle to enter.
 * @returns The requested bundle.
 */
ose_bundle ose_enter(ose_bundle bundle, const char * const address);





/**
 * @brief Exit a bundle context and move one level up.
 *
 * @param bundle The current bundle.
 * @returns The bundle one level up.
 */
ose_bundle ose_exit(ose_bundle bundle);





/**
 * @brief Add an amount to the size of a bundle. This function takes care
 * of adjusting the size of the blob of free space, and must be called to 
 * ensure that all sizes are updated properly, when using the context 
 * machinery in this file, as well as the stack operations in #ose_stackops.h.
 *
 * @param bundle The bundle to change the size of.
 * @param amt The amount to add to (or subtract from) the bundle.
 */
void ose_addToSize(ose_bundle bundle, int32_t amt);
void ose_incSize(ose_bundle bundle, int32_t amt);
void ose_decSize(ose_bundle bundle, int32_t amt);





/**
 * @brief Copy the topmost bundle element to a destination at the same level.
 *
 * @param src The source bundle.
 * @param dest The destination bundle.
 */
void ose_copyBundleElemToDest(ose_bundle src, ose_bundle dest);





/**
 * @brief Copy the topmost bundle element to a destination at the same level.
 *
 * @param src The source bundle.
 * @param dest_addr The destination address.
 */
void ose_copyBundleElemToDestAddr(ose_bundle src, const char * const dest_addr);





/**
 * @brief Move the topmost bundle element to a destination at the same level.
 *
 * @param src The source bundle.
 * @param dest The destination bundle.
 */
void ose_moveBundleElemToDest(ose_bundle src, ose_bundle dest);





/**
 * @brief Move the topmost bundle element to a destination at the same level.
 *
 * @param src The source bundle.
 * @param dest_addr The destination address.
 */
void ose_moveBundleElemToDestAddr(ose_bundle src, const char * const dest_addr);





/**
 * @brief Move the topmost bundle element to a destination at the same level,
 * 	  replacing the first element with the same address if it is a message.
 *        If no message in the destination bundle exists with the same address,
 * 	  this function behaves like #ose_copyBundleElemToDestAddr.
 *
 * @param src The source bundle.
 * @param dest The destination bundle.
 */
void ose_replaceBundleElemInDest(ose_bundle src, ose_bundle dest);





/**
 * @brief Move the topmost bundle element to a destination at the same level,
 * 	  replacing the first element with the same address if it is a message.
 *        If no message in the destination bundle exists with the same address,
 * 	  this function behaves like #ose_copyBundleElemToDestAddr.
 *
 * @param src The source bundle.
 * @param dest_addr The destination address.
 */
void ose_replaceBundleElemInDestAddr(ose_bundle src,
				     const char * const dest_addr);






void ose_copyBundle(ose_constbundle src, ose_bundle dest);
void ose_appendBundle(ose_bundle src, ose_bundle dest);
void ose_replaceBundle(ose_bundle src, ose_bundle dest);
void ose_copyElem(ose_constbundle src, ose_bundle dest);
void ose_moveElem(ose_bundle src, ose_bundle dest);





/**
 * @brief Initialize a new bundle from an array of bytes.
 *
 * @param nbytes The number of bytes in the array.
 * @param bytes An array of bytes containing nbytes, preallocated.
 * @returns An initialized ose_bundle.
 * 
 * There is no need to dispose of the returned object in a special way.
 * You may simply free the byte array when you are finished, in accordance
 * with the way it was allocated.
 */
ose_bundle ose_newBundleFromCBytes(int32_t nbytes, char *bytes);

#ifdef __cplusplus
}
#endif

#endif
