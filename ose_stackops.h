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
 * @brief This file provides functions that operate on an OSE bundle as a stack.
 * 
 * We consider an OSE bundle @f$B@f$ to be a stack @f$S@f$ of elements 
 * @f$E_i^\tau@f$,
 * where @f$t@f$, if present, denotes their type, either @f$msg@f$ or
 * @f$bndl@f$. An element contains items @f$I_i^\tau@f$, where again, @f$t@f$
 * denotes their type, if known. Although extended OSC types such as 
 * double precision floating point numbers may be enabled, here we consider
 * only the basic required OSC 1.0 types: 32-bit signed integers, 32-bit
 * IEEE 754 floating point numbers, null-terminated strings, and binary
 * blobs, represented by types @f$i, f, s, b@f$ respectively. The types of
 * elements and items will be omitted unless required by a particular function.
 * 
 * The stack @f$S@f$ that these functions operate on is an OSE bundle @f$B@f$,
 * and the items of the stack are the elements @f$E_i@f$. Since the elements
 * of the stack are themselves aggregate data structures, this code provides
 * a number of functions that operate on the structure of the topmost element,
 * often resulting in a change to its structure and the pushing of an element
 * onto the top of the stack. For example, the @f$\textrm{POP}@f$ 
 * function conceptually
 * "pops" the last item of the topmost element, and pushes it onto the stack.
 * If the topmost element of the stack is a bundle @f$E_1^{bndl}@f$, 
 * its topmost element is removed and becomes the topmost element of 
 * @f$S@f$, i.e. 
 * @f$\textrm{POP}(S(E_1^{bndl}(E_{1_1},\ldots E_{1_N}),\ldots, E_N))=
 * S'(E_{1_1}, {E_1^{bndl}},\ldots, E_N)@f$. However, if the topmost element
 * of @f$S@f$ is a message, then the topmost item of that message will be
 * pushed onto a new message, which will be pushed onto @f$S@f$, i.e.
 * @f$\textrm{POP}(S(E_1^{msg}(I_{1_1},\ldots, I_{1_N}),\ldots, E_N))=
 * S(E_1^{msg}(I_{1_1}), {E_1^{msg}}',\ldots,E_N)@f$. In both of these cases,
 * the elements marked with a tick (@f$'@f$) have had their topmost item
 * removed and may be empty, i.e. their payloads may have no items.
 *
 * Practically, @f$B@f$ and @f$S@f$ are equivalent, however, we make a 
 * distinction due to the conventions associated with stacks and OSC bundles.
 * Typically, the "first" element of an OSC bundle is the one closest
 * to the bundle header, the start of the byte array. When processed by 
 * an OSC server such as the OpenSoundControl object in Max/MSP, a canonical
 * OSC implementation, the expectation is that the "first" message will
 * be output first, and all messages will proceed in the order that they
 * are found in the bundle. The "topmost" element of @f$S@f$, however,
 * is the "last" element of @f$B@f$, the furthest from the beginning
 * of the byte array, while the "first" element of @f$B@f$ is the 
 * "bottom" of @f$S@f$. 
 * 
 * @f$B@f$ and @f$S@f$ are simply two different 
 * conventions for labeling and referring to the same data. When the notation
 * @f$B(E_1(I_{1_1},\ldots, I_{1_N}),\ldots, E_N)@f$ is used, 
 * @f$E_1@f$ refers to the "first" element
 * of an OSC bundle, i.e. the one closest to the beginning of the byte array,
 * and @f$I_{1_1}@f$ refers to the "first" item of that element, either
 * the first bundle element if @f$E_1@f$ is a bundle, or the first argument
 * if @f$E_1@f$ is a message. When we write 
 * @f$S(E_1(I_{1_1},\ldots, I_{1_N}),\ldots, E_N)@f$, @f$E_1@f$ refers
 * to the topmost element of the stack @f$S@f$, i.e. 
 * @f$B(E_1)\equiv S(E_N)@f$, and @f$B(E_1)\equiv S(E_N)@f$. Similarly,
 * @f[
 * \begin{array}{ll}
 * B(E_1(I_{1_1})) & \equiv S(E_N(I_{N_N})),\\
 * B(E_1(I_{1_N})) & \equiv S(E_N(I_{N_1})),\\
 * B(E_N(I_{1_1})) & \equiv S(E_1(I_{N_N})),\\
 * B(E_N(I_{N_N})) & \equiv S(E_1(I_{1_1}))
 * \end{array}
 * @f].
 *
 * In the case of a message element, when neccessary to notate the address,
 * it will appear as the first element, separated from the arguments by a
 * pipe: @f$E^{msg}(A_{/name}|I_1, ...)@f$.
 */

#ifndef OSE_STACKOPS_H
#define OSE_STACKOPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ose_conf.h"
#include "ose.h"
#include "ose_util.h"

/**
 * @name C API
 * @brief The C API is an interface for translating C values to 
 * OSE / OSC values and back.
 * 
 * The interface consists of three families of functions: 
 * @f$\textrm{PUSH}@f$, @f$\textrm{POP}@f$, and @f$\textrm{PEEK}@f$, 
 * all of which have variants specific to each OSE / OSC type.
 *
 * The @f$\textrm{PUSH}@f$ family of functions each take a bundle and a C type
 * as arguments and push a new message onto the stack that contains
 * a single item, which is the OSC equivalent of the C value:
 * @f$\textrm{PUSH}^\tau(S, v^\tau) \Rightarrow, 
 * S'(E^{msg}(I^\tau_1), \ldots )@f$.
 *
 * The @f$\textrm{POP}@f$ family of functions each take a bundle 
 * as an argument and return the C equivalent of the topmost
 * item of the topmost element of the stack, removing that item:
 * @f$\textrm{POP}^\tau(S(E_1(I_{1_1}^\tau))) \Rightarrow, 
 * (S'(E_1'), v_{1_1}^\tau)@f$.
 *
 * The @f$\textrm{PEEK}@f$ family of functions each operate exactly
 * as those of the @f$\textrm{POP}@f$ family, except that they leave
 * the topmost item of the topmost element on the stack:
 * @f$\textrm{PEEK}^\tau(S(E_1(I_{1_1}^\tau))) \Rightarrow, 
 * (S, v_{1_1}^\tau)@f$.
 */
/**@{*/

/**************************************************
 * C API
 **************************************************/

/**
 * @brief Push a 32-bit signed int onto the stack.
 * @f$\textrm{PUSH}^i(S, v^i) \Rightarrow S'({E_1^{msg}}'(I^i))@f$
 */
void ose_pushInt32(ose_bundle bundle, int32_t i);




/**
 * @brief Push a 32-bit float onto the stack.
 * @f$\textrm{PUSH}^f(S, v^f) \Rightarrow S'({E_1^{msg}}'(I^f))@f$
 */
void ose_pushFloat(ose_bundle bundle, float f);




/**
 * @brief Push a string onto the stack.
 * @f$\textrm{PUSH}^s(S, v^s) \Rightarrow S'({E_1^{msg}}'(I^s))@f$
 */
void ose_pushString(ose_bundle bundle, const char * const s);




/**
 * @brief Push a blob onto the stack.
 * @f$\textrm{PUSH}^i(S, v^i, v^{char*}) \Rightarrow S'({E_1^{msg}}'(I^b))@f$
 */
void ose_pushBlob(ose_bundle bundle, int32_t blobsize, const char * const blob);




#ifdef OSE_PROVIDE_TYPE_SYMBOL
/**
 * @brief Push a symbol onto the stack.
 * @f$\textrm{PUSH}^S(S, v^S) \Rightarrow S'({E_1^{msg}}'(I^S))@f$
 */
void ose_pushSymbol(ose_bundle bundle, const char * const s);
#endif




#ifdef OSE_PROVIDE_TYPE_DOUBLE
/**
 * @brief Push a double-precision float onto the stack.
 * @f$\textrm{PUSH}^d(S, v^d) \Rightarrow S'({E_1^{msg}}'(I^d))@f$
 */
void ose_pushDouble(ose_bundle bundle, double f);
#endif




#ifdef OSE_PROVIDE_TYPE_INT8
/**
 * @brief Push an 8-bit signed int onto the stack.
 * @f$\textrm{PUSH}^c(S, v^c) \Rightarrow S'({E_1^{msg}}'(I^c))@f$
 */
void ose_pushInt8(ose_bundle bundle, int8_t i);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT8
/**
 * @brief Push an 8-bit unsigned int onto the stack.
 * @f$\textrm{PUSH}^C(S, v^C) \Rightarrow S'({E_1^{msg}}'(I^C))@f$
 */
void ose_pushUInt8(ose_bundle bundle, uint8_t i);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT32
/**
 * @brief Push a 32-bit unsigned int onto the stack.
 * @f$\textrm{PUSH}^k(S, v^k) \Rightarrow S'({E_1^{msg}}'(I^k))@f$
 */
void ose_pushUInt32(ose_bundle bundle, uint32_t i);
#endif




#ifdef OSE_PROVIDE_TYPE_INT64
/**
 * @brief Push a 64-bit signed int onto the stack.
 * @f$\textrm{PUSH}^h(S, v^h) \Rightarrow S'({E_1^{msg}}'(I^h))@f$
 */
void ose_pushInt64(ose_bundle bundle, int64_t i);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT64
/**
 * @brief Push a 64-bit unsigned int onto the stack.
 * @f$\textrm{PUSH}^H(S, v^H) \Rightarrow S'({E_1^{msg}}'(I^H))@f$
 */
void ose_pushUInt64(ose_bundle bundle, uint64_t i);
#endif




#ifdef OSE_PROVIDE_TYPE_TIMETAG
/**
 * @brief Push an OSCtimetag onto the stack.
 * @f$\textrm{PUSH}^t(S, v^i, v^i) \Rightarrow S'({E_1^{msg}}'(I^t))@f$
 */
void ose_pushTimetag(ose_bundle bundle, uint32_t sec, uint32_t fsec);
#endif




#ifdef OSE_PROVIDE_TYPE_TRUE
/**
 * @brief Push TRUE onto the stack.
 * @f$\textrm{PUSH}^T(S) \Rightarrow S'({E_1^{msg}}'(I^T))@f$
 */
void ose_pushTrue(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_FALSE
/**
 * @brief Push FALSE onto the stack.
 * @f$\textrm{PUSH}^F(S) \Rightarrow S'({E_1^{msg}}'(I^F))@f$
 */
void ose_pushFalse(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_NULL
/**
 * @brief Push NULL onto the stack.
 * @f$\textrm{PUSH}^N(S) \Rightarrow S'({E_1^{msg}}'(I^N))@f$
 */
void ose_pushNull(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_INFINITUM
/**
 * @brief Push an INFINITUM onto the stack.
 * @f$\textrm{PUSH}^I(S) \Rightarrow S'({E_1^{msg}}'(I^I))@f$
 */
void ose_pushInfinitum(ose_bundle bundle);
#endif




void ose_pushCFn(ose_bundle bundle, const ose_fn fn);





/**
 * @brief see #ose_writeMessage.
 */
void ose_pushMessage(ose_bundle bundle,
		     const char * const address,
		     int32_t addresslen,
		     int32_t n,
		     ...);




/**
 * @brief Get the address of the topmost element.
 * @f$\textrm{PEEKADDRESS}(S(A) \Rightarrow A@f$
 *
 * @returns A pointer to the address. 
 */
char *ose_peekAddress(const ose_bundle bundle);




/**
 * @brief Get the type of the topmost item of the topmost element.
 * @note This function returns #OSETT_NOTYPETAG if the message has no arguments
 * @f$\textrm{PEEKMESSARGTYPE}(S(E_1(I^\tau_{1_1}))) \Rightarrow t@f$
 */
char ose_peekMessageArgType(const ose_bundle bundle);




/**
 * @brief Get the type of the topmost element.
 * @f$\textrm{PEEKTYPE}(S(E^\tau_1)) \Rightarrow t@f$
 */
char ose_peekType(const ose_bundle bundle);




/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKINT32}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^i@f$
 *
 * @warning This function does no type or bounds checking.
 */
int32_t ose_peekInt32(const ose_bundle bundle);




/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKFLOAT}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^f@f$
 *
 * @warning This function does no type or bounds checking.
 */
float ose_peekFloat(const ose_bundle bundle);




/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKSTRING}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^s@f$
 *
 * @warning This function does no type or bounds checking.
 */
char *ose_peekString(const ose_bundle bundle);




/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKBLOB}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^b@f$
 *
 * @warning This function does no type or bounds checking.
 */
char *ose_peekBlob(const ose_bundle bundle);




#ifdef OSE_PROVIDE_TYPE_SYMBOL
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKSYMBOL}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^S@f$
 *
 * @warning This function does no type or bounds checking.
 */
