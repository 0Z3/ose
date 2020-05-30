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

var ose = {
    'newBundle' : (size = 1024)=>{
	return Module.ccall('osejs_newBundle', 'number', ['number'], [size]);
    },
    'OSETT_INT32' : 105,
    'OSETT_FLOAT' : 102,
    'OSETT_STRING' : 115,
    'OSETT_BLOB' : 98,
    'OSETT_TRUE' : 84,
    'OSETT_FALSE' : 70,
    'OSETT_ID' : 44,
    'OSETT_BUNDLE' : 124,
    'OSETT_MESSAGE' : 45,
    
    'pushInt32' : Module.cwrap('ose_pushInt32', null, ['number', 'number']),
    'pushFloat' : Module.cwrap('ose_pushFloat', null, ['number', 'number']),
    'pushString' : Module.cwrap('ose_pushString', null, ['number', 'string']),
    'pushBlob' : Module.cwrap('ose_pushBlob', null, ['number', 'number', 'number']),
    'peekAddress' : Module.cwrap('ose_peekAddress', 'string', ['number']),
    'peekAddress' : Module.cwrap('ose_peekAddress', 'number', ['number']),
    'peekInt32' : Module.cwrap('ose_peekInt32', 'number', ['number']),
    'peekFloat' : Module.cwrap('ose_peekFloat', 'number', ['number']),
    'peekString' : Module.cwrap('ose_peekString', 'string', ['number']),
    'peekBlob' : Module.cwrap('ose_peekBlob', 'number', ['number']),
    'popInt32' : Module.cwrap('ose_popInt32', 'number', ['number']),
    'popFloat' : Module.cwrap('ose_popFloat', 'number', ['number']),
    'withPoppedString' : (bundle, fn, context)=>{
    	let str = Module.ccall('osejs_popString', 'string', ['number'], [bundle]);
	let ret = fn(str, context);
	Module._free(str);
	return ret;
    },
    'withPoppedBlob' : (bundle, fn, context)=>{
    	let str = Module.ccall('osejs_popBlob', 'number', ['number'], [bundle]);
	let ret = fn(str, context);
	Module._free(str);
	return ret;
    },

    'ose_2drop' : Module.cwrap('ose_2drop', null, ['number']),
    '2dup' : Module.cwrap('ose_2dup', null, ['number']),
    '2over' : Module.cwrap('ose_2over', null, ['number']),
    '2swap' : Module.cwrap('ose_2swap', null, ['number']),
    'drop' : Module.cwrap('ose_drop', null, ['number']),
    'dup' : Module.cwrap('ose_dup', null, ['number']),
    'nip' : Module.cwrap('ose_nip', null, ['number']),
    'notrot' : Module.cwrap('ose_notrot', null, ['number']),
    'over' : Module.cwrap('ose_over', null, ['number']),
    'pick' : Module.cwrap('ose_pick', null, ['number']),
    'pickBottom' : Module.cwrap('ose_pickBottom', null, ['number']),
    'pickMatch' : Module.cwrap('ose_pickMatch', null, ['number']),
    'roll' : Module.cwrap('ose_roll', null, ['number']),
    'rollBottom' : Module.cwrap('ose_rollBottom', null, ['number']),
    'rollMatch' : Module.cwrap('ose_rollMatch', null, ['number']),
    'rot' : Module.cwrap('ose_rot', null, ['number']),
    'swap' : Module.cwrap('ose_swap', null, ['number']),
    'tuck' : Module.cwrap('ose_tuck', null, ['number']),

    'bundleAll' : Module.cwrap('ose_bundleAll', null, ['number']),
    'bundleFromBottom' : Module.cwrap('ose_bundleFromBottom', null, ['number']),
    'bundleFromTop' : Module.cwrap('ose_bundleFromTop', null, ['number']),
    'clear' : Module.cwrap('ose_clear', null, ['number']),
    'clearPayload' : Module.cwrap('ose_clearPayload', null, ['number']),
    'pop' : Module.cwrap('ose_pop', null, ['number']),
    'popAll' : Module.cwrap('ose_popAll', null, ['number']),
    'popAllDrop' : Module.cwrap('ose_popAllDrop', null, ['number']),
    'popAllBundle' : Module.cwrap('ose_popAllBundle', null, ['number']),
    'popAllDropBundle' : Module.cwrap('ose_popAllDropBundle', null, ['number']),
    'push' : Module.cwrap('ose_push', null, ['number']),
    'unpack' : Module.cwrap('ose_unpack', null, ['number']),
    'unpackDrop' : Module.cwrap('ose_unpackDrop', null, ['number']),
    'unpackBundle' : Module.cwrap('ose_unpackBundle', null, ['number']),
    'unpackDropBundle' : Module.cwrap('ose_unpackDropBundle', null, ['number']),

    'countElems' : Module.cwrap('ose_countElems', null, ['number']),
    'countItems' : Module.cwrap('ose_countItems', null, ['number']),
    'lengthAddress' : Module.cwrap('ose_lengthAddress', null, ['number']),
    'lengthTT' : Module.cwrap('ose_lengthTT', null, ['number']),
    'lengthItem' : Module.cwrap('ose_lengthItem', null, ['number']),
    'lengthsItems' : Module.cwrap('ose_lengthsItems', null, ['number']),
    'sizeAddress' : Module.cwrap('ose_sizeAddress', null, ['number']),
    'sizeElem' : Module.cwrap('ose_sizeElem', null, ['number']),
    'sizeItem' : Module.cwrap('ose_sizeItem', null, ['number']),
    'sizePayload' : Module.cwrap('ose_sizePayload', null, ['number']),
    'sizesElems' : Module.cwrap('ose_sizesElems', null, ['number']),
    'sizesItems' : Module.cwrap('ose_sizesItems', null, ['number']),
    'sizeTT' : Module.cwrap('ose_sizeTT', null, ['number']),

    'blobToElem' : Module.cwrap('ose_blobToElem', null, ['number']),
    'blobToType' : Module.cwrap('ose_blobToType', null, ['number']),
    'concatenateBlobs' : Module.cwrap('ose_concatenateBlobs', null, ['number']),
    'concatenateStrings' : Module.cwrap('ose_concatenateStrings', null, ['number']),
    'copyAddressToString' : Module.cwrap('ose_copyAddressToString', null, ['number']),
    'copyPayloadToBlob' : Module.cwrap('ose_copyPayloadToBlob', null, ['number']),
    'swapStringToAddress' : Module.cwrap('ose_swapStringToAddress', null, ['number']),
    'copyTTToBlob' : Module.cwrap('ose_copyTTToBlob', null, ['number']),
    'decatenateBlob' : Module.cwrap('ose_decatenateBlob', null, ['number']),
    'decatenateString' : Module.cwrap('ose_decatenateString', null, ['number']),
    'elemToBlob' : Module.cwrap('ose_elemToBlob', null, ['number']),
    'itemToBlob' : Module.cwrap('ose_itemToBlob', null, ['number']),
    'joinBlobs' : Module.cwrap('ose_joinBlobs', null, ['number']),
    'joinStrings' : Module.cwrap('ose_joinStrings', null, ['number']),
    'moveStringToAddress' : Module.cwrap('ose_moveStringToAddress', null, ['number']),
    'splitBlobFromEnd' : Module.cwrap('ose_splitBlobFromEnd', null, ['number']),
    'splitBlobFromStart' : Module.cwrap('ose_splitBlobFromStart', null, ['number']),
    'splitStringFromEnd' : Module.cwrap('ose_splitStringFromEnd', null, ['number']),
    'splitStringFromStart' : Module.cwrap('ose_splitStringFromStart', null, ['number']),
    'swap4Bytes' : Module.cwrap('ose_swap4Bytes', null, ['number']),
    'swap8Bytes' : Module.cwrap('ose_swap8Bytes', null, ['number']),
    'swapNBytes' : Module.cwrap('ose_swapNBytes', null, ['number']),
    'trimBlob' : Module.cwrap('ose_trimBlob', null, ['number']),
    'trimString' : Module.cwrap('ose_trimString', null, ['number']),
    'match' : Module.cwrap('ose_match', null, ['number']),
    'pmatch' : Module.cwrap('ose_pmatch', null, ['number']),

    'makeBlob' : Module.cwrap('ose_makeBlob', null, ['number']),
    'pushBundle' : Module.cwrap('ose_pushBundle', null, ['number']),

    'add' : Module.cwrap('ose_add', null, ['number']),
    'sub' : Module.cwrap('ose_sub', null, ['number']),
    'mul' : Module.cwrap('ose_mul', null, ['number']),
    'div' : Module.cwrap('ose_div', null, ['number']),
    'mod' : Module.cwrap('ose_mod', null, ['number']),
    'neg' : Module.cwrap('ose_neg', null, ['number']),
    'eql' : Module.cwrap('ose_eql', null, ['number']),
    'lte' : Module.cwrap('ose_lte', null, ['number']),
    'lt' : Module.cwrap('ose_lt', null, ['number']),
    'and' : Module.cwrap('ose_and', null, ['number']),
    'or' : Module.cwrap('ose_or', null, ['number']),

    'pprintBundle' : Module.cwrap('osejs_pprintBundle', null, ['number']),

    'spaceAvailable' : Module.cwrap('ose_spaceAvailable', 'number', ['number']),
};
