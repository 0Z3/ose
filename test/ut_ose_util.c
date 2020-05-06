#include "common.h"
#include "ut_common.h"

void ut_ose_pnbytes(void)
{
	UNIT_TEST(ose_pnbytes(-1), ASSERTION_FAILED, "expect failed assertion:");
	UNIT_TEST(ose_pnbytes(0), 4, "padding");
	UNIT_TEST(ose_pnbytes(1), 4, "padding");
	UNIT_TEST(ose_pnbytes(2), 4, "padding");
	UNIT_TEST(ose_pnbytes(3), 4, "padding");
	UNIT_TEST(ose_pnbytes(4), 8, "padding");
	UNIT_TEST(ose_pnbytes(5), 8, "padding");
	UNIT_TEST(ose_pnbytes(6), 8, "padding");
	UNIT_TEST(ose_pnbytes(7), 8, "padding");
	UNIT_TEST(ose_pnbytes(8), 12, "padding");
	UNIT_TEST(ose_pnbytes(9), 12, "padding");
	UNIT_TEST(ose_pnbytes(10), 12, "padding");
	UNIT_TEST(ose_pnbytes(11), 12, "padding");
	UNIT_TEST(ose_pnbytes(12), 16, "padding");
}

void ut_ose_pstrlen(void)
{
	UNIT_TEST(ose_pstrlen(NULL), ASSERTION_FAILED, "expect failed assertion:");
	UNIT_TEST(ose_pstrlen(""), 4, "padding");
	UNIT_TEST(ose_pstrlen("a"), 4, "padding");
	UNIT_TEST(ose_pstrlen("ab"), 4, "padding");
	UNIT_TEST(ose_pstrlen("abc"), 4, "padding");
	UNIT_TEST(ose_pstrlen("abcd"), 8, "padding");
	UNIT_TEST(ose_pstrlen("abcde"), 8, "padding");
	UNIT_TEST(ose_pstrlen("abcdef"), 8, "padding");
	UNIT_TEST(ose_pstrlen("abcdefg"), 8, "padding");
	UNIT_TEST(ose_pstrlen("abcdefgh"), 12, "padding");
	UNIT_TEST(ose_pstrlen("abcdefghi"), 12, "padding");
	UNIT_TEST(ose_pstrlen("abcdefghij"), 12, "padding");
	UNIT_TEST(ose_pstrlen("abcdefghijk"), 12, "padding");
	UNIT_TEST(ose_pstrlen("abcdefghijkl"), 16, "padding");
}

void ut_ose_isAddressChar(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isAddressChar((char)i),
				  OSETT_FALSE,
				  "i < 32 : char is nonprintable");
		}else if(i > 127){
			UNIT_TEST(ose_isAddressChar((char)i),
				  OSETT_FALSE,
				  "i > 127 : char is nonprintable");
		}else{
			switch(i){
			case ' ':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "reserved char : SPC");
				break;
			case '#':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "reserved char : #");
				break;
			case '*':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "pattern char : *");
				break;
			case ',':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "reserved char : ,");
				break;
		        case '/':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_TRUE,
					  "address container separator : /");
				break;
			case '?':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "pattern char : ?");
				break;
			case '[':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "pattern char : [");
				break;
			case ']':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "pattern char : ]");
				break;
			case '{':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "pattern char : {");
				break;
			case '}':
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_FALSE,
					  "}");
				break;
			default:
				UNIT_TEST(ose_isAddressChar((char)i),
					  OSETT_TRUE,
					  "valid address char");
				break;
			}
		}
	}
}

void ut_ose_isKnownTypetag(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isKnownTypetag((char)i),
				  OSETT_FALSE,
				  "i < 32");
		}else if(i > 127){
			UNIT_TEST(ose_isKnownTypetag((char)i),
				  OSETT_FALSE,
				  "i > 127");
		}else{
			switch(i){
			case OSETT_ID:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "int32");
				break;
			case OSETT_INT32:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "int32");
				break;
			case OSETT_FLOAT:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "float");
				break;
			case OSETT_STRING:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "string");
				break;
			case OSETT_BLOB:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "blob");
				break;
			case OSETT_DOUBLE:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "double");
				break;
			case OSETT_SYMBOL:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "symbol");
				break;
			case OSETT_INT8:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "int8");
				break;
			case OSETT_UINT8:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "uint8");
				break;
			case OSETT_UINT32:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "uint32");
				break;
			case OSETT_INT64:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "int64");
				break;
			case OSETT_UINT64:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "uint64");
				break;
			case OSETT_TIMETAG:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "timetag");
				break;
			case OSETT_TRUE:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "true");
				break;
			case OSETT_FALSE:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "false");
				break;
			case OSETT_NULL:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "null");
				break;
			case OSETT_INFINITUM:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_TRUE,
					  "infinitum");
				break;
			default:
				UNIT_TEST(ose_isKnownTypetag((char)i),
					  OSETT_FALSE,
					  "unknown type");
				break;
			}
		}
	}
}

void ut_ose_isStringType(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isStringType((char)i),
				  OSETT_FALSE,
				  "i < 32");
		}else if(i > 127){
			UNIT_TEST(ose_isStringType((char)i),
				  OSETT_FALSE,
				  "i > 127");
		}else{
			switch(i){
			case OSETT_STRING:
				UNIT_TEST(ose_isStringType((char)i),
					  OSETT_TRUE,
					  "string");
				break;
			case OSETT_SYMBOL:
				UNIT_TEST(ose_isStringType((char)i),
					  OSETT_TRUE,
					  "symbol");
				break;
			default:
				UNIT_TEST(ose_isStringType((char)i),
					  OSETT_FALSE,
					  "non string type");
				break;
			}
		}
	}
}

void ut_ose_isIntegerType(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isIntegerType((char)i),
				  OSETT_FALSE,
				  "i < 32");
		}else if(i > 127){
			UNIT_TEST(ose_isIntegerType((char)i),
				  OSETT_FALSE,
				  "i > 127");
		}else{
			switch(i){
			case OSETT_INT32:
				UNIT_TEST(ose_isIntegerType((char)i),
					  OSETT_TRUE,
					  "int32");
				break;
			case OSETT_INT8:
				UNIT_TEST(ose_isIntegerType((char)i),
					  OSETT_TRUE,
					  "int8");
				break;
			case OSETT_UINT8:
				UNIT_TEST(ose_isIntegerType((char)i),
					  OSETT_TRUE,
					  "uint8");
				break;
			case OSETT_UINT32:
				UNIT_TEST(ose_isIntegerType((char)i),
					  OSETT_TRUE,
					  "uint32");
				break;
			case OSETT_INT64:
				UNIT_TEST(ose_isIntegerType((char)i),
					  OSETT_TRUE,
					  "int64");
				break;
			case OSETT_UINT64:
				UNIT_TEST(ose_isIntegerType((char)i),
					  OSETT_TRUE,
					  "uint64");
				break;
			default:
				UNIT_TEST(ose_isIntegerType((char)i),
					  OSETT_FALSE,
					  "non integral type");
				break;
			}
		}
	}
}

void ut_ose_isFloatType(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isFloatType((char)i),
				  OSETT_FALSE,
				  "i < 32");
		}else if(i > 127){
			UNIT_TEST(ose_isFloatType((char)i),
				  OSETT_FALSE,
				  "i > 127");
		}else{
			switch(i){
			case OSETT_FLOAT:
				UNIT_TEST(ose_isFloatType((char)i),
					  OSETT_TRUE,
					  "float");
				break;
			case OSETT_DOUBLE:
				UNIT_TEST(ose_isFloatType((char)i),
					  OSETT_TRUE,
					  "double");
				break;
			default:
				UNIT_TEST(ose_isFloatType((char)i),
					  OSETT_FALSE,
					  "non floating point type");
				break;
			}
		}
	}
}

