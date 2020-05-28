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
#include "ose.h"
#include "ose_stackops.h"
#include "ose_builtins.h"
#line 34 "ose_symtab.gperf"
struct _ose_symtab_rec { char *name; void (*f)(ose_bundle);};

#define TOTAL_KEYWORDS 107
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 23
#define MIN_HASH_VALUE 3
#define MAX_HASH_VALUE 217
/* maximum key range = 215, duplicates = 0 */

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
  static const unsigned char asso_values[] =
    {
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218,   0,   5, 218, 218,
      218,  50,  10, 218, 218, 218,   0, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218,  30,  15,  80,
        5,   5,  25,  90, 105,   0,  85,   5,   5,   0,
       50,  20,  30,  30,  45,   0,   0,  70,  90,   0,
      218,   0, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218, 218, 218, 218,
      218, 218, 218, 218, 218, 218, 218
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
    {""}, {""}, {""},
#line 136 "ose_symtab.gperf"
    {"/lt", ose_builtin_lt},
    {""}, {""}, {""}, {""},
#line 90 "ose_symtab.gperf"
    {"/size/tt", ose_builtin_sizeTT},
#line 135 "ose_symtab.gperf"
    {"/lte", ose_builtin_lte},
#line 86 "ose_symtab.gperf"
    {"/size/item", ose_builtin_sizeItem},
    {""},
#line 89 "ose_symtab.gperf"
    {"/sizes/items", ose_builtin_sizesItems},
#line 114 "ose_symtab.gperf"
    {"/swap/bytes/8", ose_builtin_swap8Bytes},
    {""},
#line 85 "ose_symtab.gperf"
    {"/size/elem", ose_builtin_sizeElem},
    {""},
#line 88 "ose_symtab.gperf"
    {"/sizes/elems", ose_builtin_sizesElems},
#line 84 "ose_symtab.gperf"
    {"/size/address", ose_builtin_sizeAddress},
#line 83 "ose_symtab.gperf"
    {"/lengths/items", ose_builtin_lengthsItems},
#line 81 "ose_symtab.gperf"
    {"/length/tt", ose_builtin_lengthTT},
    {""},
#line 82 "ose_symtab.gperf"
    {"/length/item", ose_builtin_lengthItem},
#line 113 "ose_symtab.gperf"
    {"/swap/bytes/4", ose_builtin_swap4Bytes},
#line 54 "ose_symtab.gperf"
    {"/rot", ose_builtin_rot},
#line 80 "ose_symtab.gperf"
    {"/length/address", ose_builtin_lengthAddress},
    {""}, {""},
#line 98 "ose_symtab.gperf"
    {"/address", ose_builtin_copyAddressToString},
#line 128 "ose_symtab.gperf"
    {"/add", ose_builtin_add},
#line 161 "ose_symtab.gperf"
    {"/replace/elem/indest", ose_builtin_replaceElemInDest},
    {""},
#line 108 "ose_symtab.gperf"
    {"/string/toaddress/move", ose_builtin_moveStringToAddress},
#line 157 "ose_symtab.gperf"
    {"/dotimes", ose_builtin_dotimes},
    {""},
#line 55 "ose_symtab.gperf"
    {"/swap", ose_builtin_swap},
    {""},
#line 79 "ose_symtab.gperf"
    {"/count/items", ose_builtin_countItems},
#line 107 "ose_symtab.gperf"
    {"/join/strings", ose_builtin_joinStrings},
    {""},
#line 155 "ose_symtab.gperf"
    {"/else", ose_builtin_else},
#line 106 "ose_symtab.gperf"
    {"/join/blobs", ose_builtin_joinBlobs},
#line 78 "ose_symtab.gperf"
    {"/count/elems", ose_builtin_countElems},
#line 96 "ose_symtab.gperf"
    {"/concat/blobs", ose_builtin_concatenateBlobs},
    {""},
#line 97 "ose_symtab.gperf"
    {"/concat/strings", ose_builtin_concatenateStrings},
    {""},
#line 160 "ose_symtab.gperf"
    {"/copy/elem/todest", ose_builtin_copyElemToDest},
#line 87 "ose_symtab.gperf"
    {"/size/payload", ose_builtin_sizePayload},
    {""},
#line 46 "ose_symtab.gperf"
    {"/-rot", ose_builtin_notrot},
#line 102 "ose_symtab.gperf"
    {"/decat/blob", ose_builtin_decatenateBlob},
#line 52 "ose_symtab.gperf"
    {"/roll/bottom", ose_builtin_rollBottom},
#line 112 "ose_symtab.gperf"
    {"/split/string/fromstart", ose_builtin_splitStringFromStart},
#line 45 "ose_symtab.gperf"
    {"/nip", ose_builtin_nip},
    {""},
#line 111 "ose_symtab.gperf"
    {"/split/string/fromend", ose_builtin_splitStringFromEnd},
#line 100 "ose_symtab.gperf"
    {"/string/toaddress/swap", ose_builtin_swapStringToAddress},
    {""},
#line 64 "ose_symtab.gperf"
    {"/clear/payload", ose_builtin_clearPayload},
    {""},
#line 63 "ose_symtab.gperf"
    {"/clear", ose_builtin_clear},
#line 49 "ose_symtab.gperf"
    {"/pick/bottom", ose_builtin_pickBottom},
#line 115 "ose_symtab.gperf"
    {"/swap/bytes/n", ose_builtin_swapNBytes},
#line 134 "ose_symtab.gperf"
    {"/eql", ose_builtin_eql},
    {""},
#line 110 "ose_symtab.gperf"
    {"/split/blob/fromstart", ose_builtin_splitBlobFromStart},
    {""},
#line 66 "ose_symtab.gperf"
    {"/pop/all", ose_builtin_popAll},
#line 109 "ose_symtab.gperf"
    {"/split/blob/fromend", ose_builtin_splitBlobFromEnd},
    {""},
#line 166 "ose_symtab.gperf"
    {"/parse", ose_builtin_parse},
    {""},
#line 101 "ose_symtab.gperf"
    {"/tt", ose_builtin_copyTTToBlob},
#line 137 "ose_symtab.gperf"
    {"/and", ose_builtin_and},
#line 68 "ose_symtab.gperf"
    {"/pop/all/bundle", ose_builtin_popAllBundle},
    {""},
#line 104 "ose_symtab.gperf"
    {"/elem/toblob", ose_builtin_elemToBlob},
#line 99 "ose_symtab.gperf"
    {"/payload", ose_builtin_copyPayloadToBlob},
#line 132 "ose_symtab.gperf"
    {"/mod", ose_builtin_mod},
#line 69 "ose_symtab.gperf"
    {"/pop/all/drop/bundle", ose_builtin_popAllDropBundle},
    {""}, {""}, {""},
#line 65 "ose_symtab.gperf"
    {"/pop", ose_builtin_pop},
#line 43 "ose_symtab.gperf"
    {"/drop", ose_builtin_drop},
#line 42 "ose_symtab.gperf"
    {"/2swap", ose_builtin_2swap},
#line 158 "ose_symtab.gperf"
    {"/end/dotimes", ose_builtin_end_dotimes},
    {""},
#line 129 "ose_symtab.gperf"
    {"/sub", ose_builtin_sub},
#line 40 "ose_symtab.gperf"
    {"/2dup", ose_builtin_2dup},
#line 39 "ose_symtab.gperf"
    {"/2drop", ose_builtin_2drop},
#line 159 "ose_symtab.gperf"
    {"/move/elem/todest", ose_builtin_moveElemToDest},
    {""}, {""}, {""}, {""}, {""},
#line 67 "ose_symtab.gperf"
    {"/pop/all/drop", ose_builtin_popAllDrop},
#line 131 "ose_symtab.gperf"
    {"/div", ose_builtin_div},
    {""}, {""},
#line 145 "ose_symtab.gperf"
    {"/is/type/int", ose_builtin_isIntegerType},
#line 148 "ose_symtab.gperf"
    {"/is/type/unit", ose_builtin_isUnitType},
#line 146 "ose_symtab.gperf"
    {"/is/type/float", ose_builtin_isFloatType},
    {""}, {""},
#line 156 "ose_symtab.gperf"
    {"/end/if", ose_builtin_end_if},
#line 149 "ose_symtab.gperf"
    {"/is/type/bool", ose_builtin_isBoolType},
#line 44 "ose_symtab.gperf"
    {"/dup", ose_builtin_dup},
#line 163 "ose_symtab.gperf"
    {"/end/quote", ose_builtin_end_quote},
    {""}, {""},
#line 103 "ose_symtab.gperf"
    {"/decat/string", ose_builtin_decatenateString},
    {""},
#line 164 "ose_symtab.gperf"
    {"/clear/env", ose_builtin_clearEnv},
    {""},
#line 94 "ose_symtab.gperf"
    {"/blob/toelem", ose_builtin_blobToElem},
    {""},
#line 133 "ose_symtab.gperf"
    {"/neg", ose_builtin_neg},
    {""},
#line 41 "ose_symtab.gperf"
    {"/2over", ose_builtin_2over},
#line 95 "ose_symtab.gperf"
    {"/blob/totype", ose_builtin_blobToType},
#line 138 "ose_symtab.gperf"
    {"/or", ose_builtin_or},
    {""},
#line 153 "ose_symtab.gperf"
    {"/eval", ose_builtin_eval},
#line 162 "ose_symtab.gperf"
    {"/quote", ose_builtin_quote},
    {""}, {""},
#line 130 "ose_symtab.gperf"
    {"/mul", ose_builtin_mul},
#line 124 "ose_symtab.gperf"
    {"/push/blob", ose_builtin_makeBlob},
    {""},
#line 105 "ose_symtab.gperf"
    {"/item/toblob", ose_builtin_itemToBlob},
    {""},
#line 51 "ose_symtab.gperf"
    {"/roll/jth", ose_builtin_roll},
    {""}, {""}, {""},
#line 154 "ose_symtab.gperf"
    {"/if", ose_builtin_if},
    {""}, {""}, {""},
#line 119 "ose_symtab.gperf"
    {"/pmatch", ose_builtin_pmatch},
    {""},
#line 48 "ose_symtab.gperf"
    {"/pick/jth", ose_builtin_pick},
#line 116 "ose_symtab.gperf"
    {"/trim/blob", ose_builtin_trimBlob},
    {""}, {""}, {""}, {""},
#line 56 "ose_symtab.gperf"
    {"/tuck", ose_builtin_tuck},
    {""},
#line 71 "ose_symtab.gperf"
    {"/unpack", ose_builtin_unpack},
    {""},
#line 143 "ose_symtab.gperf"
    {"/is/type/known", ose_builtin_isKnownTypetag},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 73 "ose_symtab.gperf"
    {"/unpack/bundle", ose_builtin_unpackBundle},
    {""},
#line 53 "ose_symtab.gperf"
    {"/roll/match", ose_builtin_rollMatch},
#line 123 "ose_symtab.gperf"
    {"/make/bundle", ose_builtin_pushBundle},
    {""},
#line 74 "ose_symtab.gperf"
    {"/unpack/drop/bundle", ose_builtin_unpackDropBundle},
#line 47 "ose_symtab.gperf"
    {"/over", ose_builtin_over},
#line 60 "ose_symtab.gperf"
    {"/bundle/all", ose_builtin_bundleAll},
    {""},
#line 61 "ose_symtab.gperf"
    {"/bundle/frombottom", ose_builtin_bundleFromBottom},
#line 165 "ose_symtab.gperf"
    {"/copy/env", ose_builtin_copyEnv},
    {""},
#line 50 "ose_symtab.gperf"
    {"/pick/match", ose_builtin_pickMatch},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 147 "ose_symtab.gperf"
    {"/is/type/numeric", ose_builtin_isNumericType},
#line 72 "ose_symtab.gperf"
    {"/unpack/drop", ose_builtin_unpackDrop},
    {""}, {""},
#line 142 "ose_symtab.gperf"
    {"/is/addresschar", ose_builtin_isAddressChar},
#line 118 "ose_symtab.gperf"
    {"/match", ose_builtin_match},
    {""}, {""}, {""},
#line 144 "ose_symtab.gperf"
    {"/is/type/string", ose_builtin_isStringType},
    {""}, {""}, {""}, {""},
#line 62 "ose_symtab.gperf"
    {"/bundle/fromtop", ose_builtin_bundleFromTop},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
#line 70 "ose_symtab.gperf"
    {"/push", ose_builtin_push},
    {""}, {""}, {""}, {""}, {""}, {""},
#line 117 "ose_symtab.gperf"
    {"/trim/string", ose_builtin_trimString}
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
#line 168 "ose_symtab.gperf"


void (*ose_symtab_lookup(char *str))(ose_bundle)
{
	const struct _ose_symtab_rec *r = _ose_symtab_lookup(str, strlen(str));
	if(r){
		return r->f;
	}else{
		return NULL;
	}
}

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