char *ose_peekSymbol(const ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_DOUBLE
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKDOUBLE}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^d@f$
 *
 * @warning This function does no type or bounds checking.
 */
double ose_peekDouble(const ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_INT8
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKINT8}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^c@f$
 *
 * @warning This function does no type or bounds checking.
 */
int8_t ose_peekInt8(const ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT8
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKUINT8}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^C@f$
 *
 * @warning This function does no type or bounds checking.
 */
uint8_t ose_peekUInt8(const ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT32
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKUINT32}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^k@f$
 *
 * @warning This function does no type or bounds checking.
 */
uint32_t ose_peekUInt32(const ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_INT64
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKINT64}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^h@f$
 *
 * @warning This function does no type or bounds checking.
 */
int64_t ose_peekInt64(const ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT64
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKUINT64}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^H@f$
 *
 * @warning This function does no type or bounds checking.
 */
uint64_t ose_peekUInt64(const ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_TIMETAG
/**
 * @brief Get the value of the topmost item of the topmost element.
 * @f$\textrm{PEEKTIMETAG}(S(E_1(I^\tau_{1_1}))) \Rightarrow v_{1_1}^t@f$
 *
 * @warning This function does no type or bounds checking.
 */
struct ose_timetag ose_peekTimetag(const ose_bundle bundle);
#endif




/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPINT32}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^i)@f$
 *
 * @warning This function does no type or bounds checking.
 */