void ut_ose_isNumericType(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isNumericType((char)i),
				  OSETT_FALSE,
				  "i < 32");
		}else if(i > 127){
			UNIT_TEST(ose_isNumericType((char)i),
				  OSETT_FALSE,
				  "i > 127");
		}else{
			switch(i){
			case OSETT_INT32:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "int32");
				break;
			case OSETT_FLOAT:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "float");
				break;
			case OSETT_DOUBLE:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "double");
				break;
			case OSETT_INT8:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "int8");
				break;
			case OSETT_UINT8:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "uint8");
				break;
			case OSETT_UINT32:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "uint32");
				break;
			case OSETT_INT64:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "int64");
				break;
			case OSETT_UINT64:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "uint64");
				break;
			case OSETT_TIMETAG:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_TRUE,
					  "timetag");
				break;
			default:
				UNIT_TEST(ose_isNumericType((char)i),
					  OSETT_FALSE,
					  "non numeric type");
				break;
			}
		}
	}
}

void ut_ose_isUnitType(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isUnitType((char)i),
				  OSETT_FALSE,
				  "i < 32");
		}else if(i > 127){
			UNIT_TEST(ose_isUnitType((char)i),
				  OSETT_FALSE,
				  "i > 127");
		}else{
			switch(i){
			case OSETT_TRUE:
				UNIT_TEST(ose_isUnitType((char)i),
					  OSETT_TRUE,
					  "true");
				break;
			case OSETT_FALSE:
				UNIT_TEST(ose_isUnitType((char)i),
					  OSETT_TRUE,
					  "false");
				break;
			case OSETT_NULL:
				UNIT_TEST(ose_isUnitType((char)i),
					  OSETT_TRUE,
					  "null");
				break;
			case OSETT_INFINITUM:
				UNIT_TEST(ose_isUnitType((char)i),
					  OSETT_TRUE,
					  "infinitum");
				break;
			default:
				UNIT_TEST(ose_isUnitType((char)i),
					  OSETT_FALSE,
					  "non unit type");
				break;
			}
		}
	}
}

void ut_ose_isBoolType(void)
{
	for(int i = 0; i < 256; i++){
		if(i < 32){
			UNIT_TEST(ose_isBoolType((char)i),
				  OSETT_FALSE,
				  "i < 32");
		}else if(i > 127){
			UNIT_TEST(ose_isBoolType((char)i),
				  OSETT_FALSE,
				  "i > 127");
		}else{
			switch(i){
			case OSETT_TRUE:
				UNIT_TEST(ose_isBoolType((char)i),
					  OSETT_TRUE,
					  "true");
				break;
			case OSETT_FALSE:
				UNIT_TEST(ose_isBoolType((char)i),
					  OSETT_TRUE,
					  "false");
				break;
			default:
				UNIT_TEST(ose_isBoolType((char)i),
					  OSETT_FALSE,
					  "non bool type");
				break;
			}
		}
	}
}

void ut_ose_isBundle(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_isBundle(bundle),
			      OSETT_FALSE,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 A2 Ti Di,
			      ose_isBundle(bundle),
			      OSETT_FALSE,
			      "naked message");
	UNIT_TEST_WITH_BUNDLE("\0\0\0\0" S36 H S16 A2 Ti Di,
			      ose_isBundle(bundle),
			      OSETT_FALSE,
			      "bundle element");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_isBundle(bundle),
			      OSETT_TRUE,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S36 "#bnudle\0\0\0\0\0\0\0\0\0" S16 A2 Ti Di,
			      ose_isBundle(bundle),
			      OSETT_FALSE,
			      "my favorite typo");
	UNIT_TEST_WITH_BUNDLE(S36 "bundle\0\0\0\0\0\0\0\0\0\0" S16 A2 Ti Di,
			      ose_isBundle(bundle),
			      OSETT_FALSE,
			      "my other favorite typo");
	UNIT_TEST_WITH_BUNDLE("\xFF\xFF\xFF\xFF" H S16 A2 Ti Di,
			      ose_isBundle(bundle),
			      OSETT_TRUE,
			      "negative size "
			      "(this function doesn't check the size)");
}

void ut_ose_bundleIsEmpty(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_bundleIsEmpty(bundle),
			       ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 A2 Ti Di,
			      ose_bundleIsEmpty(bundle),
			       ASSERTION_FAILED,
			      "naked message");
	UNIT_TEST_WITH_BUNDLE("\0\0\0\0" S36 H S16 A2 Ti Di,
			      ose_bundleIsEmpty(bundle),
			       ASSERTION_FAILED,
			      "bundle element");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_bundleIsEmpty(bundle),
			      OSETT_TRUE,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S36 "#bnudle\0\0\0\0\0\0\0\0\0" S16 A2 Ti Di,
			      ose_bundleIsEmpty(bundle),
			       ASSERTION_FAILED,
			      "my favorite typo");
	UNIT_TEST_WITH_BUNDLE(S36 "bundle\0\0\0\0\0\0\0\0\0\0" S16 A2 Ti Di,
			      ose_bundleIsEmpty(bundle),
			       ASSERTION_FAILED,
			      "my other favorite typo");
	UNIT_TEST_WITH_BUNDLE("\xFF\xFF\xFF\xFF" H S16 A2 Ti Di,
			      ose_bundleIsEmpty(bundle),
			       ASSERTION_FAILED,
			      "negative size");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Ti Di,
			      ose_bundleIsEmpty(bundle),
			      OSETT_FALSE,
			      "non empty bundle");
}

void ut_ose_getBundleElemCount(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_getBundleElemCount(bundle),
			       ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE("\xFF\xFF\xFF\xFF" H S12 A2 Ti Di,
			      ose_getBundleElemCount(bundle),
			       ASSERTION_FAILED,
			      "negative size");
	UNIT_TEST_WITH_BUNDLE("\0\0\0\0" S32 H S12 A2 Ti Di,
			      ose_getBundleElemCount(bundle),
			       ASSERTION_FAILED,
			      "bundle elem");
	UNIT_TEST_WITH_BUNDLE(S12 A2 Ti Di,
			      ose_getBundleElemCount(bundle),
			       ASSERTION_FAILED,
			      "naked message");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_getBundleElemCount(bundle),
			      0,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      ose_getBundleElemCount(bundle),
			      1,
			      "one message");
	UNIT_TEST_WITH_BUNDLE(S52 H S12 A2 Ti Di S16 A1 Ts Ds4,
			      ose_getBundleElemCount(bundle),
			      2,
			      "two messages");
	UNIT_TEST_WITH_BUNDLE(S52 H "\xFF\xFF\xFF\xFF" A2 Ti Di S16 A1 Ts Ds4,
			      ose_getBundleElemCount(bundle),
			       ASSERTION_FAILED,
			      "message with negative size");
}

void ut_ose_getBundleElemElemCount(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_getBundleElemElemCount(bundle, 20),
			       ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE("\xFF\xFF\xFF\xFF" H S12 A2 Ti Di,
			      ose_getBundleElemElemCount(bundle, 20),
			       ASSERTION_FAILED,
			      "negative size");
	UNIT_TEST_WITH_BUNDLE("\0\0\0\0" S32 H S12 A2 Ti Di,
			      ose_getBundleElemElemCount(bundle, 20),
			       ASSERTION_FAILED,
			      "bundle elem");
	UNIT_TEST_WITH_BUNDLE(S12 A2 Ti Di,
			      ose_getBundleElemElemCount(bundle, 0),
			       ASSERTION_FAILED,
			      "naked message");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_getBundleElemElemCount(bundle, 16),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S28 H S8 A2 T0,
			      ose_getBundleElemElemCount(bundle, 16),
			      0,
			      "one message, zero items");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      ose_getBundleElemElemCount(bundle, 16),
			      1,
			      "one message, one item");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tii Di Di,
			      ose_getBundleElemElemCount(bundle, 16),
			      2,
			      "one message, two items");
	UNIT_TEST_WITH_BUNDLE(S52 H "\xFF\xFF\xFF\xFF" A2 Ti Di S16 A1 Ts Ds4,
			      ose_getBundleElemElemCount(bundle, 16),
			      ASSERTION_FAILED,
			      "first message has a negative size -- "
			      "fails due to a call to "
			      "ose_getBundleElemAddressOffset.");
			      //"this passes, as a size check of this message "
			      //"isn't necessary here.");
	UNIT_TEST_WITH_BUNDLE(S52 H "\xFF\xFF\xFF\xFF" A2 Ti Di S16 A1 Ts Ds4,
			      ose_getBundleElemElemCount(bundle, 32),
			       1,
			      "first message has a negative size -- "
			      "this passes, as a size check of this message "
			      "isn't necessary here.");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 H,
			      ose_getBundleElemElemCount(bundle, 16),
			      0,
			      "one bundle, zero items");
	UNIT_TEST_WITH_BUNDLE(S52 H S32 H S12 A2 Ti Di,
			      ose_getBundleElemElemCount(bundle, 16),
			      1,
			      "one bundle, one item");
	UNIT_TEST_WITH_BUNDLE(S72 H S52 H S12 A2 Ti Di S16 H,
			      ose_getBundleElemElemCount(bundle, 16),
			      2,
			      "one bundle, two items");
	// no need to check situations already covered by ose_getBundleElemCount
}

