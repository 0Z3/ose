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
	  __exports = typeof exports === 'undefined' ? this['osevm'] = {} : exports){
    __exports['init'] = __module.cwrap('osevm_init', 'number', ['number']);
    __exports['run'] = __module.cwrap('osevm_run', null, ['number']);
    __exports['input'] = (vm)=>{
	return __module.ccall('ose_enter', 'number', ['number', 'string'],
 			    [vm, "/ii"]);
    };
    __exports['stack'] = (vm)=>{
	return __module.ccall('ose_enter', 'number', ['number', 'string'],
 			    [vm, "/ss"]);
    };
    __exports['env'] = (vm)=>{
	return __module.ccall('ose_enter', 'number', ['number', 'string'],
 			    [vm, "/ee"]);
    };
    __exports['control'] = (vm)=>{
	return __module.ccall('ose_enter', 'number', ['number', 'string'],
 			    [vm, "/cc"]);
    };
    __exports['dump'] = (vm)=>{
	return __module.ccall('ose_enter', 'number', ['number', 'string'],
 			    [vm, "/dd"]);
    };
    __exports['output'] = (vm)=>{
	return __module.ccall('ose_enter', 'number', ['number', 'string'],
 			    [vm, "/oo"]);
    };
})();