int32_t ose_popInt32(ose_bundle bundle);




/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPFLOAT}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^f)@f$
 *
 * @warning This function does no type or bounds checking.
 */
float ose_popFloat(ose_bundle bundle);




/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPSTRING}(S(E_1(I^\tau_{1_1})), v^{char*}) \Rightarrow 
 * (S'(E_1)', v_{1_1}^s, \textrm{LENGTH}(v_{1_1}))@f$
 *
 * @param bundle The bundle
 * @param buf A pointer to an array large enough to store a copy of 
 * the string.
 * @returns The length of the string, as reported by 
 * [strlen](https://pubs.opengroup.org/onlinepubs/9699919799/).
 * @warning This function does no type or bounds checking.
 */
int32_t ose_popString(ose_bundle bundle, char *buf);




/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPBLOB}(S(E_1(I^\tau_{1_1})), v^{char*}) \Rightarrow 
 * (S'(E_1)', v_{1_1}^{char*}, \textrm{LENGTH}(v_{1_1}))@f$
 *
 * @param bundle The bundle
 * @param buf A pointer to an array large enough to store a copy of 
 * the blob payload, i.e. not including the 4 byte size.
 * @returns The size of the blob (not including the 4 byte size, or any padding.
 * @warning This function does no type or bounds checking.
 */
int32_t ose_popBlob(ose_bundle bundle, char *buf);




#ifdef OSE_PROVIDE_TYPE_SYMBOL
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPSYMBOL}(S(E_1(I^\tau_{1_1})), v^{char*}) \Rightarrow 
 * (S'(E_1)', v_{1_1}^S, \textrm{LENGTH}(v_{1_1}))@f$
 *
 * @param bundle The bundle
 * @param buf A pointer to an array large enough to store a copy of 
 * the symbol (string).
 * @returns The length of the symbol (string), as reported by 
 * [strlen](https://pubs.opengroup.org/onlinepubs/9699919799/).
 * @warning This function does no type or bounds checking.
 */
int32_t ose_popSymbol(ose_bundle bundle, char *buf);
#endif




#ifdef OSE_PROVIDE_TYPE_DOUBLE
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPDOUBLE}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^d)@f$
 *
 * @warning This function does no type or bounds checking.
 */