void ut_ose_bundleHasAtLeastNElems(void)
{
	UNIT_TEST_WITH_BUNDLE(S52 H S32 H S12 A2 Ti Di,
			      ose_bundleHasAtLeastNElems(bundle, -3),
			      ASSERTION_FAILED,
			      "negative n");
	UNIT_TEST_WITH_BUNDLE(S52 H S32 H S12 A2 Ti Di,
			      ose_bundleHasAtLeastNElems(bundle, 0),
			      ASSERTION_FAILED,
			      "zero n");
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_bundleHasAtLeastNElems(bundle, 1),
			      ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_bundleHasAtLeastNElems(bundle, 1),
			      OSETT_FALSE,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A4 Ti Di,
			      ose_bundleHasAtLeastNElems(bundle, 1),
			      OSETT_TRUE,
			      "bundle with one elem");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A4 Ti Di,
			      ose_bundleHasAtLeastNElems(bundle, 2),
			      OSETT_FALSE,
			      "bundle with one elem");
	UNIT_TEST_WITH_BUNDLE(S52 H S16 A4 Ti Di S12 A2 Tf Df,
			      ose_bundleHasAtLeastNElems(bundle, 2),
			      OSETT_TRUE,
			      "bundle with two elems");
	UNIT_TEST_WITH_BUNDLE(S52 H S16 A4 Ti Di S12 A2 Tf Df,
			      ose_bundleHasAtLeastNElems(bundle, 1),
			      OSETT_TRUE,
			      "bundle with two elems");
}

void ut_ose_getBundleElemType(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_getBundleElemType(bundle, 16),
			      ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_getBundleElemType(bundle, 16),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      ose_getBundleElemType(bundle, 16),
			      OSETT_MESSAGE,
			      "message");
	UNIT_TEST_WITH_BUNDLE(S56 H S32 H S12 A2 Ti Di,
			      ose_getBundleElemType(bundle, 16),
			      OSETT_BUNDLE,
			      "bundle");
}

void ut_ose_readByte(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_readByte(bundle, 0),
			       ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readByte(bundle, -4),
			      0,
			      "negative offset");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readByte(bundle, -3),
			      0,
			      "negative offset");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readByte(bundle, -2),
			      0,
			      "negative offset");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readByte(bundle, -1),
			      0x20,
			      "negative offset");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readByte(bundle, 0),
			      '#',
			      "0 offset");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readByte(bundle, 1),
			      'b',
			      "positive offset");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readByte(bundle, 7),
			      '\0',
			      "NULL byte");
}

void ut_ose_writeByte(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_writeByte(bundle, 25, OSETT_FLOAT),
			       ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ti Di S8 A0 T0,
			      (ose_writeByte(bundle, -1, 0x20),
			       memcmp("\0\0\0\x20" H S12 A0 Ti Di S8 A0 T0,
				       ose_getBundlePtr(bundle) - 4,
				       48)),
			      0,
			      "negative offset");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ti Di S8 A0 T0,
			      (ose_writeByte(bundle, 25, OSETT_FLOAT),
			       memcmp(H S12 A0 Tf Di S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "positive offset");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ti Di S8 A0 T0,
			      (ose_writeByte(bundle, 31, 0x66),
			       memcmp(H S12 A0 Ti "\0\0\0\x66" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "positive offset");
}

void ut_ose_readInt32(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_readInt32(bundle, 0),
			       ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readInt32(bundle, -4),
			      32,
			      "negative offset");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readInt32(bundle, 16),
			      12,
			      "size");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Ti Di,
			      ose_readInt32(bundle, 28),
			      32,
			      "data");
}

void ut_ose_writeInt32(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_writeInt32(bundle, 25, OSETT_FLOAT),
			       ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ti Di S8 A0 T0,
			      (ose_writeInt32(bundle, -4, 0x66),
			       memcmp("\0\0\0\x66" H S12 A0 Ti Di S8 A0 T0,
				       ose_getBundlePtr(bundle) - 4,
				       48)),
			      0,
			      "negative offset");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ti Di S8 A0 T0,
			      (ose_writeInt32(bundle, 28, 0x66),
			       memcmp(H S12 A0 Ti "\0\0\0\x66" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "data");
}

void ut_ose_readFloat(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_readFloat(bundle, 0),
			       ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A3 Tf Df,
			      ose_readFloat(bundle, 28),
			      544,
			      "data");
}

void ut_ose_writeFloat(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_writeFloat(bundle, 25, OSETT_FLOAT),
			       ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Tf Di S8 A0 T0,
			      (ose_writeFloat(bundle, 28, 544.0),
			       memcmp(H S12 A0 Tf Df S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "data");
}

void ut_ose_readString(void)
{
	// 1
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_readString(bundle, 28), 0),
			       ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds0,
			      strcmp(ose_readString(bundle, 28), Ds0),
			      0,
			      "empty string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds1,
			      strcmp(ose_readString(bundle, 28), Ds1),
			      0,
			      "one char string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds4,
			      strcmp(ose_readString(bundle, 28), Ds4),
			      0,
			      "four char string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds5,
			      strcmp(ose_readString(bundle, 28), Ds5),
			      0,
			      "five char string");

	// establish that we're not fooled by an empty string
	// 6
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds0 Ds0,
			      strcmp(ose_readString(bundle, 28), Ds0),
			      0,
			      "two zero char strings");
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds0 Ds0,
			      strcmp(ose_readString(bundle, 32), Ds0),
			      0,
			      "two zero char strings");
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds1 Ds0,
			      strcmp(ose_readString(bundle, 28), Ds1),
			      0,
			      "one char and zero char strings");
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds1 Ds0,
			      strcmp(ose_readString(bundle, 32), Ds0),
			      0,
			      "one char and zero char strings");
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds0 Ds1,
			      strcmp(ose_readString(bundle, 28), Ds0),
			      0,
			      "zero char and one char strings");
	// 11
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds0 Ds1,
			      strcmp(ose_readString(bundle, 32), Ds1),
			      0,
			      "zero char and one char strings");
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds1 Ds1,
			      strcmp(ose_readString(bundle, 28), Ds1),
			      0,
			      "two one char strings");
	UNIT_TEST_WITH_BUNDLE(S36 H S12 A0 Tss Ds1 Ds1,
			      strcmp(ose_readString(bundle, 32), Ds1),
			      0,
			      "two one char strings");

	// not fooled by a string that ends on a 4-byte boundary either
	UNIT_TEST_WITH_BUNDLE(S40 H S12 A0 Tss Ds4 Ds1,
			      strcmp(ose_readString(bundle, 28), Ds4),
			      0,
			      "four byte string");
	UNIT_TEST_WITH_BUNDLE(S40 H S12 A0 Tss Ds4 Ds1,
			      strcmp(ose_readString(bundle, 36), Ds1),
			      0,
			      "four byte string");
	UNIT_TEST_WITH_BUNDLE(S40 H S12 A0 Tss Ds5 Ds1,
			      strcmp(ose_readString(bundle, 28), Ds5),
			      0,
			      "five byte string");
	UNIT_TEST_WITH_BUNDLE(S40 H S12 A0 Tss Ds5 Ds1,
			      strcmp(ose_readString(bundle, 36), Ds1),
			      0,
			      "five byte string");
}

