/*
  Copyright (c) 2020 John MacCallum
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

(function(__module = typeof libose === 'undefined' ? Module : libose,
	  __exports = typeof exports === 'undefined' ? this['ose'] = {} : exports){
    __exports['newBundle'] = (size = 1024)=>{
	return __module.ccall('osejs_newBundle', 'number', ['number'], [size]);
    };
    __exports['OSETT_INT32'] = 105;
    __exports['OSETT_FLOAT'] = 102;
    __exports['OSETT_STRING'] = 115;
    __exports['OSETT_BLOB'] = 98;
    __exports['OSETT_TRUE'] = 84;
    __exports['OSETT_FALSE'] = 70;
    __exports['OSETT_ID'] = 44;
    __exports['OSETT_BUNDLE'] = 124;
    __exports['OSETT_MESSAGE'] = 45;
    
    __exports['pushInt32'] = __module.cwrap('ose_pushInt32', null, ['number', 'number']);
    __exports['pushFloat'] = __module.cwrap('ose_pushFloat', null, ['number', 'number']);
    __exports['pushString'] = __module.cwrap('ose_pushString', null, ['number', 'string']);
    __exports['pushBlob'] = __module.cwrap('ose_pushBlob', null, ['number', 'number', 'number']);
    __exports['peekAddress'] = __module.cwrap('ose_peekAddress', 'string', ['number']);
    __exports['peekAddress'] = __module.cwrap('ose_peekAddress', 'number', ['number']);
    __exports['peekInt32'] = __module.cwrap('ose_peekInt32', 'number', ['number']);
    __exports['peekFloat'] = __module.cwrap('ose_peekFloat', 'number', ['number']);
    __exports['peekString'] = __module.cwrap('ose_peekString', 'string', ['number']);
    __exports['peekBlob'] = __module.cwrap('ose_peekBlob', 'number', ['number']);
    __exports['popInt32'] = __module.cwrap('ose_popInt32', 'number', ['number']);
    __exports['popFloat'] = __module.cwrap('ose_popFloat', 'number', ['number']);
    __exports['withPoppedString'] = (bundle, fn, context)=>{
    	let str = __module.ccall('osejs_popString', 'string', ['number'], [bundle]);
	let ret = fn(str, context);
	__module._free(str);
	return ret;
    };
    __exports['withPoppedBlob'] = (bundle, fn, context)=>{
    	let str = __module.ccall('osejs_popBlob', 'number', ['number'], [bundle]);
	let ret = fn(str, context);
	__module._free(str);
	return ret;
    };

    __exports['ose_2drop'] = __module.cwrap('ose_2drop', null, ['number']);
    __exports['2dup'] = __module.cwrap('ose_2dup', null, ['number']);
    __exports['2over'] = __module.cwrap('ose_2over', null, ['number']);
    __exports['2swap'] = __module.cwrap('ose_2swap', null, ['number']);
    __exports['drop'] = __module.cwrap('ose_drop', null, ['number']);
    __exports['dup'] = __module.cwrap('ose_dup', null, ['number']);
    __exports['nip'] = __module.cwrap('ose_nip', null, ['number']);
    __exports['notrot'] = __module.cwrap('ose_notrot', null, ['number']);
    __exports['over'] = __module.cwrap('ose_over', null, ['number']);
    __exports['pick'] = __module.cwrap('ose_pick', null, ['number']);
    __exports['pickBottom'] = __module.cwrap('ose_pickBottom', null, ['number']);
    __exports['pickMatch'] = __module.cwrap('ose_pickMatch', null, ['number']);
    __exports['roll'] = __module.cwrap('ose_roll', null, ['number']);
    __exports['rollBottom'] = __module.cwrap('ose_rollBottom', null, ['number']);
    __exports['rollMatch'] = __module.cwrap('ose_rollMatch', null, ['number']);
    __exports['rot'] = __module.cwrap('ose_rot', null, ['number']);
    __exports['swap'] = __module.cwrap('ose_swap', null, ['number']);
    __exports['tuck'] = __module.cwrap('ose_tuck', null, ['number']);
    
    __exports['bundleAll'] = __module.cwrap('ose_bundleAll', null, ['number']);
    __exports['bundleFromBottom'] = __module.cwrap('ose_bundleFromBottom', null, ['number']);
    __exports['bundleFromTop'] = __module.cwrap('ose_bundleFromTop', null, ['number']);
    __exports['clear'] = __module.cwrap('ose_clear', null, ['number']);
    __exports['clearPayload'] = __module.cwrap('ose_clearPayload', null, ['number']);
    __exports['pop'] = __module.cwrap('ose_pop', null, ['number']);
    __exports['popAll'] = __module.cwrap('ose_popAll', null, ['number']);
    __exports['popAllDrop'] = __module.cwrap('ose_popAllDrop', null, ['number']);
    __exports['popAllBundle'] = __module.cwrap('ose_popAllBundle', null, ['number']);
    __exports['popAllDropBundle'] = __module.cwrap('ose_popAllDropBundle', null, ['number']);
    __exports['push'] = __module.cwrap('ose_push', null, ['number']);
    __exports['unpack'] = __module.cwrap('ose_unpack', null, ['number']);
    __exports['unpackDrop'] = __module.cwrap('ose_unpackDrop', null, ['number']);
    __exports['unpackBundle'] = __module.cwrap('ose_unpackBundle', null, ['number']);
    __exports['unpackDropBundle'] = __module.cwrap('ose_unpackDropBundle', null, ['number']);
    
    __exports['countElems'] = __module.cwrap('ose_countElems', null, ['number']);
    __exports['countItems'] = __module.cwrap('ose_countItems', null, ['number']);
    __exports['lengthAddress'] = __module.cwrap('ose_lengthAddress', null, ['number']);
    __exports['lengthTT'] = __module.cwrap('ose_lengthTT', null, ['number']);
    __exports['lengthItem'] = __module.cwrap('ose_lengthItem', null, ['number']);
    __exports['lengthsItems'] = __module.cwrap('ose_lengthsItems', null, ['number']);
    __exports['sizeAddress'] = __module.cwrap('ose_sizeAddress', null, ['number']);
    __exports['sizeElem'] = __module.cwrap('ose_sizeElem', null, ['number']);
    __exports['sizeItem'] = __module.cwrap('ose_sizeItem', null, ['number']);
    __exports['sizePayload'] = __module.cwrap('ose_sizePayload', null, ['number']);
    __exports['sizesElems'] = __module.cwrap('ose_sizesElems', null, ['number']);
    __exports['sizesItems'] = __module.cwrap('ose_sizesItems', null, ['number']);
    __exports['sizeTT'] = __module.cwrap('ose_sizeTT', null, ['number']);
    
    __exports['blobToElem'] = __module.cwrap('ose_blobToElem', null, ['number']);
    __exports['blobToType'] = __module.cwrap('ose_blobToType', null, ['number']);
    __exports['concatenateBlobs'] = __module.cwrap('ose_concatenateBlobs', null, ['number']);
    __exports['concatenateStrings'] = __module.cwrap('ose_concatenateStrings', null, ['number']);
    __exports['copyAddressToString'] = __module.cwrap('ose_copyAddressToString', null, ['number']);
    __exports['copyPayloadToBlob'] = __module.cwrap('ose_copyPayloadToBlob', null, ['number']);
    __exports['swapStringToAddress'] = __module.cwrap('ose_swapStringToAddress', null, ['number']);
    __exports['copyTTToBlob'] = __module.cwrap('ose_copyTTToBlob', null, ['number']);
    __exports['decatenateBlobFromEnd'] = __module.cwrap('ose_decatenateBlobFromEnd', null, ['number']);
    __exports['decatenateBlobFromStart'] = __module.cwrap('ose_decatenateBlobFromStart', null, ['number']);
    __exports['decatenateStringFromEnd'] = __module.cwrap('ose_decatenateStringFromEnd', null, ['number']);
    __exports['decatenateStringFromStart'] = __module.cwrap('ose_decatenateStringFromStart', null, ['number']);
    __exports['elemToBlob'] = __module.cwrap('ose_elemToBlob', null, ['number']);
    __exports['itemToBlob'] = __module.cwrap('ose_itemToBlob', null, ['number']);
    __exports['joinStrings'] = __module.cwrap('ose_joinStrings', null, ['number']);
    __exports['moveStringToAddress'] = __module.cwrap('ose_moveStringToAddress', null, ['number']);
    __exports['splitStringFromEnd'] = __module.cwrap('ose_splitStringFromEnd', null, ['number']);
    __exports['splitStringFromStart'] = __module.cwrap('ose_splitStringFromStart', null, ['number']);
    __exports['swap4Bytes'] = __module.cwrap('ose_swap4Bytes', null, ['number']);
    __exports['swap8Bytes'] = __module.cwrap('ose_swap8Bytes', null, ['number']);
    __exports['swapNBytes'] = __module.cwrap('ose_swapNBytes', null, ['number']);
    __exports['trimStringEnd'] = __module.cwrap('ose_trimStringEnd', null, ['number']);
    __exports['trimStringStart'] = __module.cwrap('ose_trimStringStart', null, ['number']);
    __exports['match'] = __module.cwrap('ose_match', null, ['number']);
    __exports['pmatch'] = __module.cwrap('ose_pmatch', null, ['number']);
    
    __exports['makeBlob'] = __module.cwrap('ose_makeBlob', null, ['number']);
    __exports['pushBundle'] = __module.cwrap('ose_pushBundle', null, ['number']);
    
    __exports['add'] = __module.cwrap('ose_add', null, ['number']);
    __exports['sub'] = __module.cwrap('ose_sub', null, ['number']);
    __exports['mul'] = __module.cwrap('ose_mul', null, ['number']);
    __exports['div'] = __module.cwrap('ose_div', null, ['number']);
    __exports['mod'] = __module.cwrap('ose_mod', null, ['number']);
    __exports['neg'] = __module.cwrap('ose_neg', null, ['number']);
    __exports['eql'] = __module.cwrap('ose_eql', null, ['number']);
    __exports['lte'] = __module.cwrap('ose_lte', null, ['number']);
    __exports['lt'] = __module.cwrap('ose_lt', null, ['number']);
    __exports['and'] = __module.cwrap('ose_and', null, ['number']);
    __exports['or'] = __module.cwrap('ose_or', null, ['number']);
    
    __exports['withPPrintedBundle'] = function(b, fn){
	let st = __module.ccall('osejs_pprintBundle', 'string', ['number'], [b]);
	fn(st);
	__module._free(st);
    };
    
    __exports['spaceAvailable'] = __module.cwrap('ose_spaceAvailable', 'number', ['number']);
    
})();