double ose_popDouble(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_INT8
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPINT8}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^c)@f$
 *
 * @warning This function does no type or bounds checking.
 */
int8_t ose_popInt8(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT8
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPUINT8}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^C)@f$
 *
 * @warning This function does no type or bounds checking.
 */
uint8_t ose_popUInt8(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT32
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPUINT32}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^k)@f$
 *
 * @warning This function does no type or bounds checking.
 */
uint32_t ose_popUInt32(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_INT64
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPINT64}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^h)@f$
 *
 * @warning This function does no type or bounds checking.
 */
int64_t ose_popInt64(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_UINT64
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPUINT64}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^H)@f$
 *
 * @warning This function does no type or bounds checking.
 */
uint64_t ose_popUInt64(ose_bundle bundle);
#endif




#ifdef OSE_PROVIDE_TYPE_TIMETAG
/**
 * @brief Get the value of the topmost item of the topmost element,
 * and remove it from the stack.
 * @f$\textrm{POPTIMETAG}(S(E_1(I^\tau_{1_1}))) \Rightarrow 
 * (S'(E_1)', v_{1_1}^t)@f$
 *
 * @warning This function does no type or bounds checking.
 */
struct ose_timetag ose_popTimetag(ose_bundle bundle);
#endif




/**@}*/




