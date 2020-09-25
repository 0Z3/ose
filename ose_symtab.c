/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf ose_symtab.gperf  */
/* Computed positions: -k'2-3,8,$' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#line 1 "ose_symtab.gperf"

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
#include <stdlib.h>
#include <string.h>
#include "ose_conf.h"
#include "ose.h"
#include "ose_stackops.h"
#include "ose_builtins.h"

#ifdef OSE_SYMTAB_FNSYMS
#define OSE_SYMTAB_VALUE(s) s, #s
#else
#define OSE_SYMTAB_VALUE(s) s
#endif

#line 43 "ose_symtab.gperf"
struct _ose_symtab_rec {
	char *name;
	void (*f)(ose_bundle);
#ifdef OSE_SYMTAB_FNSYMS
	char *fnsym;
#endif
};

#define TOTAL_KEYWORDS 109
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 23
#define MIN_HASH_VALUE 8
#define MAX_HASH_VALUE 257
/* maximum key range = 250, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
_ose_symtab_hash (register const char *str, register size_t len)
{
  static const unsigned short asso_values[] =
    {
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258,   0,  60, 258, 258,
      258,  25,  40, 258, 258, 258,  35, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258,  45,  45,  60,
        5,   0,  50,  90,   0,  10,  75,  15,   5,  10,
      110,  25,  15,  20,  40,   0,   0,  65,  60,  55,
        0,   5, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258, 258, 258, 258,
      258, 258, 258, 258, 258, 258, 258
    };
  register unsigned int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[7]];
      /*FALLTHROUGH*/
      case 7:
      case 6:
      case 5:
      case 4:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
        hval += asso_values[(unsigned char)str[1]+1];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static const struct _ose_symtab_rec _ose_symtab_wordlist[] =
  {
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 136 "ose_symtab.gperf"
    {"/replace", OSE_SYMTAB_VALUE(ose_builtin_replace)},
    {""}, {""}, {""}, {""},
#line 157 "ose_symtab.gperf"
    {"/lt", OSE_SYMTAB_VALUE(ose_builtin_lt)},
#line 156 "ose_symtab.gperf"
    {"/lte", OSE_SYMTAB_VALUE(ose_builtin_lte)},
#line 179 "ose_symtab.gperf"
    {"/replace/bundle", OSE_SYMTAB_VALUE(ose_builtin_replaceBundle)},
    {""}, {""},
#line 107 "ose_symtab.gperf"
    {"/size/tt", OSE_SYMTAB_VALUE(ose_builtin_sizeTT)},
    {""}, {""},
#line 86 "ose_symtab.gperf"
    {"/split", OSE_SYMTAB_VALUE(ose_builtin_split)},
#line 105 "ose_symtab.gperf"
    {"/sizes/elems", OSE_SYMTAB_VALUE(ose_builtin_sizesElems)},
#line 122 "ose_symtab.gperf"
    {"/decat/string/fromstart", OSE_SYMTAB_VALUE(ose_builtin_decatenateStringFromStart)},
#line 100 "ose_symtab.gperf"
    {"/lengths/items", OSE_SYMTAB_VALUE(ose_builtin_lengthsItems)},
    {""},
#line 121 "ose_symtab.gperf"
    {"/decat/string/fromend", OSE_SYMTAB_VALUE(ose_builtin_decatenateStringFromEnd)},
    {""},
#line 101 "ose_symtab.gperf"
    {"/size/address", OSE_SYMTAB_VALUE(ose_builtin_sizeAddress)},
#line 69 "ose_symtab.gperf"
    {"/rot", OSE_SYMTAB_VALUE(ose_builtin_rot)},
#line 103 "ose_symtab.gperf"
    {"/size/item", OSE_SYMTAB_VALUE(ose_builtin_sizeItem)},
    {""},
#line 106 "ose_symtab.gperf"
    {"/sizes/items", OSE_SYMTAB_VALUE(ose_builtin_sizesItems)},
#line 176 "ose_symtab.gperf"
    {"/dotimes", OSE_SYMTAB_VALUE(ose_builtin_dotimes)},
#line 66 "ose_symtab.gperf"
    {"/roll/jth", OSE_SYMTAB_VALUE(ose_builtin_roll)},
#line 102 "ose_symtab.gperf"
    {"/size/elem", OSE_SYMTAB_VALUE(ose_builtin_sizeElem)},
    {""},
#line 135 "ose_symtab.gperf"
    {"/pmatch", OSE_SYMTAB_VALUE(ose_builtin_pmatch)},
#line 128 "ose_symtab.gperf"
    {"/split/string/fromstart", OSE_SYMTAB_VALUE(ose_builtin_splitStringFromStart)},
#line 63 "ose_symtab.gperf"
    {"/pick/jth", OSE_SYMTAB_VALUE(ose_builtin_pick)},
    {""},
#line 127 "ose_symtab.gperf"
    {"/split/string/fromend", OSE_SYMTAB_VALUE(ose_builtin_splitStringFromEnd)},
#line 95 "ose_symtab.gperf"
    {"/count/elems", OSE_SYMTAB_VALUE(ose_builtin_countElems)},
    {""},
#line 79 "ose_symtab.gperf"
    {"/clear/payload", OSE_SYMTAB_VALUE(ose_builtin_clearPayload)},
#line 61 "ose_symtab.gperf"
    {"/-rot", OSE_SYMTAB_VALUE(ose_builtin_notrot)},
#line 57 "ose_symtab.gperf"
    {"/2swap", OSE_SYMTAB_VALUE(ose_builtin_2swap)},
    {""}, {""},
#line 155 "ose_symtab.gperf"
    {"/neq", OSE_SYMTAB_VALUE(ose_builtin_neq)},
#line 55 "ose_symtab.gperf"
    {"/2dup", OSE_SYMTAB_VALUE(ose_builtin_2dup)},
#line 54 "ose_symtab.gperf"
    {"/2drop", OSE_SYMTAB_VALUE(ose_builtin_2drop)},
#line 96 "ose_symtab.gperf"
    {"/count/items", OSE_SYMTAB_VALUE(ose_builtin_countItems)},
#line 125 "ose_symtab.gperf"
    {"/join/strings", OSE_SYMTAB_VALUE(ose_builtin_joinStrings)},
#line 60 "ose_symtab.gperf"
    {"/nip", OSE_SYMTAB_VALUE(ose_builtin_nip)},
#line 181 "ose_symtab.gperf"
    {"/copy/elem", OSE_SYMTAB_VALUE(ose_builtin_copyElem)},
#line 78 "ose_symtab.gperf"
    {"/clear", OSE_SYMTAB_VALUE(ose_builtin_clear)},
#line 138 "ose_symtab.gperf"
    {"/lookup", OSE_SYMTAB_VALUE(ose_builtin_lookup)},
#line 115 "ose_symtab.gperf"
    {"/address", OSE_SYMTAB_VALUE(ose_builtin_copyAddressToString)},
#line 147 "ose_symtab.gperf"
    {"/add", OSE_SYMTAB_VALUE(ose_builtin_add)},
#line 58 "ose_symtab.gperf"
    {"/drop", OSE_SYMTAB_VALUE(ose_builtin_drop)},
    {""}, {""},
#line 81 "ose_symtab.gperf"
    {"/pop/all", OSE_SYMTAB_VALUE(ose_builtin_popAll)},
#line 80 "ose_symtab.gperf"
    {"/pop", OSE_SYMTAB_VALUE(ose_builtin_pop)},
#line 83 "ose_symtab.gperf"
    {"/pop/all/bundle", OSE_SYMTAB_VALUE(ose_builtin_popAllBundle)},
#line 120 "ose_symtab.gperf"
    {"/decat/blob/fromstart", OSE_SYMTAB_VALUE(ose_builtin_decatenateBlobFromStart)},
    {""},
#line 118 "ose_symtab.gperf"
    {"/tt", OSE_SYMTAB_VALUE(ose_builtin_copyTTToBlob)},
#line 119 "ose_symtab.gperf"
    {"/decat/blob/fromend", OSE_SYMTAB_VALUE(ose_builtin_decatenateBlobFromEnd)},
#line 84 "ose_symtab.gperf"
    {"/pop/all/drop/bundle", OSE_SYMTAB_VALUE(ose_builtin_popAllDropBundle)},
#line 182 "ose_symtab.gperf"
    {"/apply", OSE_SYMTAB_VALUE(ose_builtin_apply)},
#line 67 "ose_symtab.gperf"
    {"/roll/bottom", OSE_SYMTAB_VALUE(ose_builtin_rollBottom)},
#line 104 "ose_symtab.gperf"
    {"/size/payload", OSE_SYMTAB_VALUE(ose_builtin_sizePayload)},
#line 150 "ose_symtab.gperf"
    {"/div", OSE_SYMTAB_VALUE(ose_builtin_div)},
#line 70 "ose_symtab.gperf"
    {"/swap", OSE_SYMTAB_VALUE(ose_builtin_swap)},
    {""},
#line 64 "ose_symtab.gperf"
    {"/pick/bottom", OSE_SYMTAB_VALUE(ose_builtin_pickBottom)},
#line 82 "ose_symtab.gperf"
    {"/pop/all/drop", OSE_SYMTAB_VALUE(ose_builtin_popAllDrop)},
#line 154 "ose_symtab.gperf"
    {"/eql", OSE_SYMTAB_VALUE(ose_builtin_eql)},
#line 98 "ose_symtab.gperf"
    {"/length/tt", OSE_SYMTAB_VALUE(ose_builtin_lengthTT)},
#line 68 "ose_symtab.gperf"
    {"/roll/match", OSE_SYMTAB_VALUE(ose_builtin_rollMatch)},
#line 126 "ose_symtab.gperf"
    {"/string/toaddress/move", OSE_SYMTAB_VALUE(ose_builtin_moveStringToAddress)},
#line 116 "ose_symtab.gperf"
    {"/payload", OSE_SYMTAB_VALUE(ose_builtin_copyPayloadToBlob)},
#line 59 "ose_symtab.gperf"
    {"/dup", OSE_SYMTAB_VALUE(ose_builtin_dup)},
#line 97 "ose_symtab.gperf"
    {"/length/address", OSE_SYMTAB_VALUE(ose_builtin_lengthAddress)},
#line 65 "ose_symtab.gperf"
    {"/pick/match", OSE_SYMTAB_VALUE(ose_builtin_pickMatch)},
#line 166 "ose_symtab.gperf"
    {"/is/type/int", OSE_SYMTAB_VALUE(ose_builtin_isIntegerType)},
#line 169 "ose_symtab.gperf"
    {"/is/type/unit", OSE_SYMTAB_VALUE(ose_builtin_isUnitType)},
#line 167 "ose_symtab.gperf"
    {"/is/type/float", OSE_SYMTAB_VALUE(ose_builtin_isFloatType)},
#line 85 "ose_symtab.gperf"
    {"/push", OSE_SYMTAB_VALUE(ose_builtin_push)},
    {""},
#line 99 "ose_symtab.gperf"
    {"/length/item", OSE_SYMTAB_VALUE(ose_builtin_lengthItem)},
#line 170 "ose_symtab.gperf"
    {"/is/type/bool", OSE_SYMTAB_VALUE(ose_builtin_isBoolType)},
    {""}, {""},
#line 56 "ose_symtab.gperf"
    {"/2over", OSE_SYMTAB_VALUE(ose_builtin_2over)},
#line 117 "ose_symtab.gperf"
    {"/string/toaddress/swap", OSE_SYMTAB_VALUE(ose_builtin_swapStringToAddress)},
#line 159 "ose_symtab.gperf"
    {"/or", OSE_SYMTAB_VALUE(ose_builtin_or)},
    {""}, {""}, {""},
#line 112 "ose_symtab.gperf"
    {"/blob/totype", OSE_SYMTAB_VALUE(ose_builtin_blobToType)},
#line 113 "ose_symtab.gperf"
    {"/concat/blobs", OSE_SYMTAB_VALUE(ose_builtin_concatenateBlobs)},
#line 152 "ose_symtab.gperf"
    {"/pow", OSE_SYMTAB_VALUE(ose_builtin_pow)},
#line 114 "ose_symtab.gperf"
    {"/concat/strings", OSE_SYMTAB_VALUE(ose_builtin_concatenateStrings)},
    {""},
#line 177 "ose_symtab.gperf"
    {"/copy/bundle", OSE_SYMTAB_VALUE(ose_builtin_copyBundle)},
#line 130 "ose_symtab.gperf"
    {"/swap/bytes/8", OSE_SYMTAB_VALUE(ose_builtin_swap8Bytes)},
    {""}, {""}, {""},
#line 111 "ose_symtab.gperf"
    {"/blob/toelem", OSE_SYMTAB_VALUE(ose_builtin_blobToElem)},
#line 129 "ose_symtab.gperf"
    {"/swap/bytes/4", OSE_SYMTAB_VALUE(ose_builtin_swap4Bytes)},
#line 148 "ose_symtab.gperf"
    {"/sub", OSE_SYMTAB_VALUE(ose_builtin_sub)},
#line 174 "ose_symtab.gperf"
    {"/exec", OSE_SYMTAB_VALUE(ose_builtin_exec)},
    {""},
#line 183 "ose_symtab.gperf"
    {"/return", OSE_SYMTAB_VALUE(ose_builtin_return)},
    {""},
#line 153 "ose_symtab.gperf"
    {"/neg", OSE_SYMTAB_VALUE(ose_builtin_neg)},
#line 62 "ose_symtab.gperf"
    {"/over", OSE_SYMTAB_VALUE(ose_builtin_over)},
    {""}, {""},
#line 133 "ose_symtab.gperf"
    {"/trim/string/start", OSE_SYMTAB_VALUE(ose_builtin_trimStringStart)},
    {""}, {""},
#line 132 "ose_symtab.gperf"
    {"/trim/string/end", OSE_SYMTAB_VALUE(ose_builtin_trimStringEnd)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 178 "ose_symtab.gperf"
    {"/append/bundle", OSE_SYMTAB_VALUE(ose_builtin_appendBundle)},
    {""}, {""},
#line 123 "ose_symtab.gperf"
    {"/elem/toblob", OSE_SYMTAB_VALUE(ose_builtin_elemToBlob)},
    {""}, {""}, {""}, {""}, {""}, {""},
#line 151 "ose_symtab.gperf"
    {"/mod", OSE_SYMTAB_VALUE(ose_builtin_mod)},
#line 143 "ose_symtab.gperf"
    {"/push/blob", OSE_SYMTAB_VALUE(ose_builtin_makeBlob)},
    {""}, {""}, {""}, {""},
#line 71 "ose_symtab.gperf"
    {"/tuck", OSE_SYMTAB_VALUE(ose_builtin_tuck)},
#line 168 "ose_symtab.gperf"
    {"/is/type/numeric", OSE_SYMTAB_VALUE(ose_builtin_isNumericType)},
    {""}, {""}, {""},
#line 87 "ose_symtab.gperf"
    {"/join", OSE_SYMTAB_VALUE(ose_builtin_join)},
    {""},
#line 124 "ose_symtab.gperf"
    {"/item/toblob", OSE_SYMTAB_VALUE(ose_builtin_itemToBlob)},
    {""}, {""},
#line 180 "ose_symtab.gperf"
    {"/move/elem", OSE_SYMTAB_VALUE(ose_builtin_moveElem)},
#line 134 "ose_symtab.gperf"
    {"/match", OSE_SYMTAB_VALUE(ose_builtin_match)},
#line 137 "ose_symtab.gperf"
    {"/assign", OSE_SYMTAB_VALUE(ose_builtin_assign)},
    {""},
#line 158 "ose_symtab.gperf"
    {"/and", OSE_SYMTAB_VALUE(ose_builtin_and)},
    {""}, {""}, {""}, {""}, {""},
#line 163 "ose_symtab.gperf"
    {"/is/addresschar", OSE_SYMTAB_VALUE(ose_builtin_isAddressChar)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 175 "ose_symtab.gperf"
    {"/if", OSE_SYMTAB_VALUE(ose_builtin_if)},
    {""},
#line 165 "ose_symtab.gperf"
    {"/is/type/string", OSE_SYMTAB_VALUE(ose_builtin_isStringType)},
    {""}, {""},
#line 131 "ose_symtab.gperf"
    {"/swap/bytes/n", OSE_SYMTAB_VALUE(ose_builtin_swapNBytes)},
#line 149 "ose_symtab.gperf"
    {"/mul", OSE_SYMTAB_VALUE(ose_builtin_mul)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 88 "ose_symtab.gperf"
    {"/unpack", OSE_SYMTAB_VALUE(ose_builtin_unpack)},
    {""}, {""}, {""}, {""}, {""}, {""},
#line 164 "ose_symtab.gperf"
    {"/is/type/known", OSE_SYMTAB_VALUE(ose_builtin_isKnownTypetag)},
    {""},
#line 75 "ose_symtab.gperf"
    {"/bundle/all", OSE_SYMTAB_VALUE(ose_builtin_bundleAll)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""},
#line 76 "ose_symtab.gperf"
    {"/bundle/frombottom", OSE_SYMTAB_VALUE(ose_builtin_bundleFromBottom)},
    {""},
#line 77 "ose_symtab.gperf"
    {"/bundle/fromtop", OSE_SYMTAB_VALUE(ose_builtin_bundleFromTop)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 142 "ose_symtab.gperf"
    {"/make/bundle", OSE_SYMTAB_VALUE(ose_builtin_pushBundle)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""}, {""},
#line 90 "ose_symtab.gperf"
    {"/unpack/bundle", OSE_SYMTAB_VALUE(ose_builtin_unpackBundle)},
    {""}, {""}, {""}, {""},
#line 91 "ose_symtab.gperf"
    {"/unpack/drop/bundle", OSE_SYMTAB_VALUE(ose_builtin_unpackDropBundle)},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 89 "ose_symtab.gperf"
    {"/unpack/drop", OSE_SYMTAB_VALUE(ose_builtin_unpackDrop)}
  };

const struct _ose_symtab_rec *
_ose_symtab_lookup (register const char *str, register size_t len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = _ose_symtab_hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = _ose_symtab_wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &_ose_symtab_wordlist[key];
        }
    }
  return 0;
}
#line 185 "ose_symtab.gperf"


void (*ose_symtab_lookup_fn(const char * const str))(ose_bundle)
{
	const struct _ose_symtab_rec *r = _ose_symtab_lookup(str, strlen(str));
	if(r){
		return r->f;
	}else{
		return NULL;
	}
}

#ifdef OSE_SYMTAB_FNSYMS
char *ose_symtab_lookup_fnsym(const char * const str)
{
	const struct _ose_symtab_rec *r = _ose_symtab_lookup(str, strlen(str));
	if(r){
		return r->fnsym;
	}else{
		return NULL;
	}
}
#endif

int ose_symtab_len(void)
{
	return TOTAL_KEYWORDS;
}

char *ose_symtab_getNthSym(int n)
{
	int c = 0, i = 0;
	//for(int i = MIN_HASH_VALUE; i < MAX_HASH_VALUE; i++){
	while(c < TOTAL_KEYWORDS){
		char *name = _ose_symtab_wordlist[i++].name;
		if(strlen(name) >= MIN_WORD_LENGTH){
			if(c == n){
				return name;
			}
			c++;
		}
	}
	return NULL;
}