void ut_ose_getStringLen(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_getStringLen(bundle, 28),
			      ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds0,
			      ose_getStringLen(bundle, 28),
			      0,
			      "0 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds1,
			      ose_getStringLen(bundle, 28),
			      1,
			      "1 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds2,
			      ose_getStringLen(bundle, 28),
			      2,
			      "2 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds3,
			      ose_getStringLen(bundle, 28),
			      3,
			      "3 length string");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Ts Ds4,
			      ose_getStringLen(bundle, 28),
			      4,
			      "4 length string");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Ts Ds5,
			      ose_getStringLen(bundle, 28),
			      5,
			      "5 length string");
}

void ut_ose_getPaddedStringLen(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_getPaddedStringLen(bundle, 28),
			      ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds0,
			      ose_getPaddedStringLen(bundle, 28),
			      4,
			      "0 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds1,
			      ose_getPaddedStringLen(bundle, 28),
			      4,
			      "1 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds2,
			      ose_getPaddedStringLen(bundle, 28),
			      4,
			      "2 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ts Ds3,
			      ose_getPaddedStringLen(bundle, 28),
			      4,
			      "3 length string");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Ts Ds4,
			      ose_getPaddedStringLen(bundle, 28),
			      8,
			      "4 length string");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Ts Ds5,
			      ose_getPaddedStringLen(bundle, 28),
			      8,
			      "5 length string");
}

void ut_ose_writeStringWithLen(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_writeStringWithLen(bundle,
						     28,
						     "foo",
						     3,
						     4),
			      ASSERTION_FAILED,
			      "expect failed assertion : NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds0 S8 A0 T0,
			      ose_writeStringWithLen(bundle,
						     28,
						     NULL,
						     0,
						     4),
			      ASSERTION_FAILED,
			      "expect failed assertion : NULL string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds0 S8 A0 T0,
			      ose_writeStringWithLen(bundle,
						     28,
						     "foo",
						     -1,
						     4),
			      ASSERTION_FAILED,
			      "expect failed assertion : negative length");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds0 S8 A0 T0,
			      ose_writeStringWithLen(bundle,
						     28,
						     "foo",
						     3,
						     0),
			      ASSERTION_FAILED,
			      "expect failed assertion : plen < len");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeStringWithLen(bundle,
						     28,
						     "",
						     0,
						     4),
			       memcmp(H S12 A0 Ts "\0\0\0\0" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "write 0 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeStringWithLen(bundle,
						     28,
						     "b",
						     1,
						     4),
			       memcmp(H S12 A0 Ts "b\0\0\0" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "write 1 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeStringWithLen(bundle,
						     28,
						     "ba",
						     2,
						     4),
			       memcmp(H S12 A0 Ts "ba\0\0" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "write 2 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeStringWithLen(bundle,
						     28,
						     "bar",
						     3,
						     4),
			       memcmp(H S12 A0 Ts "bar\0" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       44)),
			      0,
			      "write 3 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S48 H S16 A0 Ts Ds4 S8 A0 T0,
			      (ose_writeStringWithLen(bundle,
						     28,
						     "barr",
						     4,
						     8),
			       memcmp(H S16 A0 Ts "barr\0\0\0\0" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       48)),
			      0,
			      "write 4 byte string over 4 byte string");
	UNIT_TEST_WITH_BUNDLE(S48 H S16 A0 Ts Ds5 S8 A0 T0,
			      (ose_writeStringWithLen(bundle,
						     28,
						     "barr",
						     4,
						     8),
			       memcmp(H S16 A0 Ts "barr\0\0\0\0" S8 A0 T0,
				       ose_getBundlePtr(bundle),
				       48)),
			      0,
			      "write 4 byte string over 5 byte string");	
}

void ut_ose_writeString(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_writeString(bundle,
					      28,
					      "foo"),
			      ASSERTION_FAILED,
			      "expect failed assertion : NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds0 S8 A0 T0,
			      ose_writeString(bundle,
					      28,
					      NULL),
			      ASSERTION_FAILED,
			      "expect failed assertion : NULL string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeString(bundle,
					       28,
					       ""),
			       memcmp(H S12 A0 Ts "\0\0\0\0" S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      44)),
			      0,
			      "write 0 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeString(bundle,
					       28,
					       "b"),
			       memcmp(H S12 A0 Ts "b\0\0\0" S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      44)),
			      0,
			      "write 1 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeString(bundle,
					       28,
					       "ba"),
			       memcmp(H S12 A0 Ts "ba\0\0" S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      44)),
			      0,
			      "write 2 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Ts Ds3 S8 A0 T0,
			      (ose_writeString(bundle,
					       28,
					       "bar"),
			       memcmp(H S12 A0 Ts "bar\0" S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      44)),
			      0,
			      "write 3 byte string over 3 byte string");
	UNIT_TEST_WITH_BUNDLE(S48 H S16 A0 Ts Ds4 S8 A0 T0,
			      (ose_writeString(bundle,
					       28,
					       "barr"),
			       memcmp(H S16 A0 Ts "barr\0\0\0\0" S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      48)),
			      0,
			      "write 4 byte string over 4 byte string");
	UNIT_TEST_WITH_BUNDLE(S48 H S16 A0 Ts Ds5 S8 A0 T0,
			      (ose_writeString(bundle,
					       28,
					       "barr"),
			       memcmp(H S16 A0 Ts "barr\0\0\0\0" S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      48)),
			      0,
			      "write 4 byte string over 5 byte string");	
}

void ut_ose_readBlob(void)
{
	// 1
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_readBlob(bundle, 28), 0),
			       ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db0,
			      memcmp(ose_readBlob(bundle, 28), Db0, 4),
			      0,
			      "empty blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tb Db1,
			      memcmp(ose_readBlob(bundle, 28), Db1, 8),
			      0,
			      "one char blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tb Db4,
			      memcmp(ose_readBlob(bundle, 28), Db4, 8),
			      0,
			      "four char blob");
	UNIT_TEST_WITH_BUNDLE(S40 H S20 A0 Tb Db5,
			      memcmp(ose_readBlob(bundle, 28), Db5, 12),
			      0,
			      "five char blob");

	// 6
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tbb Db0 Db0,
			      memcmp(ose_readBlob(bundle, 28), Db0, 4),
			      0,
			      "two zero char blobs");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tbb Db0 Db0,
			      memcmp(ose_readBlob(bundle, 32), Db0, 4),
			      0,
			      "two zero char blobs");
	UNIT_TEST_WITH_BUNDLE(S40 H S20 A0 Tbb Db1 Db0,
			      memcmp(ose_readBlob(bundle, 28), Db1, 8),
			      0,
			      "one char and zero char blobs");
	UNIT_TEST_WITH_BUNDLE(S40 H S20 A0 Tbb Db1 Db0,
			      memcmp(ose_readBlob(bundle, 36), Db0, 4),
			      0,
			      "one char and zero char blobs");
	UNIT_TEST_WITH_BUNDLE(S40 H S20 A0 Tbb Db0 Db1,
			      memcmp(ose_readBlob(bundle, 28), Db0, 4),
			      0,
			      "zero char and one char blobs");
	// 11
	UNIT_TEST_WITH_BUNDLE(S40 H S20 A0 Tbb Db0 Db1,
			      memcmp(ose_readBlob(bundle, 32), Db1, 8),
			      0,
			      "zero char and one char blobs");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A0 Tbb Db1 Db1,
			      memcmp(ose_readBlob(bundle, 28), Db1, 8),
			      0,
			      "two one char blobs");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A0 Tbb Db1 Db1,
			      memcmp(ose_readBlob(bundle, 36), Db1, 8),
			      0,
			      "two one char blobs");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A0 Tbb Db4 Db1,
			      memcmp(ose_readBlob(bundle, 28), Db4, 8),
			      0,
			      "four byte blob");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A0 Tbb Db4 Db1,
			      memcmp(ose_readBlob(bundle, 36), Db1, 8),
			      0,
			      "four byte blob");
	// 16
	UNIT_TEST_WITH_BUNDLE(S48 H S28 A0 Tbb Db5 Db1,
			      memcmp(ose_readBlob(bundle, 28), Db5, 12),
			      0,
			      "five byte blob");
	UNIT_TEST_WITH_BUNDLE(S48 H S28 A0 Tbb Db5 Db1,
			      memcmp(ose_readBlob(bundle, 40), Db1, 8),
			      0,
			      "five byte string");
}