/**
 * @name OSE API
 */
/**@{*/

/**************************************************
 * Stack Operations
 **************************************************/
/**
 * @brief Drop the top two elements on the stack.
 * @f$\mathrm{2DROP}(S(E_1, E_2)) \Rightarrow S'@f$
 */
void ose_2drop(ose_bundle bundle);




/**
 * @brief Duplicate the top two elements on the stack.
 * @f$\mathrm{2DUP}(S(E_1, E_2)) \Rightarrow 
 * S'(E_1, E_2, E_1, E_2)@f$
 */
void ose_2dup(ose_bundle bundle);




/**
 * @brief Copy elements three and four /over/ elements
 * one and two to the top of the stack.
 * @f$\mathrm{2OVER}(S(E_1, E_2, E_3, E_4)) \Rightarrow 
 * S'(E_3, E_4, E_1, E_2, E_3, E_4)@f$
 */
void ose_2over(ose_bundle bundle);




/**
 * @brief Swap elements three and four with one and two.
 * @f$\mathrm{2SWAP}(S(E_1, E_2, E_3, E_4)) \Rightarrow 
 * S'(E_3, E_4, E_1, E_2)@f$
 */
void ose_2swap(ose_bundle bundle);




/**
 * @brief Drop the top element on the stack.
 * @f$\mathrm{DROP}(S(E_1)) \Rightarrow S'@f$
 */
void ose_dropAtOffset(ose_bundle bundle, int32_t offset);
void ose_drop(ose_bundle bundle);




/**
 * @brief Duplicate the top element on the stack.
 * @f$\mathrm{DUP}(S(E_1)) \Rightarrow S'(E_1, E_1)@f$
 */
void ose_dup(ose_bundle bundle);




/**
 * @brief Drop the second item on the stack.
 * @f$\mathrm{NIP}(S(E_1, E_2)) \Rightarrow S'(E_1)@f$
 */
void ose_nip(ose_bundle bundle);




/**
 * @brief Move the top element of the stack to the third position.
 * @f$\mathrm{ROT}(S(E_1, E_2, E_3)) \Rightarrow S'(E_2, E_3, E_1)@f$
 */
void ose_notrot(ose_bundle bundle);




/**
 * @brief Copy the second item /over/ the first to the top of the stack.
 * @f$\mathrm{over}(S(E_1, E_2)) \Rightarrow S'(E_2, E_1, E_2)@f$
 */
void ose_over(ose_bundle bundle);




