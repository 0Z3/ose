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

(function(exports){
    exports['newBundle'] = (size = 1024)=>{
	return Module.ccall('osejs_newBundle', 'number', ['number'], [size]);
    };
    exports['OSETT_INT32'] = 105;
    exports['OSETT_FLOAT'] = 102;
    exports['OSETT_STRING'] = 115;
    exports['OSETT_BLOB'] = 98;
    exports['OSETT_TRUE'] = 84;
    exports['OSETT_FALSE'] = 70;
    exports['OSETT_ID'] = 44;
    exports['OSETT_BUNDLE'] = 124;
    exports['OSETT_MESSAGE'] = 45;
    
    exports['pushInt32'] = Module.cwrap('ose_pushInt32', null, ['number', 'number']);
    exports['pushFloat'] = Module.cwrap('ose_pushFloat', null, ['number', 'number']);
    exports['pushString'] = Module.cwrap('ose_pushString', null, ['number', 'string']);
    exports['pushBlob'] = Module.cwrap('ose_pushBlob', null, ['number', 'number', 'number']);
    exports['peekAddress'] = Module.cwrap('ose_peekAddress', 'string', ['number']);
    exports['peekAddress'] = Module.cwrap('ose_peekAddress', 'number', ['number']);
    exports['peekInt32'] = Module.cwrap('ose_peekInt32', 'number', ['number']);
    exports['peekFloat'] = Module.cwrap('ose_peekFloat', 'number', ['number']);
    exports['peekString'] = Module.cwrap('ose_peekString', 'string', ['number']);
    exports['peekBlob'] = Module.cwrap('ose_peekBlob', 'number', ['number']);
    exports['popInt32'] = Module.cwrap('ose_popInt32', 'number', ['number']);
    exports['popFloat'] = Module.cwrap('ose_popFloat', 'number', ['number']);
    exports['withPoppedString'] = (bundle, fn, context)=>{
    	let str = Module.ccall('osejs_popString', 'string', ['number'], [bundle]);
	let ret = fn(str, context);
	Module._free(str);
	return ret;
    };
    exports['withPoppedBlob'] = (bundle, fn, context)=>{
    	let str = Module.ccall('osejs_popBlob', 'number', ['number'], [bundle]);
	let ret = fn(str, context);
	Module._free(str);
	return ret;
    };

    exports['ose_2drop'] = Module.cwrap('ose_2drop', null, ['number']);
    exports['2dup'] = Module.cwrap('ose_2dup', null, ['number']);
    exports['2over'] = Module.cwrap('ose_2over', null, ['number']);
    exports['2swap'] = Module.cwrap('ose_2swap', null, ['number']);
    exports['drop'] = Module.cwrap('ose_drop', null, ['number']);
    exports['dup'] = Module.cwrap('ose_dup', null, ['number']);
    exports['nip'] = Module.cwrap('ose_nip', null, ['number']);
    exports['notrot'] = Module.cwrap('ose_notrot', null, ['number']);
    exports['over'] = Module.cwrap('ose_over', null, ['number']);
    exports['pick'] = Module.cwrap('ose_pick', null, ['number']);
    exports['pickBottom'] = Module.cwrap('ose_pickBottom', null, ['number']);
    exports['pickMatch'] = Module.cwrap('ose_pickMatch', null, ['number']);
    exports['roll'] = Module.cwrap('ose_roll', null, ['number']);
    exports['rollBottom'] = Module.cwrap('ose_rollBottom', null, ['number']);
    exports['rollMatch'] = Module.cwrap('ose_rollMatch', null, ['number']);
    exports['rot'] = Module.cwrap('ose_rot', null, ['number']);
    exports['swap'] = Module.cwrap('ose_swap', null, ['number']);
    exports['tuck'] = Module.cwrap('ose_tuck', null, ['number']);
    
    exports['bundleAll'] = Module.cwrap('ose_bundleAll', null, ['number']);
    exports['bundleFromBottom'] = Module.cwrap('ose_bundleFromBottom', null, ['number']);
    exports['bundleFromTop'] = Module.cwrap('ose_bundleFromTop', null, ['number']);
    exports['clear'] = Module.cwrap('ose_clear', null, ['number']);
    exports['clearPayload'] = Module.cwrap('ose_clearPayload', null, ['number']);
    exports['pop'] = Module.cwrap('ose_pop', null, ['number']);
    exports['popAll'] = Module.cwrap('ose_popAll', null, ['number']);
    exports['popAllDrop'] = Module.cwrap('ose_popAllDrop', null, ['number']);
    exports['popAllBundle'] = Module.cwrap('ose_popAllBundle', null, ['number']);
    exports['popAllDropBundle'] = Module.cwrap('ose_popAllDropBundle', null, ['number']);
    exports['push'] = Module.cwrap('ose_push', null, ['number']);
    exports['unpack'] = Module.cwrap('ose_unpack', null, ['number']);
    exports['unpackDrop'] = Module.cwrap('ose_unpackDrop', null, ['number']);
    exports['unpackBundle'] = Module.cwrap('ose_unpackBundle', null, ['number']);
    exports['unpackDropBundle'] = Module.cwrap('ose_unpackDropBundle', null, ['number']);
    
    exports['countElems'] = Module.cwrap('ose_countElems', null, ['number']);
    exports['countItems'] = Module.cwrap('ose_countItems', null, ['number']);
    exports['lengthAddress'] = Module.cwrap('ose_lengthAddress', null, ['number']);
    exports['lengthTT'] = Module.cwrap('ose_lengthTT', null, ['number']);
    exports['lengthItem'] = Module.cwrap('ose_lengthItem', null, ['number']);
    exports['lengthsItems'] = Module.cwrap('ose_lengthsItems', null, ['number']);
    exports['sizeAddress'] = Module.cwrap('ose_sizeAddress', null, ['number']);
    exports['sizeElem'] = Module.cwrap('ose_sizeElem', null, ['number']);
    exports['sizeItem'] = Module.cwrap('ose_sizeItem', null, ['number']);
    exports['sizePayload'] = Module.cwrap('ose_sizePayload', null, ['number']);
    exports['sizesElems'] = Module.cwrap('ose_sizesElems', null, ['number']);
    exports['sizesItems'] = Module.cwrap('ose_sizesItems', null, ['number']);
    exports['sizeTT'] = Module.cwrap('ose_sizeTT', null, ['number']);
    
    exports['blobToElem'] = Module.cwrap('ose_blobToElem', null, ['number']);
    exports['blobToType'] = Module.cwrap('ose_blobToType', null, ['number']);
    exports['concatenateBlobs'] = Module.cwrap('ose_concatenateBlobs', null, ['number']);
    exports['concatenateStrings'] = Module.cwrap('ose_concatenateStrings', null, ['number']);
    exports['copyAddressToString'] = Module.cwrap('ose_copyAddressToString', null, ['number']);
    exports['copyPayloadToBlob'] = Module.cwrap('ose_copyPayloadToBlob', null, ['number']);
    exports['swapStringToAddress'] = Module.cwrap('ose_swapStringToAddress', null, ['number']);
    exports['copyTTToBlob'] = Module.cwrap('ose_copyTTToBlob', null, ['number']);
    exports['decatenateBlob'] = Module.cwrap('ose_decatenateBlob', null, ['number']);
    exports['decatenateString'] = Module.cwrap('ose_decatenateString', null, ['number']);
    exports['elemToBlob'] = Module.cwrap('ose_elemToBlob', null, ['number']);
    exports['itemToBlob'] = Module.cwrap('ose_itemToBlob', null, ['number']);
    exports['joinBlobs'] = Module.cwrap('ose_joinBlobs', null, ['number']);
    exports['joinStrings'] = Module.cwrap('ose_joinStrings', null, ['number']);
    exports['moveStringToAddress'] = Module.cwrap('ose_moveStringToAddress', null, ['number']);
    exports['splitBlobFromEnd'] = Module.cwrap('ose_splitBlobFromEnd', null, ['number']);
    exports['splitBlobFromStart'] = Module.cwrap('ose_splitBlobFromStart', null, ['number']);
    exports['splitStringFromEnd'] = Module.cwrap('ose_splitStringFromEnd', null, ['number']);
    exports['splitStringFromStart'] = Module.cwrap('ose_splitStringFromStart', null, ['number']);
    exports['swap4Bytes'] = Module.cwrap('ose_swap4Bytes', null, ['number']);
    exports['swap8Bytes'] = Module.cwrap('ose_swap8Bytes', null, ['number']);
    exports['swapNBytes'] = Module.cwrap('ose_swapNBytes', null, ['number']);
    exports['trimBlob'] = Module.cwrap('ose_trimBlob', null, ['number']);
    exports['trimString'] = Module.cwrap('ose_trimString', null, ['number']);
    exports['match'] = Module.cwrap('ose_match', null, ['number']);
    exports['pmatch'] = Module.cwrap('ose_pmatch', null, ['number']);
    
    exports['makeBlob'] = Module.cwrap('ose_makeBlob', null, ['number']);
    exports['pushBundle'] = Module.cwrap('ose_pushBundle', null, ['number']);
    
    exports['add'] = Module.cwrap('ose_add', null, ['number']);
    exports['sub'] = Module.cwrap('ose_sub', null, ['number']);
    exports['mul'] = Module.cwrap('ose_mul', null, ['number']);
    exports['div'] = Module.cwrap('ose_div', null, ['number']);
    exports['mod'] = Module.cwrap('ose_mod', null, ['number']);
    exports['neg'] = Module.cwrap('ose_neg', null, ['number']);
    exports['eql'] = Module.cwrap('ose_eql', null, ['number']);
    exports['lte'] = Module.cwrap('ose_lte', null, ['number']);
    exports['lt'] = Module.cwrap('ose_lt', null, ['number']);
    exports['and'] = Module.cwrap('ose_and', null, ['number']);
    exports['or'] = Module.cwrap('ose_or', null, ['number']);
    
    exports['pprintBundle'] = Module.cwrap('osejs_pprintBundle', null, ['number']);
    
    exports['spaceAvailable'] = Module.cwrap('ose_spaceAvailable', 'number', ['number']);
    
})(typeof exports === 'undefined' ? this['ose'] = {} : exports);