void ut_ose_readBlobSize(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_readBlobSize(bundle, 28),
			      ASSERTION_FAILED,
			      "expect failed assertion");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db0,
			      ose_readBlobSize(bundle, 28),
			      0,
			      "0 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db1,
			      ose_readBlobSize(bundle, 28),
			      1,
			      "1 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db2,
			      ose_readBlobSize(bundle, 28),
			      2,
			      "2 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db3,
			      ose_readBlobSize(bundle, 28),
			      3,
			      "3 length blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tb Db4,
			      ose_readBlobSize(bundle, 28),
			      4,
			      "4 length blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tb Db5,
			      ose_readBlobSize(bundle, 28),
			      5,
			      "5 length blob");	
}

void ut_ose_getBlobPaddingForNBytes(void)
{
	UNIT_TEST(ose_getBlobPaddingForNBytes(-1), ASSERTION_FAILED,
		  "expect failed assertion:");
	UNIT_TEST(ose_getBlobPaddingForNBytes(0), 0, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(1), 3, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(2), 2, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(3), 1, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(4), 0, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(5), 3, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(6), 2, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(7), 1, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(8), 0, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(9), 3, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(10), 2, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(11), 1, "padding");
	UNIT_TEST(ose_getBlobPaddingForNBytes(12), 0, "padding");
}

void ut_ose_getPaddedBlobSize(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_getPaddedBlobSize(bundle, 28),
			      ASSERTION_FAILED,
			      "expect failed assertion : NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb "\xFF\xFF\xFF\xFF",
			      ose_getPaddedBlobSize(bundle, 28),
			      ASSERTION_FAILED,
			      "expect failed assertion : negative blob size");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db0,
			      ose_getPaddedBlobSize(bundle, 28),
			      0,
			      "0 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db1,
			      ose_getPaddedBlobSize(bundle, 28),
			      4,
			      "1 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db2,
			      ose_getPaddedBlobSize(bundle, 28),
			      4,
			      "2 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Tb Db3,
			      ose_getPaddedBlobSize(bundle, 28),
			      4,
			      "3 length blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tb Db4,
			      ose_getPaddedBlobSize(bundle, 28),
			      4,
			      "4 length blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tb Db5,
			      ose_getPaddedBlobSize(bundle, 28),
			      8,
			      "5 length blob");	
}

void ut_ose_readBlobPayload(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_readBlobPayload(bundle, 28), 0),
			       ASSERTION_FAILED,
			      "expect failed assertion : NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db0,
			      (ose_readBlobPayload(bundle, 28), 0),
			      ASSERTION_FAILED,
			      "expect failed assertion : zero length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb "\xFF\xFF\xFF\xFF",
			      (ose_readBlobPayload(bundle, 28), 0),
			      ASSERTION_FAILED,
			      "expect failed assertion : negative length blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tb Db1,
			      memcmp(ose_readBlobPayload(bundle, 28),
				     &(Db1[4]),
				     4),
			      0,
			      "one char blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tb Db4,
			      memcmp(ose_readBlobPayload(bundle, 28),
				     &(Db4[4]),
				     4),
			      0,
			      "four char blob");
	UNIT_TEST_WITH_BUNDLE(S40 H S20 A0 Tb Db5,
			      memcmp(ose_readBlobPayload(bundle, 28), &(Db5[4]),
				     8),
			      0,
			      "five char blob");
}

void ut_ose_writeBlob(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_writeBlob(bundle,
					    28,
					    3,
					    "987"),
			      ASSERTION_FAILED,
			      "expect failed assertion : NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tb Db1,
			      ose_writeBlob(bundle,
					    28,
					    8,
					    NULL),
			      ASSERTION_FAILED,
			      "expect failed assertion : NULL blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tb Db1,
			      ose_writeBlob(bundle,
					    28,
					    -4,
					    "987"),
			      ASSERTION_FAILED,
			      "expect failed assertion : negative size");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Tb Db3 S8 A0 T0,
			      (ose_writeBlob(bundle,
					     28,
					     1,
					     &(Db1[4])),
			       memcmp(H S12 A0 Tb Db1 S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      44)),
			      0,
			      "write 1 byte blob over 3 byte blob");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Tb Db3 S8 A0 T0,
			      (ose_writeBlob(bundle,
					     28,
					     2,
					     &(Db2[4])),
			       memcmp(H S12 A0 Tb Db2 S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      44)),
			      0,
			      "write 2 byte blob over 3 byte blob");
	UNIT_TEST_WITH_BUNDLE(S44 H S12 A0 Tb Db3 S8 A0 T0,
			      (ose_writeBlob(bundle,
					     28,
					     3,
					     &(Db3[4])),
			       memcmp(H S12 A0 Tb Db3 S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      44)),
			      0,
			      "write 3 byte blob over 3 byte blob");
	UNIT_TEST_WITH_BUNDLE(S48 H S16 A0 Tb Db4 S8 A0 T0,
			      (ose_writeBlob(bundle,
					     28,
					     4,
					     &(Db4[4])),
			       memcmp(H S16 A0 Tb Db4 S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      48)),
			      0,
			      "write 4 byte blob over 4 byte blob");
	UNIT_TEST_WITH_BUNDLE(S48 H S16 A0 Tb Db5 S8 A0 T0,
			      (ose_writeBlob(bundle,
					     28,
					     5,
					     &(Db5[4])),
			       memcmp(H S16 A0 Tb Db5 S8 A0 T0,
				      ose_getBundlePtr(bundle),
				      48)),
			      0,
			      "write 5 byte blob over 5 byte blob");
}

void ut_ose_readDouble(void)
{
	
}

void ut_ose_writeDouble(void)
{
	
}

void ut_ose_readUInt32(void)
{
	
}

void ut_ose_writeUInt32(void)
{
	
}

void ut_ose_readInt64(void)
{
	
}

void ut_ose_writeInt64(void)
{
	
}

void ut_ose_readUInt64(void)
{
	
}

void ut_ose_writeUInt64(void)
{
	
}

void ut_ose_readTimetag(void)
{
	
}

void ut_ose_writeTimetag(void)
{
	
}

void ut_ose_getLastBundleElemOffset(void)
{
	UNIT_TEST(ose_getLastBundleElemOffset(ose_makeBundle(NULL)),
		  ASSERTION_FAILED,
		  "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_getLastBundleElemOffset(bundle),
			      16,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE("\xFF\xFF\xFF\xFF" H S12 A1 Ti Di S12 A3 Tf Df,
			      ose_getLastBundleElemOffset(bundle),
			      ASSERTION_FAILED,
			      "negative bundle size");
	UNIT_TEST_WITH_BUNDLE(S44 H S64 A1 Ti Di S12 A3 Tf Df,
			      ose_getLastBundleElemOffset(bundle),
			      ASSERTION_FAILED,
			      "wrong size for first message");
	UNIT_TEST_WITH_BUNDLE(S64 H S12 A2 Ts Ds3 S64 A1 Ti Di S12 A3 Tf Df,
			      ose_getLastBundleElemOffset(bundle),
			      ASSERTION_FAILED,
			      "wrong size for second message");
	UNIT_TEST_WITH_BUNDLE(S64 H S12 A2 Ts Ds3 S12 A1 Ti Di S16 A3 Tf Df,
			      ose_getLastBundleElemOffset(bundle),
			      ASSERTION_FAILED,
			      "wrong size for last message");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A1 Ti Di,
			      ose_getLastBundleElemOffset(bundle),
			      16,
			      "one message");
	UNIT_TEST_WITH_BUNDLE(S48 H S12 A1 Ti Di S12 A3 Tf Df,
			      ose_getLastBundleElemOffset(bundle),
			      32,
			      "two messages");
	UNIT_TEST_WITH_BUNDLE(S72 H S12 A1 Ti Di S20 A4 Ts Ds4 S12 A3 Tf Df,
			      ose_getLastBundleElemOffset(bundle),
			      56,
			      "three  messages");
}