/**
 * @brief Copy the @f$ith@f$ element to the top of the stack, where
 * @f$i@f$ is the value of the topmost item of the topmost element.
 * 
 * The topmost item of the topmost element must be an integer @f$I_1^i@f$.
 *
 * @f$\mathrm{PICK}(S(E_1(I_{1_1}^i), E_2,\ldots, E_i)) \Rightarrow 
 * S'(E_i, E_2, \ldots, E_i)@f$
 */
void ose_pick(ose_bundle bundle);




/**
 * @brief Copy the bottommost element of the stack to the top
 * @f$\mathrm{PICKBOTTOM}(S(E_1, \ldots, E_N)) \Rightarrow 
 * S'(E_N, E_1, \ldots, E_N)@f$
 */
void ose_pickBottom(ose_bundle bundle);




/**
 * @brief Copy the bottommost element whose address matches
 * the string contained in the topmost item of the topmost element.
 *
 * The topmost item of the topmost element must be a string.
 * Addresses are compared starting at the beginning of the bundle
 * (the bottom of the stack), and the first address that matches
 * is copied, halting the matching process. Matching is done by 
 * simple string comparison---no pattern matching
 * is performed.
 * 
 * @f$\mathrm{PICKMATCH}(S(E_1^{msg}(I_{1_1}^s, \ldots, E_i(A=I_{1_1}|\ldots))))
 * \Rightarrow S'(E_1, \ldots, E_i)@f$
 */
int32_t ose_pickMatch_impl(ose_bundle bundle);
void ose_pickMatch(ose_bundle bundle);
int32_t ose_pickPMatch_impl(ose_bundle bundle);
void ose_picPkMatch(ose_bundle bundle);




/**
 * @brief Move the @f$ith@f$ element to the top of the stack, where
 * @f$i@f$ is the value of the topmost item of the topmost element.
 * 
 * The topmost item of the topmost element must be an integer @f$I_1^i@f$.
 *
 * @f$\mathrm{ROLL}(S(E_1(I_{1_1}^i), E_2,\ldots, E_i)) \Rightarrow 
 * S'(E_i, E_1, E_2)@f$
 */
void ose_roll(ose_bundle bundle);




/**
 * @brief Move the bottommost element of the stack to the top
 * @f$\mathrm{ROLLBOTTOM}(S(E_1, \ldots, E_N)) \Rightarrow 
 * S'(E_N, E_1, \ldots)@f$
 */
void ose_rollBottom(ose_bundle bundle);




/**
 * @brief Move the bottommost element whose address matches
 * the string contained in the topmost item of the topmost element.
 *
 * The topmost item of the topmost element must be a string.
 * Addresses are compared starting at the beginning of the bundle
 * (the bottom of the stack), and the first address that matches
 * is copied, halting the matching process. Matching is done by 
 * simple string comparison---no pattern matching
 * is performed.
 * 
 * @f$\mathrm{ROLLMATCH}(S(E_1^{msg}(I_{1_1}^s, \ldots, E_i(A=I_{1_1}|\ldots))))
 * \Rightarrow S'(E_1, \ldots)@f$
 */
int32_t ose_rollMatch_impl(ose_bundle bundle);
void ose_rollMatch(ose_bundle bundle);
int32_t ose_rollPMatch_impl(ose_bundle bundle);
void ose_picPkMatch(ose_bundle bundle);




/**
 * @brief Move the third element to the top of the stack.
 * @f$\mathrm{ROT}(S(E_1, E_2, E_3)) \Rightarrow S'(E_3, E_1, E_2)@f$
 */
void ose_rot(ose_bundle bundle);




/**
 * @brief Swap the top two elements on the stack.
 * @f$\mathrm{SWAP}(S(E_1, E_2)) \Rightarrow S'(E_2, E_1)@f$
 */
void ose_swap(ose_bundle bundle);




/**
 * @brief Copy the element on top of the stack below the second element.
 * @f$\mathrm{TUCK}(S(E_1, E_2)) \Rightarrow S'(E_1, E_2, E_1)@f$
 */
void ose_tuck(ose_bundle bundle);

/**************************************************
 * Grouping / Ungrouping
 **************************************************/
