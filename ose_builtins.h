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

#ifndef OSE_BUILTINS_H
#define OSE_BUILTINS_H

#ifdef __cplusplus
extern "C" {
#endif

// this function is defined in ose_symtab.c
void (*ose_builtin_lookup(char *str))(ose_bundle);

#define OSE_BUILTIN_DECL(name)			\
	void ose_builtin_##name(ose_bundle bundle);

OSE_BUILTIN_DECL(2drop)
OSE_BUILTIN_DECL(2dup)
OSE_BUILTIN_DECL(2over)
OSE_BUILTIN_DECL(2swap)
OSE_BUILTIN_DECL(drop)
OSE_BUILTIN_DECL(dup)
OSE_BUILTIN_DECL(nip)
OSE_BUILTIN_DECL(notrot)
OSE_BUILTIN_DECL(over)
OSE_BUILTIN_DECL(pick)
OSE_BUILTIN_DECL(pickBottom)
OSE_BUILTIN_DECL(pickMatch)
OSE_BUILTIN_DECL(roll)
OSE_BUILTIN_DECL(rollBottom)
OSE_BUILTIN_DECL(rollMatch)
OSE_BUILTIN_DECL(rot)
OSE_BUILTIN_DECL(swap)
OSE_BUILTIN_DECL(tuck)

OSE_BUILTIN_DECL(bundleAll)
OSE_BUILTIN_DECL(bundleFromBottom)
OSE_BUILTIN_DECL(bundleFromTop)
OSE_BUILTIN_DECL(clear)
OSE_BUILTIN_DECL(clearPayload)
OSE_BUILTIN_DECL(pop)
OSE_BUILTIN_DECL(popAll)
OSE_BUILTIN_DECL(popAllDrop)
OSE_BUILTIN_DECL(popAllBundle)
OSE_BUILTIN_DECL(popAllDropBundle)
OSE_BUILTIN_DECL(push)
OSE_BUILTIN_DECL(unpack)
OSE_BUILTIN_DECL(unpackDrop)
OSE_BUILTIN_DECL(unpackBundle)
OSE_BUILTIN_DECL(unpackDropBundle)

OSE_BUILTIN_DECL(countElems)
OSE_BUILTIN_DECL(countItems)
OSE_BUILTIN_DECL(lengthAddress)
OSE_BUILTIN_DECL(lengthTT)
OSE_BUILTIN_DECL(lengthItem)
OSE_BUILTIN_DECL(lengthsItems)
OSE_BUILTIN_DECL(sizeAddress)
OSE_BUILTIN_DECL(sizeElem)
OSE_BUILTIN_DECL(sizeItem)
OSE_BUILTIN_DECL(sizePayload)
OSE_BUILTIN_DECL(sizesElems)
OSE_BUILTIN_DECL(sizesItems)
OSE_BUILTIN_DECL(sizeTT)

OSE_BUILTIN_DECL(blobToElem)
OSE_BUILTIN_DECL(blobToType)
OSE_BUILTIN_DECL(concatenateBlobs)
OSE_BUILTIN_DECL(concatenateStrings)
OSE_BUILTIN_DECL(copyAddressToString)
OSE_BUILTIN_DECL(copyPayloadToBlob)
OSE_BUILTIN_DECL(swapStringToAddress)
OSE_BUILTIN_DECL(copyTTToBlob)
OSE_BUILTIN_DECL(decatenateBlobFromEnd)
OSE_BUILTIN_DECL(decatenateBlobFromStart)
OSE_BUILTIN_DECL(decatenateStringFromEnd)
OSE_BUILTIN_DECL(decatenateStringFromStart)
OSE_BUILTIN_DECL(elemToBlob)
OSE_BUILTIN_DECL(itemToBlob)
OSE_BUILTIN_DECL(joinStrings)
OSE_BUILTIN_DECL(moveStringToAddress)
OSE_BUILTIN_DECL(splitStringFromEnd)
OSE_BUILTIN_DECL(splitStringFromStart)
OSE_BUILTIN_DECL(swap4Bytes)
OSE_BUILTIN_DECL(swap8Bytes)
OSE_BUILTIN_DECL(swapNBytes)
OSE_BUILTIN_DECL(trimStringEnd)
OSE_BUILTIN_DECL(trimStringStart)
OSE_BUILTIN_DECL(match)
OSE_BUILTIN_DECL(pmatch)

OSE_BUILTIN_DECL(makeBlob)
OSE_BUILTIN_DECL(pushBundle)

OSE_BUILTIN_DECL(add)
OSE_BUILTIN_DECL(sub)
OSE_BUILTIN_DECL(mul)
OSE_BUILTIN_DECL(div)
OSE_BUILTIN_DECL(mod)
OSE_BUILTIN_DECL(pow)
OSE_BUILTIN_DECL(neg)
OSE_BUILTIN_DECL(eql)
OSE_BUILTIN_DECL(neq)
OSE_BUILTIN_DECL(lte)
OSE_BUILTIN_DECL(lt)
OSE_BUILTIN_DECL(and)
OSE_BUILTIN_DECL(or)

OSE_BUILTIN_DECL(isAddressChar);
OSE_BUILTIN_DECL(isKnownTypetag);
OSE_BUILTIN_DECL(isStringType);
OSE_BUILTIN_DECL(isIntegerType);
OSE_BUILTIN_DECL(isFloatType);
OSE_BUILTIN_DECL(isNumericType);
OSE_BUILTIN_DECL(isUnitType);
OSE_BUILTIN_DECL(isBoolType);

void ose_builtin_eval(ose_bundle osevm);
void ose_builtin_if(ose_bundle osevm);
void ose_builtin_dotimes(ose_bundle osevm);
void ose_builtin_moveElemToDest(ose_bundle osevm);
void ose_builtin_copyElemToDest(ose_bundle osevm);
void ose_builtin_replaceElemInDest(ose_bundle osevm);
void ose_builtin_clearEnv(ose_bundle osevm);
void ose_builtin_copyEnv(ose_bundle osevm);
void ose_builtin_assign(ose_bundle osevm);
void ose_builtin_apply(ose_bundle osevm);
void ose_builtin_defun(ose_bundle osevm);

#ifdef __cplusplus
}
#endif

#endif