void ut_ose_getBundleElemAddressOffset(void)
{
	// this function just adds 4 to its argument, so here we
	// just do some basic checking to make sure the assertions
	// fail when they're supposed to
	UNIT_TEST(ose_getBundleElemAddressOffset(ose_makeBundle(NULL), 16),
		  ASSERTION_FAILED,
		  "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_getBundleElemAddressOffset(bundle, 16),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S60 H S16 A4 Ti Di S20 A4 Ts Ds4,
			      ose_getBundleElemAddressOffset(bundle, 0),
			      ASSERTION_FAILED,
			      "offset less than bundle header size");
	UNIT_TEST_WITH_BUNDLE(S60 H S16 A4 Ti Di S20 A4 Ts Ds4,
			      ose_getBundleElemAddressOffset(bundle, 64),
			      ASSERTION_FAILED,
			      "offset beyond end of bundle");
	UNIT_TEST_WITH_BUNDLE(S64 H S16 A4 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemAddressOffset(bundle, 60),
			      ASSERTION_FAILED,
			      "0 length message");
}

void ut_ose_getBundleElemTTOffset(void)
{
	UNIT_TEST(ose_getBundleElemTTOffset(ose_makeBundle(NULL), 16),
		  ASSERTION_FAILED,
		  "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_getBundleElemTTOffset(bundle, 16),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S60 H S16 A4 Ti Di S20 A4 Ts Ds4,
			      ose_getBundleElemTTOffset(bundle, 0),
			      ASSERTION_FAILED,
			      "offset less than bundle header size");
	UNIT_TEST_WITH_BUNDLE(S60 H S16 A4 Ti Di S20 A4 Ts Ds4,
			      ose_getBundleElemTTOffset(bundle, 64),
			      ASSERTION_FAILED,
			      "offset beyond end of bundle");
	UNIT_TEST_WITH_BUNDLE(S64 H S16 A4 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemTTOffset(bundle, 60),
			      ASSERTION_FAILED,
			      "0 length message");
	UNIT_TEST_WITH_BUNDLE(S60 H S12 A0 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemTTOffset(bundle, 16),
			      24,
			      "0 length address");
	UNIT_TEST_WITH_BUNDLE(S60 H S12 A3 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemTTOffset(bundle, 16),
			      24,
			      "3 length address");
	UNIT_TEST_WITH_BUNDLE(S64 H S16 A4 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemTTOffset(bundle, 16),
			      28,
			      "4 length address");
	UNIT_TEST_WITH_BUNDLE(S84 H S64 H S16 A4 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemTTOffset(bundle, 16),
			      28,
			      "bundle");
}

void ut_ose_getBundleElemPayloadOffset(void)
{
	UNIT_TEST(ose_getBundleElemPayloadOffset(ose_makeBundle(NULL), 16),
		  ASSERTION_FAILED,
		  "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_getBundleElemPayloadOffset(bundle, 16),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S60 H S16 A4 Ti Di S20 A4 Ts Ds4,
			      ose_getBundleElemPayloadOffset(bundle, 0),
			      ASSERTION_FAILED,
			      "offset less than bundle header size");
	UNIT_TEST_WITH_BUNDLE(S60 H S16 A4 Ti Di S20 A4 Ts Ds4,
			      ose_getBundleElemPayloadOffset(bundle, 64),
			      ASSERTION_FAILED,
			      "offset beyond end of bundle");
	UNIT_TEST_WITH_BUNDLE(S64 H S16 A4 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemPayloadOffset(bundle, 60),
			      ASSERTION_FAILED,
			      "0 length message");
	UNIT_TEST_WITH_BUNDLE(S60 H S12 A0 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemPayloadOffset(bundle, 16),
			      28,
			      "0 length address");
	UNIT_TEST_WITH_BUNDLE(S60 H S12 A3 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemPayloadOffset(bundle, 16),
			      28,
			      "3 length address");
	UNIT_TEST_WITH_BUNDLE(S64 H S16 A4 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemPayloadOffset(bundle, 16),
			      32,
			      "4 length address");
	UNIT_TEST_WITH_BUNDLE(S84 H S64 H S16 A4 Ti Di S20 A4 Ts Ds4 S0,
			      ose_getBundleElemPayloadOffset(bundle, 16),
			      36,
			      "bundle");
}

void ut_ose_getFirstOffsetForMatch(void)
{
	
}

void ut_ose_getFirstOffsetForPMatch(void)
{
	
}

void ut_ose_getTypedDatumSize(void)
{
	for(int i = 0; i < 256; i++){
		switch(i){
		case OSETT_ID:
			UNIT_TEST(ose_getTypedDatumSize(i, Di), 0, "id");
			break;
		case OSETT_INT8:
		case OSETT_UINT8:
		case OSETT_INT32:
		case OSETT_UINT32:
		case OSETT_FLOAT:
			UNIT_TEST(ose_getTypedDatumSize(i, Di), 4, "4 byte");
			break;
				case OSETT_STRING:
		case OSETT_SYMBOL:
			UNIT_TEST(ose_getTypedDatumSize(i, NULL),
				  ASSERTION_FAILED, "string NULL pointer");
			UNIT_TEST(ose_getTypedDatumSize(i, Ds0), 4,
				  "string type");
			UNIT_TEST(ose_getTypedDatumSize(i, Ds1), 4,
				  "string type");
			UNIT_TEST(ose_getTypedDatumSize(i, Ds2), 4,
				  "string type");
			UNIT_TEST(ose_getTypedDatumSize(i, Ds3), 4,
				  "string type");
			UNIT_TEST(ose_getTypedDatumSize(i, Ds4), 8,
				  "string type");
			break;
		case OSETT_BLOB: {
			char buf[16];
			char *p = align(buf);
			
			UNIT_TEST(ose_getTypedDatumSize(i, NULL),
				  ASSERTION_FAILED, "blob NULL pointer");
			memcpy(p, Db0, 4);
			UNIT_TEST(ose_getTypedDatumSize(i, p), 4, "blob");
			memcpy(p, Db1, 8);
			UNIT_TEST(ose_getTypedDatumSize(i, p), 8, "blob");
			memcpy(p, Db2, 8);
			UNIT_TEST(ose_getTypedDatumSize(i, p), 8, "blob");
			memcpy(p, Db3, 8);
			UNIT_TEST(ose_getTypedDatumSize(i, p), 8, "blob");
			memcpy(p, Db4, 8);
			UNIT_TEST(ose_getTypedDatumSize(i, p), 8, "blob");
			memcpy(p, Db5, 12);
			UNIT_TEST(ose_getTypedDatumSize(i, p), 12, "blob");
		}
			break;
		case OSETT_INT64:
		case OSETT_UINT64:
		case OSETT_TIMETAG:
		case OSETT_DOUBLE:
			SKIP_UNIT_TEST(ose_getTypedDatumSize(i, Di), 4, "8 byte");
			break;
		case OSETT_TRUE:
		case OSETT_FALSE:
		case OSETT_NULL:
		case OSETT_INFINITUM:
			UNIT_TEST(ose_getTypedDatumSize(i, Di), 0,
				  "unit type (0 bytes)");
			break;
		default:
			UNIT_TEST(ose_getTypedDatumSize(i, Di),
				  ASSERTION_FAILED, "unknown typetag");
			break;
		}
	}
}