void ose_bundleAll(ose_bundle bundle);
void ose_bundleFromBottom(ose_bundle bundle);
void ose_bundleFromTop(ose_bundle bundle);
void ose_clear(ose_bundle bundle);
void ose_clearPayload(ose_bundle bundle);
void ose_pop(ose_bundle bundle);
void ose_popAll(ose_bundle bundle);
void ose_popAllDrop(ose_bundle bundle);
void ose_popAllBundle(ose_bundle bundle);
void ose_popAllDropBundle(ose_bundle bundle);
void ose_push(ose_bundle bundle);
void ose_split(ose_bundle bundle);
void ose_join(ose_bundle bundle);
void ose_unpack(ose_bundle bundle);
void ose_unpackDrop(ose_bundle bundle);
void ose_unpackBundle(ose_bundle bundle);
void ose_unpackDropBundle(ose_bundle bundle);


/**************************************************
 * Queries
 **************************************************/
void ose_countElems(ose_bundle bundle);
void ose_countItems(ose_bundle bundle);
void ose_lengthAddress(ose_bundle bundle);
void ose_lengthTT(ose_bundle bundle);
void ose_lengthItem(ose_bundle bundle);
void ose_lengthsItems(ose_bundle bundle);
void ose_sizeAddress(ose_bundle bundle);
void ose_sizeElem(ose_bundle bundle);
void ose_sizeItem(ose_bundle bundle);
void ose_sizePayload(ose_bundle bundle);
void ose_sizesElems(ose_bundle bundle);
void ose_sizesItems(ose_bundle bundle);
void ose_sizeTT(ose_bundle bundle);

/**************************************************
 * Operations on Bundle Elements and Items
 **************************************************/
void ose_blobToElem(ose_bundle bundle);
void ose_blobToType(ose_bundle bundle);
void ose_concatenateBlobs(ose_bundle bundle);
void ose_concatenateStrings(ose_bundle bundle);
void ose_copyAddressToString(ose_bundle bundle);
void ose_copyPayloadToBlob(ose_bundle bundle);
void ose_swapStringToAddress(ose_bundle bundle);
void ose_copyTTToBlob(ose_bundle bundle);
void ose_decatenateBlobFromEnd(ose_bundle bundle);
void ose_decatenateBlobFromStart(ose_bundle bundle);
void ose_decatenateStringFromEnd(ose_bundle bundle);
void ose_decatenateStringFromStart(ose_bundle bundle);
void ose_elemToBlob(ose_bundle bundle);
void ose_itemToBlob(ose_bundle bundle);
void ose_joinStrings(ose_bundle bundle);
void ose_moveStringToAddress(ose_bundle bundle);
void ose_splitStringFromEnd(ose_bundle bundle);
void ose_splitStringFromStart(ose_bundle bundle);
void ose_swap4Bytes(ose_bundle bundle);
void ose_swap8Bytes(ose_bundle bundle);
void ose_swapNBytes(ose_bundle bundle);
void ose_trimStringEnd(ose_bundle bundle);
void ose_trimStringStart(ose_bundle bundle);
void ose_match(ose_bundle bundle);
void ose_pmatch(ose_bundle bundle);
void ose_replace(ose_bundle bundle);
void ose_assign(ose_bundle bundle);
void ose_lookup(ose_bundle bundle);

/**************************************************
 * Creatio Ex Nihilo
 **************************************************/
void ose_makeBlob(ose_bundle bundle);
void ose_pushBundle(ose_bundle bundle);

/**@}*/

/**
 * @name Builtin Math Functions
 */
/**@{*/
/**************************************************
 * Arithmetic
 **************************************************/
void ose_add(ose_bundle bundle);
void ose_sub(ose_bundle bundle);
void ose_mul(ose_bundle bundle);
void ose_div(ose_bundle bundle);
void ose_mod(ose_bundle bundle);
void ose_pow(ose_bundle bundle);
void ose_neg(ose_bundle bundle);
void ose_eql(ose_bundle bundle);
void ose_neq(ose_bundle bundle);
void ose_lte(ose_bundle bundle);
void ose_lt(ose_bundle bundle);
void ose_and(ose_bundle bundle);
void ose_or(ose_bundle bundle);
/**@}*/

#ifdef __cplusplus
}
#endif

#endif