void ut_ose_getPayloadItemSize(void)
{
	// we don't need to re-run all the same tests that we ran for
	// ose_getTypedDatumSize. we just need to make sure the assertions
	// fail as expected.
	UNIT_TEST(ose_getPayloadItemSize(ose_makeBundle(NULL),
					 OSETT_INT32, 28),
		  ASSERTION_FAILED,
		  "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      ose_getPayloadItemSize(bundle,
						     OSETT_INT32, 0),
			      ASSERTION_FAILED,
			      "not in the payload");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      ose_getPayloadItemSize(bundle,
						     OSETT_INT32, 64),
			      ASSERTION_FAILED,
			      "beyond end of bundle");
}

void ut_ose_getTypedDatumLength(void)
{
	for(int i = 0; i < 256; i++){
		switch(i){
		case OSETT_ID:
			UNIT_TEST(ose_getTypedDatumLength(i, Di), 0, "id");
			break;
		case OSETT_INT8:
		case OSETT_UINT8:
		case OSETT_INT32:
		case OSETT_UINT32:
		case OSETT_FLOAT:
			UNIT_TEST(ose_getTypedDatumLength(i, Di), 4, "4 byte");
			break;
		case OSETT_STRING:
		case OSETT_SYMBOL:
			UNIT_TEST(ose_getTypedDatumLength(i, NULL),
				  ASSERTION_FAILED, "string NULL pointer");
			UNIT_TEST(ose_getTypedDatumLength(i, Ds0), 0,
				  "string type");
			UNIT_TEST(ose_getTypedDatumLength(i, Ds1), 1,
				  "string type");
			UNIT_TEST(ose_getTypedDatumLength(i, Ds2), 2,
				  "string type");
			UNIT_TEST(ose_getTypedDatumLength(i, Ds3), 3,
				  "string type");
			UNIT_TEST(ose_getTypedDatumLength(i, Ds4), 4,
				  "string type");
			break;
		case OSETT_BLOB: {
			char buf[16];
			char *p = align(buf);
			
			UNIT_TEST(ose_getTypedDatumLength(i, NULL),
				  ASSERTION_FAILED, "blob NULL pointer");
			memcpy(p, Db0, 4);
			UNIT_TEST(ose_getTypedDatumLength(i, p), 0, "blob");
			memcpy(p, Db1, 8);
			UNIT_TEST(ose_getTypedDatumLength(i, p), 1, "blob");
			memcpy(p, Db2, 8);
			UNIT_TEST(ose_getTypedDatumLength(i, p), 2, "blob");
			memcpy(p, Db3, 8);
			UNIT_TEST(ose_getTypedDatumLength(i, p), 3, "blob");
			memcpy(p, Db4, 8);
			UNIT_TEST(ose_getTypedDatumLength(i, p), 4, "blob");
			memcpy(p, Db5, 12);
			UNIT_TEST(ose_getTypedDatumLength(i, p), 5, "blob");
		}
			break;
		case OSETT_INT64:
		case OSETT_UINT64:
		case OSETT_TIMETAG:
		case OSETT_DOUBLE:
			SKIP_UNIT_TEST(ose_getTypedDatumLength(i, Di), 4, "8 byte");
			break;
		case OSETT_TRUE:
		case OSETT_FALSE:
		case OSETT_NULL:
		case OSETT_INFINITUM:
			UNIT_TEST(ose_getTypedDatumLength(i, Di), 0,
				  "unit type (0 bytes)");
			break;
		default:
			UNIT_TEST(ose_getTypedDatumLength(i, Di),
				  ASSERTION_FAILED, "unknown typetag");
			break;
		}
	}	
}

void ut_ose_getPayloadItemLength(void)
{
	// we don't need to re-run all the same tests that we ran for
	// ose_getTypedDatumLength. we just need to make sure the assertions
	// fail as expected.
	UNIT_TEST(ose_getPayloadItemLength(ose_makeBundle(NULL),
					 OSETT_INT32, 28),
		  ASSERTION_FAILED,
		  "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      ose_getPayloadItemLength(bundle,
						     OSETT_INT32, 0),
			      ASSERTION_FAILED,
			      "not in the payload");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      ose_getPayloadItemLength(bundle,
						     OSETT_INT32, 64),
			      ASSERTION_FAILED,
			      "beyond end of bundle");	
}

void ut_ose_getNthPayloadItem(void)
{
	int32_t to, ntt, lto, po, lpo;
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_getNthPayloadItem(bundle, 1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "NULL pointer");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      (ose_getNthPayloadItem(bundle, -1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "negative n");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      (ose_getNthPayloadItem(bundle, 1, 0,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "o in header");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      (ose_getNthPayloadItem(bundle, 1, 0,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      (ose_getNthPayloadItem(bundle, 1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S32 H S16 A2 Ti Di,
			      (ose_getNthPayloadItem(bundle, 1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "bad message size");
	UNIT_TEST_WITH_BUNDLE(S28 H S8 A2 T0,
			      (ose_getNthPayloadItem(bundle, 0, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "zero items, n = 0");
	UNIT_TEST_WITH_BUNDLE(S28 H S8 A2 T0,
			      (ose_getNthPayloadItem(bundle, 1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "zero items, n = 1");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      (ose_getNthPayloadItem(bundle, 0, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "one item, n = 0");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      (ose_getNthPayloadItem(bundle, 1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 2
				&& lto == 25
				&& po == 28
				&& lpo == 28)),
			      1,
			      "one item, n = 1");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      (ose_getNthPayloadItem(bundle, 2, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 2
				&& lto == 24
				&& po == 28
				&& lpo == 28)),
			      1,
			      "one item, n = 2");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A2 Ti Di,
			      (ose_getNthPayloadItem(bundle, 3, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "one item, n = 3");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tii Dii,
			      (ose_getNthPayloadItem(bundle, 0, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "two items, n = 0");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tii Dii,
			      (ose_getNthPayloadItem(bundle, 1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 3
				&& lto == 26
				&& po == 28
				&& lpo == 32)),
			      1,
			      "two items, n = 1");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tii Dii,
			      (ose_getNthPayloadItem(bundle, 2, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 3
				&& lto == 25
				&& po == 28
				&& lpo == 28)),
			      1,
			      "two items, n = 2");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tii Dii,
			      (ose_getNthPayloadItem(bundle, 3, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 3
				&& lto == 24
				&& po == 28
				&& lpo == 28)),
			      1,
			      "two items, n = 3");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A2 Tii Dii,
			      (ose_getNthPayloadItem(bundle, 4, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "two items, n = 3");

	UNIT_TEST_WITH_BUNDLE(S44 H S24 A2 Tiii Diii,
			      (ose_getNthPayloadItem(bundle, 0, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "three items, n = 0");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A2 Tiii Diii,
			      (ose_getNthPayloadItem(bundle, 1, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 4
				&& lto == 27
				&& po == 32
				&& lpo == 40)),
			      1,
			      "three items, n = 1");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A2 Tiii Diii,
			      (ose_getNthPayloadItem(bundle, 2, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 4
				&& lto == 26
				&& po == 32
				&& lpo == 36)),
			      1,
			      "three items, n = 2");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A2 Tiii Diii,
			      (ose_getNthPayloadItem(bundle, 3, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 4
				&& lto == 25
				&& po == 32
				&& lpo == 32)),
			      1,
			      "three items, n = 3");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A2 Tiii Diii,
			      (ose_getNthPayloadItem(bundle, 4, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       (to == 24
				&& ntt == 4
				&& lto == 24
				&& po == 32
				&& lpo == 32)),
			      1,
			      "three items, n = 4");
	UNIT_TEST_WITH_BUNDLE(S44 H S24 A2 Tiii Diii,
			      (ose_getNthPayloadItem(bundle, 5, 16,
						     &to, &ntt, &lto,
						     &po, &lpo),
			       0),
			      ASSERTION_FAILED,
			      "three items, n = 5");
}

void ut_ose_vwriteMessage(void){}

void ut_ose_writeMessage(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_writeMessage(bundle,
					       0, NULL, 0, 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      ose_writeMessage(bundle,
					       0, NULL, 0, 0),
			      ASSERTION_FAILED,
			      "offset in header");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      ose_writeMessage(bundle,
					       16, NULL, 0, 0),
			      ASSERTION_FAILED,
			      "NULL address");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      ose_writeMessage(bundle,
					       16, "/xx", -1, 0),
			      ASSERTION_FAILED,
			      "negative address length");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
					       16, "", 0, 0),
			       memcmp(ose_getBundlePtr(bundle),
				      H S8 A0 T0,
				      28)),
			      0,
			      "empty address");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
					       16, "/f", 2, 0),
			       memcmp(ose_getBundlePtr(bundle),
				      H S8 A2 T0,
				      28)),
			      0,
			      "non-empty address");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
					       16, "/foo", 4, 0),
			       memcmp(ose_getBundlePtr(bundle),
				      H S12 A4 T0,
				      32)),
			      0,
			      "address length 4");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_INT32, 0x20),
			       memcmp(ose_getBundlePtr(bundle),
				      H S16 A4 Ti Di,
				      36)),
			      0,
			      "address length 4, int32");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_FLOAT, 544.),
			       memcmp(ose_getBundlePtr(bundle),
				      H S16 A4 Tf Df,
				      36)),
			      0,
			      "address length 4, float");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_STRING, Ds0),
			       memcmp(ose_getBundlePtr(bundle),
				      H S16 A4 Ts Ds0,
				      36)),
			      0,
			      "address length 4, string length 0");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_STRING, Ds1),
			       memcmp(ose_getBundlePtr(bundle),
				      H S16 A4 Ts Ds1,
				      36)),
			      0,
			      "address length 4, string length 1");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_STRING, Ds2),
			       memcmp(ose_getBundlePtr(bundle),
				      H S16 A4 Ts Ds2,
				      36)),
			      0,
			      "address length 4, string length 2");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_STRING, Ds3),
			       memcmp(ose_getBundlePtr(bundle),
				      H S16 A4 Ts Ds3,
				      36)),
			      0,
			      "address length 4, string length 3");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_STRING, Ds4),
			       memcmp(ose_getBundlePtr(bundle),
				      H S20 A4 Ts Ds4,
				      40)),
			      0,
			      "address length 4, string length 4");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_BLOB, 0, &(Db0[4])),
			       memcmp(ose_getBundlePtr(bundle),
				      H S16 A4 Tb Db0,
				      36)),
			      0,
			      "address length 4, blob length 0");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_BLOB, 1, &(Db1[4])),
			       memcmp(ose_getBundlePtr(bundle),
				      H S20 A4 Tb Db1,
				      40)),
			      0,
			      "address length 4, blob length 1");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_BLOB, 2, &(Db2[4])),
			       memcmp(ose_getBundlePtr(bundle),
				      H S20 A4 Tb Db2,
				      40)),
			      0,
			      "address length 4, blob length 2");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_BLOB, 3, &(Db3[4])),
			       memcmp(ose_getBundlePtr(bundle),
				      H S20 A4 Tb Db3,
				      40)),
			      0,
			      "address length 4, blob length 3");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_BLOB, 4, &(Db4[4])),
			       memcmp(ose_getBundlePtr(bundle),
				      H S20 A4 Tb Db4,
				      40)),
			      0,
			      "address length 4, blob length 4");
	UNIT_TEST_WITH_BUNDLE(S16 H "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 1,
						OSETT_BLOB, 5, &(Db5[4])),
			       memcmp(ose_getBundlePtr(bundle),
				      H S24 A4 Tb Db5,
				      44)),
			      0,
			      "address length 4, blob length 5");
	
	UNIT_TEST_WITH_BUNDLE(S16 H
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
			      "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0",
			      (ose_writeMessage(bundle,
						16, "/foo", 4, 5,
						OSETT_INT32, 0x20,
						OSETT_FLOAT, 544.,
						OSETT_STRING, "fooo",
						OSETT_BLOB, 4, "\x01\x02\x03\x04",
						OSETT_INT32, 0x20),
			       memcmp(ose_getBundlePtr(bundle),
				      H S44 A4 ",ifsbi\0\0"
				      Di Df Ds4 Db4 Di,
				      60)),
			      0,
			      "address length 4, blob length 0");
}

int main(int ac, char **av)
{
	init();
	UNIT_TEST_FUNCTION(ose_pnbytes);
	UNIT_TEST_FUNCTION(ose_pstrlen);
	UNIT_TEST_FUNCTION(ose_isAddressChar);
	UNIT_TEST_FUNCTION(ose_isKnownTypetag);
	UNIT_TEST_FUNCTION(ose_isStringType);
	UNIT_TEST_FUNCTION(ose_isIntegerType);
	UNIT_TEST_FUNCTION(ose_isFloatType);
	UNIT_TEST_FUNCTION(ose_isNumericType);
	UNIT_TEST_FUNCTION(ose_isUnitType);
	UNIT_TEST_FUNCTION(ose_isBoolType);
	UNIT_TEST_FUNCTION(ose_isBundle);
	UNIT_TEST_FUNCTION(ose_bundleIsEmpty);
	UNIT_TEST_FUNCTION(ose_getBundleElemCount);
	UNIT_TEST_FUNCTION(ose_getBundleElemElemCount);
	UNIT_TEST_FUNCTION(ose_bundleHasAtLeastNElems);
	UNIT_TEST_FUNCTION(ose_getBundleElemType);

	UNIT_TEST_FUNCTION(ose_readByte);
	UNIT_TEST_FUNCTION(ose_writeByte);
	UNIT_TEST_FUNCTION(ose_readInt32);
	UNIT_TEST_FUNCTION(ose_writeInt32);
	UNIT_TEST_FUNCTION(ose_readFloat);
	UNIT_TEST_FUNCTION(ose_writeFloat);
	UNIT_TEST_FUNCTION(ose_readString);
	UNIT_TEST_FUNCTION(ose_getStringLen);
	UNIT_TEST_FUNCTION(ose_getPaddedStringLen);
	UNIT_TEST_FUNCTION(ose_writeStringWithLen);
	UNIT_TEST_FUNCTION(ose_writeString);
	UNIT_TEST_FUNCTION(ose_readBlob);
	UNIT_TEST_FUNCTION(ose_readBlobSize);
	UNIT_TEST_FUNCTION(ose_getBlobPaddingForNBytes);
	UNIT_TEST_FUNCTION(ose_getPaddedBlobSize);
	UNIT_TEST_FUNCTION(ose_readBlobPayload);
	UNIT_TEST_FUNCTION(ose_writeBlob);
	
	SKIP_UNIT_TEST_FUNCTION(ose_readDouble, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_writeDouble, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_readUInt32, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_writeUInt32, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_readInt64, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_writeInt64, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_readUInt64, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_writeUInt64, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_readTimetag, "test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_writeTimetag, "test not implemented yet");
	
	UNIT_TEST_FUNCTION(ose_getLastBundleElemOffset);
	UNIT_TEST_FUNCTION(ose_getBundleElemAddressOffset);
	UNIT_TEST_FUNCTION(ose_getBundleElemTTOffset);
	UNIT_TEST_FUNCTION(ose_getBundleElemPayloadOffset);

	SKIP_UNIT_TEST_FUNCTION(ose_getFirstOffsetForMatch,
				"test not implemented yet");
	SKIP_UNIT_TEST_FUNCTION(ose_getFirstOffsetForPMatch,
				"test not implemented yet");

	UNIT_TEST_FUNCTION(ose_getTypedDatumSize);
	UNIT_TEST_FUNCTION(ose_getPayloadItemSize);
	UNIT_TEST_FUNCTION(ose_getTypedDatumLength);
	UNIT_TEST_FUNCTION(ose_getPayloadItemLength);
	UNIT_TEST_FUNCTION(ose_getNthPayloadItem);

	SKIP_UNIT_TEST_FUNCTION(ose_vwriteMessage,
				"wrapper for ose_vwriteMessage");
	UNIT_TEST_FUNCTION(ose_writeMessage);
				

	finalize();
	return 0;
}
