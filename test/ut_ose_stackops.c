#include "common.h"
#include "ut_common.h"
#include "../ose_stackops.h"

void ut_ose_pushInt32(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_pushInt32(bundle, 0), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushInt32(bundle, 32),
					   memcmp(S32 H S12 A0 Ti Di,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "Push 0");
}

void ut_ose_pushFloat(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_pushFloat(bundle, 0), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushFloat(bundle, 544),
					   memcmp(S32 H S12 A0 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "Push 0");
}

void ut_ose_pushString(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_pushString(bundle, 0), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushString(bundle, NULL), 0),
					  ASSERTION_FAILED,
					  "NULL string");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushString(bundle, Ds0),
					   memcmp(S32 H S12 A0 Ts Ds0,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "0 length string");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushString(bundle, Ds1),
					   memcmp(S32 H S12 A0 Ts Ds1,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "1 length string");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushString(bundle, Ds2),
					   memcmp(S32 H S12 A0 Ts Ds2,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "2 length string");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushString(bundle, Ds3),
					   memcmp(S32 H S12 A0 Ts Ds3,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "3 length string");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushString(bundle, Ds4),
					   memcmp(S36 H S16 A0 Ts Ds4,
						  ose_getBundlePtr(bundle) - 4,
						  40)),
					  0,
					  "4 length string");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushString(bundle, Ds5),
					   memcmp(S36 H S16 A0 Ts Ds5,
						  ose_getBundlePtr(bundle) - 4,
						  40)),
					  0,
					  "5 length string");
}

void ut_ose_pushBlob(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_pushBlob(bundle, 0, NULL), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushBlob(bundle, -4, NULL), 0),
					  ASSERTION_FAILED,
					  "negative size");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushBlob(bundle, 0, NULL),
					   memcmp(S32 H S12 A0 Tb Db0,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "NULL blob");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushBlob(bundle, 1, "\x01"),
					   memcmp(S36 H S16 A0 Tb Db1,
						  ose_getBundlePtr(bundle) - 4,
						  40)),
					  0,
					  "1 length blob");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushBlob(bundle, 2, "\x01\x02"),
					   memcmp(S36 H S16 A0 Tb Db2,
						  ose_getBundlePtr(bundle) - 4,
						  40)),
					  0,
					  "2 length blob");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushBlob(bundle, 3, "\x01\x02\x03"),
					   memcmp(S36 H S16 A0 Tb Db3,
						  ose_getBundlePtr(bundle) - 4,
						  40)),
					  0,
					  "3 length blob");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushBlob(bundle, 4, "\x01\x02\x03\x04"),
					   memcmp(S36 H S16 A0 Tb Db4,
						  ose_getBundlePtr(bundle) - 4,
						  40)),
					  0,
					  "4 length blob");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_pushBlob(bundle, 5, "\x01\x02\x03\x04\x05"),
					   memcmp(S40 H S20 A0 Tb Db5,
						  ose_getBundlePtr(bundle) - 4,
						  44)),
					  0,
					  "5 length blob");
}

void ut_ose_pushSymbol(void)
{

}

void ut_ose_pushDouble(void)
{

}

void ut_ose_pushInt8(void)
{

}

void ut_ose_pushUInt8(void)
{

}

void ut_ose_pushUInt32(void)
{

}

void ut_ose_pushInt64(void)
{

}

void ut_ose_pushUInt64(void)
{

}

void ut_ose_pushTimetag(void)
{

}

void ut_ose_pushTrue(void)
{

}

void ut_ose_pushFalse(void)
{

}

void ut_ose_pushNull(void)
{

}

void ut_ose_pushInfinitum(void)
{

}

void ut_ose_pushCFn(void)
{

}

void ut_ose_pushMessage(void)
{

}

void ut_ose_peekAddress(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_peekAddress(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_peekAddress(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A0 Ti Di,
					  strcmp(ose_peekAddress(bundle), A0),
					  0,
					  "1 message, empty address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A1 Ti Di,
					  strcmp(ose_peekAddress(bundle), A1),
					  0,
					  "1 message, 1 char address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A4 Ti Di,
					  strcmp(ose_peekAddress(bundle), A4),
					  0,
					  "1 message, 4 char address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A5 Ti Di,
					  strcmp(ose_peekAddress(bundle), A5),
					  0,
					  "1 message, 5 char address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S52 H S32 H S12 A3 Ti Di,
					  strcmp(ose_peekAddress(bundle), "#bundle"),
					  0,
					  "1 bundle");

	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A0 Ti Di S12 A0 Ti Di,
					  strcmp(ose_peekAddress(bundle), A0),
					  0,
					  "2 messages, empty address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S52 H S16 A1 Ts Ds4 S12 A1 Ti Di,
					  strcmp(ose_peekAddress(bundle), A1),
					  0,
					  "2 messages, 1 char address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S60 H S20 A4 Ts Ds5 S16 A4 Ti Di,
					  strcmp(ose_peekAddress(bundle), A4),
					  0,
					  "2 messages, 4 char address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S60 H S20 A5 Ts Ds5 S16 A5 Ti Di,
					  strcmp(ose_peekAddress(bundle), A5),
					  0,
					  "2 messages, 5 char address");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S72 H S12 A3 Ti Di S36 H S12 A3 Ti Di,
					  strcmp(ose_peekAddress(bundle), "#bundle"),
					  0,
					  "1 message, 1 bundle");
}

void ut_ose_peekMessageArgType(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_peekMessageArgType(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_peekMessageArgType(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S20 H S0,
					  ose_peekMessageArgType(bundle),
					  OSETT_NOTYPETAG,
					  "empty message (size 0)");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S24 H S4 A0,
					  ose_peekMessageArgType(bundle),
					  OSETT_NOTYPETAG,
					  "empty message (size 4)");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S28 H S8 A0 T0,
					  ose_peekMessageArgType(bundle),
					  OSETT_NOTYPETAG,
					  "empty message (size 8)");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A0 Ti Di,
					  ose_peekMessageArgType(bundle),
					  OSETT_INT32,
					  "int32");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A0 Tf Df,
					  ose_peekMessageArgType(bundle),
					  OSETT_FLOAT,
					  "float");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A0 Ts Ds3,
					  ose_peekMessageArgType(bundle),
					  OSETT_STRING,
					  "string");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S36 H S16 A0 Tb Db4,
					  ose_peekMessageArgType(bundle),
					  OSETT_BLOB,
					  "blob");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S36 H S12 A0 Tiiii Diiii,
					  ose_peekMessageArgType(bundle),
					  ASSERTION_FAILED,
					  "bad message size");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S56 H S36 H S16 A0 Tb Db4,
					  ose_peekMessageArgType(bundle),
					  ASSERTION_FAILED,
					  "bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S52 H S32 A0 Tifsb Di Df Ds4 Db0,
					  ose_peekMessageArgType(bundle),
					  OSETT_BLOB,
					  "mixed types");
}

void ut_ose_peekType(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_peekType(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      (ose_peekType(bundle), 0),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S20 H S0,
			      ose_peekType(bundle),
			      OSETT_MESSAGE,
			      "empty message (size 0)");
	UNIT_TEST_WITH_BUNDLE(S24 H S4 A0,
			      ose_peekType(bundle),
			      OSETT_MESSAGE,
			      "empty message (size 4)");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ti Di,
			      ose_peekType(bundle),
			      OSETT_MESSAGE,
			      "normal message");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 H,
			      ose_peekType(bundle),
			      OSETT_BUNDLE,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S52 H S32 H S12 A2 Ti Di,
			      ose_peekType(bundle),
			      OSETT_BUNDLE,
			      "normal bundle");
	UNIT_TEST_WITH_BUNDLE(S72 H S32 H S12 A2 Ti Di S12 A2 Tf Df,
			      ose_peekType(bundle),
			      OSETT_MESSAGE,
			      "bundle message");
	UNIT_TEST_WITH_BUNDLE(S72 H S12 A2 Tf Df S32 H S16 A2 Ti Di,
			      ose_peekType(bundle),
			      OSETT_MESSAGE,
			      "message bundle");
}

void ut_ose_peekInt32(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_peekInt32(bundle),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_peekInt32(bundle),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S20 H S0,
			      ose_peekInt32(bundle),
			      ASSERTION_FAILED,
			      "empty message (size 0)");
	UNIT_TEST_WITH_BUNDLE(S24 H S4 A0,
			      ose_peekInt32(bundle),
			      ASSERTION_FAILED,
			      "empty message (size 4)");
	UNIT_TEST_WITH_BUNDLE(S28 H S8 A0 T0,
			      ose_peekInt32(bundle),
			      ASSERTION_FAILED,
			      "empty message (size 8)");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ti Di,
			      ose_peekInt32(bundle),
			      32,
			      "int32");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tf Df,
			      ose_peekInt32(bundle),
			      1141374976,
			      "float");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds5,
			      ose_peekInt32(bundle),
			      1718579055,
			      "string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db4,
			      ose_peekInt32(bundle),
			      4,
			      "blob");
}

void ut_ose_peekFloat(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      ose_peekFloat(bundle),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      ose_peekFloat(bundle),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S20 H S0,
			      ose_peekFloat(bundle),
			      ASSERTION_FAILED,
			      "empty message (size 0)");
	UNIT_TEST_WITH_BUNDLE(S24 H S4 A0,
			      ose_peekFloat(bundle),
			      ASSERTION_FAILED,
			      "empty message (size 4)");
	UNIT_TEST_WITH_BUNDLE(S28 H S8 A0 T0,
			      ose_peekFloat(bundle),
			      ASSERTION_FAILED,
			      "empty message (size 8)");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tf Df,
			      ose_peekFloat(bundle) == 544.f,
			      1,
			      "float");
}

void ut_ose_peekString(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_peekString(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      (ose_peekString(bundle), 0),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S20 H S0,
			      (ose_peekString(bundle), 0),
			      ASSERTION_FAILED,
			      "empty message (size 0)");
	UNIT_TEST_WITH_BUNDLE(S24 H S4 A0,
			      (ose_peekString(bundle), 0),
			      ASSERTION_FAILED,
			      "empty message (size 4)");
	UNIT_TEST_WITH_BUNDLE(S28 H S8 A0 T0,
			      (ose_peekString(bundle), 0),
			      ASSERTION_FAILED,
			      "empty message (size 8)");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds0,
			      strcmp(ose_peekString(bundle), Ds0),
			      0,
			      "0 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds1,
			      strcmp(ose_peekString(bundle), Ds1),
			      0,
			      "1 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds2,
			      strcmp(ose_peekString(bundle), Ds2),
			      0,
			      "2 length string");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Ts Ds3,
			      strcmp(ose_peekString(bundle), Ds3),
			      0,
			      "3 length string");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Ts Ds4,
			      strcmp(ose_peekString(bundle), Ds4),
			      0,
			      "4 length string");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Ts Ds5,
			      strcmp(ose_peekString(bundle), Ds5),
			      0,
			      "5 length string");
}

void ut_ose_peekBlob(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_peekBlob(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_BUNDLE(S16 H,
			      (ose_peekBlob(bundle), 0),
			      ASSERTION_FAILED,
			      "empty bundle");
	UNIT_TEST_WITH_BUNDLE(S20 H S0,
			      (ose_peekBlob(bundle), 0),
			      ASSERTION_FAILED,
			      "empty message (size 0)");
	UNIT_TEST_WITH_BUNDLE(S24 H S4 A0,
			      (ose_peekBlob(bundle), 0),
			      ASSERTION_FAILED,
			      "empty message (size 4)");
	UNIT_TEST_WITH_BUNDLE(S28 H S8 A0 T0,
			      (ose_peekBlob(bundle), 0),
			      ASSERTION_FAILED,
			      "empty message (size 8)");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db0,
			      strcmp(ose_peekBlob(bundle), Db0),
			      0,
			      "0 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db1,
			      strcmp(ose_peekBlob(bundle), Db1),
			      0,
			      "1 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db2,
			      strcmp(ose_peekBlob(bundle), Db2),
			      0,
			      "2 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db3,
			      strcmp(ose_peekBlob(bundle), Db3),
			      0,
			      "3 length blob");
	UNIT_TEST_WITH_BUNDLE(S32 H S12 A0 Tb Db4,
			      strcmp(ose_peekBlob(bundle), Db4),
			      0,
			      "4 length blob");
	UNIT_TEST_WITH_BUNDLE(S36 H S16 A0 Tb Db5,
			      strcmp(ose_peekBlob(bundle), Db5),
			      0,
			      "5 length blob");
}

void ut_ose_peekSymbol(void)
{

}

void ut_ose_peekDouble(void)
{

}

void ut_ose_peekInt8(void)
{

}

void ut_ose_peekUInt8(void)
{

}

void ut_ose_peekUInt32(void)
{

}

void ut_ose_peekInt64(void)
{

}

void ut_ose_peekUInt64(void)
{

}

void ut_ose_peekTimetag(void)
{

}

void ut_ose_peekTrue(void)
{

}

void ut_ose_peekFalse(void)
{

}

void ut_ose_peekNull(void)
{

}

void ut_ose_peekInfinitum(void)
{

}

// void ut_ose_peekCFn(void)
// {

// }

void ut_ose_popInt32(void){}
void ut_ose_popFloat(void){}
void ut_ose_popString(void){}
void ut_ose_popBlob(void){}

void ut_ose_popSymbol(void)
{

}

void ut_ose_popDouble(void)
{

}

void ut_ose_popInt8(void)
{

}

void ut_ose_popUInt8(void)
{

}

void ut_ose_popUInt32(void)
{

}

void ut_ose_popInt64(void)
{

}

void ut_ose_popUInt64(void)
{

}

void ut_ose_popTimetag(void)
{

}

/**************************************************
 * Stack Operations
 **************************************************/
void ut_ose_2drop(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_2drop(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_2drop(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_2drop(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_2drop(bundle),
					   memcmp(S16 H,
						  ose_getBundlePtr(bundle) - 4,
						  20)),
					  0,
					  "bundle with 2 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_2drop(bundle),
					   memcmp(S32 H S12 A2 Ti Di,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "bundle with 3 messages");
}
void ut_ose_2dup(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_2dup(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_2dup(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_2dup(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_2dup(bundle),
					   memcmp(S80 H S12 A2 Ti Di S12 A1 Tf Df S12 A2 Ti Di S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  84)),
					  0,
					  "bundle with 2 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_2dup(bundle),
					   memcmp(S104 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1 S12 A1 Tf Df S16 A4 Ts Ds1,
						  ose_getBundlePtr(bundle) - 4,
						  108)),
					  0,
					  "bundle with 3 messages");
}
void ut_ose_2over(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_2over(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_2over(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_2over(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_2over(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only two messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2,
					  (ose_2over(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only three messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S88 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2 S16 A3 Ts Ds4,
					  (ose_2over(bundle),
					   memcmp(S120 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2 S16 A3 Ts Ds4 S12 A2 Ti Di S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  124)),
					  0,
					  "bundle with 4 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S112 H S20 A4 Ts Ds4 S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2 S16 A3 Ts Ds4,
					  (ose_2over(bundle),
					   memcmp(S144 H S20 A4 Ts Ds4 S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2 S16 A3 Ts Ds4 S12 A2 Ti Di S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  148)),
					  0,
					  "bundle with 5 messages");
}
void ut_ose_2swap(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_2swap(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_2swap(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_2swap(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_2swap(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only two messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2,
					  (ose_2swap(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only three messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S88 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2 S16 A3 Ts Ds4,
					  (ose_2swap(bundle),
					   memcmp(S88 H S16 A4 Ts Ds2 S16 A3 Ts Ds4 S12 A2 Ti Di S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  92)),
					  0,
					  "bundle with 4 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S112 H S20 A4 Ts Ds4 S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds2 S16 A3 Ts Ds4,
					  (ose_2swap(bundle),
					   memcmp(S112 H S20 A4 Ts Ds4 S16 A4 Ts Ds2 S16 A3 Ts Ds4 S12 A2 Ti Di S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  116)),
					  0,
					  "bundle with 5 messages");
}
void ut_ose_drop(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_drop(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_drop(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_drop(bundle),
					   memcmp(S16 H,
						  ose_getBundlePtr(bundle) - 4,
						  20)),
					  0,
					  "bundle with 1 message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_drop(bundle),
					   memcmp(S32 H S12 A2 Ti Di,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "bundle with 2 messages");
}
void ut_ose_dup(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_dup(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_dup(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_dup(bundle),
					   memcmp(S48 H S12 A2 Ti Di S12 A2 Ti Di,
						  ose_getBundlePtr(bundle) - 4,
						  52)),
					  0,
					  "bundle with 1 message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_dup(bundle),
					   memcmp(S64 H S12 A2 Ti Di S12 A1 Tf Df S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  68)),
					  0,
					  "bundle with 2 messages");
}
void ut_ose_nip(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_nip(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_nip(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_nip(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_nip(bundle),
					   memcmp(S32 H S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  36)),
					  0,
					  "bundle with 2 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_nip(bundle),
					   memcmp(S52 H S12 A2 Ti Di S16 A4 Ts Ds1,
						  ose_getBundlePtr(bundle) - 4,
						  56)),
					  0,
					  "bundle with 3 messages");
}
void ut_ose_notrot(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_notrot(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_notrot(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
			      (ose_notrot(bundle), 0),
			      ASSERTION_FAILED,
			      "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_notrot(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only two messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_notrot(bundle),
					   memcmp(S68 H S16 A4 Ts Ds1 S12 A2 Ti Di S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  72)),
					  0,
					  "bundle with 3 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S92 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1 S20 A5 Tb Db4,
					  (ose_notrot(bundle),
					   memcmp(S92 H S12 A2 Ti Di S20 A5 Tb Db4 S12 A1 Tf Df S16 A4 Ts Ds1,
						  ose_getBundlePtr(bundle) - 4,
						  96)),
					  0,
					  "bundle with 4 messages");
}
void ut_ose_over(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_over(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_over(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_over(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_over(bundle),
					   memcmp(S64 H S12 A2 Ti Di S12 A1 Tf Df S12 A2 Ti Di,
						  ose_getBundlePtr(bundle) - 4,
						  68)),
					  0,
					  "bundle with 2 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_over(bundle),
					   memcmp(S84 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1 S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  88)),
					  0,
					  "bundle with 3 messages");
}
void ut_ose_pick(void)
{

}
void ut_ose_pickBottom(void)
{

}
void ut_ose_pickMatch(void)
{

}
void ut_ose_roll(void)
{

}
void ut_ose_rollBottom(void)
{

}
void ut_ose_rollMatch(void)
{

}
void ut_ose_rot(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_rot(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_rot(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_rot(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_rot(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only two messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_rot(bundle),
					   memcmp(S68 H S12 A1 Tf Df S16 A4 Ts Ds1  S12 A2 Ti Di,
						  ose_getBundlePtr(bundle) - 4,
						  72)),
					  0,
					  "bundle with 3 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S92 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1 S20 A5 Tb Db4,
					  (ose_rot(bundle),
					   memcmp(S92 H S12 A2 Ti Di S16 A4 Ts Ds1 S20 A5 Tb Db4 S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  96)),
					  0,
					  "bundle with 4 messages");
}
void ut_ose_swap(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_swap(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_swap(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_swap(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_swap(bundle),
					   memcmp(S48 H S12 A1 Tf Df S12 A2 Ti Di,
						  ose_getBundlePtr(bundle) - 4,
						  52)),
					  0,
					  "bundle with 2 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_swap(bundle),
					   memcmp(S68 H S12 A2 Ti Di S16 A4 Ts Ds1 S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  72)),
					  0,
					  "bundle with 3 messages");
}
void ut_ose_tuck(void)
{
	UNIT_TEST_WITH_BUNDLE(NULL,
			      (ose_tuck(bundle), 0),
			      ASSERTION_FAILED,
			      "NULL bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S16 H,
					  (ose_tuck(bundle), 0),
					  ASSERTION_FAILED,
					  "empty bundle");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S32 H S12 A2 Ti Di,
					  (ose_tuck(bundle), 0),
					  ASSERTION_FAILED,
					  "bundle with only one message");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S48 H S12 A2 Ti Di S12 A1 Tf Df,
					  (ose_tuck(bundle),
					   memcmp(S64 H S12 A1 Tf Df S12 A2 Ti Di S12 A1 Tf Df,
						  ose_getBundlePtr(bundle) - 4,
						  68)),
					  0,
					  "bundle with 2 messages");
	UNIT_TEST_WITH_INITIALIZED_BUNDLE(S68 H S12 A2 Ti Di S12 A1 Tf Df S16 A4 Ts Ds1,
					  (ose_tuck(bundle),
					   memcmp(S88 H S12 A2 Ti Di S16 A4 Ts Ds1 S12 A1 Tf Df S16 A4 Ts Ds1,
						  ose_getBundlePtr(bundle) - 4,
						  92)),
					  0,
					  "bundle with 3 messages");
}

/**************************************************
 * Grouping / Ungrouping
 **************************************************/
void ut_ose_bundleAll(void)
{

}
void ut_ose_bundleFromBottom(void)
{

}
void ut_ose_bundleFromTop(void)
{

}
void ut_ose_clear(void)
{

}
void ut_ose_clearPayload(void)
{

}
void ut_ose_pop(void)
{

}
void ut_ose_popAll(void)
{

}
void ut_ose_popAllDrop(void)
{

}
void ut_ose_popAllBundle(void)
{

}
void ut_ose_popAllDropBundle(void)
{

}
void ut_ose_push(void)
{

}
void ut_ose_unpack(void)
{

}
void ut_ose_unpackDrop(void)
{

}
void ut_ose_unpackBundle(void)
{

}
void ut_ose_unpackDropBundle(void)
{

}


/**************************************************
 * Queries
 **************************************************/
void ut_ose_countElems(void)
{

}
void ut_ose_countItems(void)
{

}
void ut_ose_lengthAddress(void)
{

}
void ut_ose_lengthTT(void)
{

}
void ut_ose_lengthItem(void)
{

}
void ut_ose_lengthsItems(void)
{

}
void ut_ose_sizeAddress(void)
{

}
void ut_ose_sizeElem(void)
{

}
void ut_ose_sizeItem(void)
{

}
void ut_ose_sizePayload(void)
{

}
void ut_ose_sizesElems(void)
{

}
void ut_ose_sizesItems(void)
{

}
void ut_ose_sizeTT(void)
{

}

/**************************************************
 * Operations on Bundle Elements and Items
 **************************************************/
void ut_ose_blobToElem(void)
{

}
void ut_ose_blobToType(void)
{

}
void ut_ose_concatenateBlobs(void)
{

}
void ut_ose_concatenateStrings(void)
{

}
void ut_ose_copyAddressToString(void)
{

}
void ut_ose_copyPayloadToBlob(void)
{

}
void ut_ose_swapStringToAddress(void)
{

}
void ut_ose_copyTTToBlob(void)
{

}
void ut_ose_decatenateBlob(void)
{

}
void ut_ose_decatenateString(void)
{

}
void ut_ose_elemToBlob(void)
{

}
void ut_ose_itemToBlob(void)
{

}
void ut_ose_joinBlobs(void)
{

}
void ut_ose_joinStrings(void)
{

}
void ut_ose_moveStringToAddress(void)
{

}
void ut_ose_splitBlobFromEnd(void)
{

}
void ut_ose_splitBlobFromStart(void)
{

}
void ut_ose_splitStringFromEnd(void)
{

}
void ut_ose_splitStringFromStart(void)
{

}
void ut_ose_swap4Bytes(void)
{

}
void ut_ose_swap8Bytes(void)
{

}
void ut_ose_swapNBytes(void)
{

}
void ut_ose_trimBlob(void)
{

}
void ut_ose_trimString(void)
{

}
void ut_ose_match(void)
{

}
void ut_ose_pmatch(void)
{

}

/**************************************************
 * Creatio Ex Nihilo
 **************************************************/
void ut_ose_makeBlob(void)
{

}
void ut_ose_pushBundle(void)
{

}

/**************************************************
 * Arithmetic
 **************************************************/
void ut_ose_add(void)
{

}
void ut_ose_sub(void)
{

}
void ut_ose_mul(void)
{

}
void ut_ose_div(void)
{

}
void ut_ose_mod(void)
{

}
void ut_ose_neg(void)
{

}
void ut_ose_eql(void)
{

}
void ut_ose_lte(void)
{

}
void ut_ose_lt(void)
{

}
void ut_ose_and(void)
{

}
void ut_ose_or(void)
{

}

int main(int ac, char **av)
{
	init();
	
	UNIT_TEST_FUNCTION(ose_pushInt32);
	UNIT_TEST_FUNCTION(ose_pushFloat);
	UNIT_TEST_FUNCTION(ose_pushString);
	UNIT_TEST_FUNCTION(ose_pushBlob);
	SKIP_UNIT_TEST_FUNCTION(ose_pushSymbol, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushDouble, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushInt8, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushUInt8, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushUInt32, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushInt64, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushUInt64, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushTimetag, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushTrue, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushFalse, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushNULL, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushInfinitum, "");
	UNIT_TEST_FUNCTION(ose_pushCFn);
	SKIP_UNIT_TEST_FUNCTION(ose_pushMessage, "");

	UNIT_TEST_FUNCTION(ose_peekAddress);
	UNIT_TEST_FUNCTION(ose_peekMessageArgType);
	UNIT_TEST_FUNCTION(ose_peekType);

	UNIT_TEST_FUNCTION(ose_peekInt32);
	UNIT_TEST_FUNCTION(ose_peekFloat);
	UNIT_TEST_FUNCTION(ose_peekString);
	UNIT_TEST_FUNCTION(ose_peekBlob);
	SKIP_UNIT_TEST_FUNCTION(ose_peekSymbol, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekDouble, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekInt8, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekUInt8, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekUInt32, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekInt64, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekUInt64, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekTimetag, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekTrue, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekFalse, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekNULL, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekInfinitum, "");
	SKIP_UNIT_TEST_FUNCTION(ose_peekCFn, "");

	SKIP_UNIT_TEST_FUNCTION(ose_popInt32, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popFloat, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popString, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popBlob, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popSymbol, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popDouble, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popInt8, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popUInt8, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popUInt32, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popInt64, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popUInt64, "covered by peek<type> and drop");
	SKIP_UNIT_TEST_FUNCTION(ose_popTimetag, "covered by peek<type> and drop");

	/**************************************************
	 * Stack Operations
	 **************************************************/
	UNIT_TEST_FUNCTION(ose_2drop);
	UNIT_TEST_FUNCTION(ose_2dup);
	UNIT_TEST_FUNCTION(ose_2over);
	UNIT_TEST_FUNCTION(ose_2swap);
	UNIT_TEST_FUNCTION(ose_drop);
	UNIT_TEST_FUNCTION(ose_dup);
	UNIT_TEST_FUNCTION(ose_nip);
	UNIT_TEST_FUNCTION(ose_notrot);
	UNIT_TEST_FUNCTION(ose_over);
	UNIT_TEST_FUNCTION(ose_pick);
	UNIT_TEST_FUNCTION(ose_pickBottom);
	UNIT_TEST_FUNCTION(ose_pickMatch);
	UNIT_TEST_FUNCTION(ose_roll);
	UNIT_TEST_FUNCTION(ose_rollBottom);
	UNIT_TEST_FUNCTION(ose_rollMatch);
	UNIT_TEST_FUNCTION(ose_rot);
	UNIT_TEST_FUNCTION(ose_swap);
	UNIT_TEST_FUNCTION(ose_tuck);

	/**************************************************
	 * Grouping / Ungrouping
	 **************************************************/
	SKIP_UNIT_TEST_FUNCTION(ose_bundleAll, "");
	SKIP_UNIT_TEST_FUNCTION(ose_bundleFromBottom, "");
	SKIP_UNIT_TEST_FUNCTION(ose_bundleFromTop, "");
	SKIP_UNIT_TEST_FUNCTION(ose_clear, "");
	SKIP_UNIT_TEST_FUNCTION(ose_clearPayload, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pop, "");
	SKIP_UNIT_TEST_FUNCTION(ose_popAll, "");
	SKIP_UNIT_TEST_FUNCTION(ose_popAllDrop, "");
	SKIP_UNIT_TEST_FUNCTION(ose_popAllBundle, "");
	SKIP_UNIT_TEST_FUNCTION(ose_popAllDropBundle, "");
	SKIP_UNIT_TEST_FUNCTION(ose_push, "");
	SKIP_UNIT_TEST_FUNCTION(ose_unpack, "");
	SKIP_UNIT_TEST_FUNCTION(ose_unpackDrop, "");
	SKIP_UNIT_TEST_FUNCTION(ose_unpackBundle, "");
	SKIP_UNIT_TEST_FUNCTION(ose_unpackDropBundle, "");


	/**************************************************
	 * Queries
	 **************************************************/
	SKIP_UNIT_TEST_FUNCTION(ose_countElems, "");
	SKIP_UNIT_TEST_FUNCTION(ose_countItems, "");
	SKIP_UNIT_TEST_FUNCTION(ose_lengthAddress, "");
	SKIP_UNIT_TEST_FUNCTION(ose_lengthTT, "");
	SKIP_UNIT_TEST_FUNCTION(ose_lengthItem, "");
	SKIP_UNIT_TEST_FUNCTION(ose_lengthsItems, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sizeAddress, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sizeElem, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sizeItem, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sizePayload, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sizesElems, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sizesItems, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sizeTT, "");

	/**************************************************
	 * Operations on Bundle Elements and Items
	 **************************************************/
	SKIP_UNIT_TEST_FUNCTION(ose_blobToElem, "");
	SKIP_UNIT_TEST_FUNCTION(ose_blobToType, "");
	SKIP_UNIT_TEST_FUNCTION(ose_concatenateBlobs, "");
	SKIP_UNIT_TEST_FUNCTION(ose_concatenateStrings, "");
	SKIP_UNIT_TEST_FUNCTION(ose_copyAddressToString, "");
	SKIP_UNIT_TEST_FUNCTION(ose_copyPayloadToBlob, "");
	SKIP_UNIT_TEST_FUNCTION(ose_swapStringToAddress, "");
	SKIP_UNIT_TEST_FUNCTION(ose_copyTTToBlob, "");
	SKIP_UNIT_TEST_FUNCTION(ose_decatenateBlob, "");
	SKIP_UNIT_TEST_FUNCTION(ose_decatenateString, "");
	SKIP_UNIT_TEST_FUNCTION(ose_elemToBlob, "");
	SKIP_UNIT_TEST_FUNCTION(ose_itemToBlob, "");
	SKIP_UNIT_TEST_FUNCTION(ose_joinBlobs, "");
	SKIP_UNIT_TEST_FUNCTION(ose_joinStrings, "");
	SKIP_UNIT_TEST_FUNCTION(ose_moveStringToAddress, "");
	SKIP_UNIT_TEST_FUNCTION(ose_splitBlobFromEnd, "");
	SKIP_UNIT_TEST_FUNCTION(ose_splitBlobFromStart, "");
	SKIP_UNIT_TEST_FUNCTION(ose_splitStringFromEnd, "");
	SKIP_UNIT_TEST_FUNCTION(ose_splitStringFromStart, "");
	SKIP_UNIT_TEST_FUNCTION(ose_swap4Bytes, "");
	SKIP_UNIT_TEST_FUNCTION(ose_swap8Bytes, "");
	SKIP_UNIT_TEST_FUNCTION(ose_swapNBytes, "");
	SKIP_UNIT_TEST_FUNCTION(ose_trimBlob, "");
	SKIP_UNIT_TEST_FUNCTION(ose_trimString, "");
	SKIP_UNIT_TEST_FUNCTION(ose_match, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pmatch, "");

	/**************************************************
	 * Creatio Ex Nihilo
	 **************************************************/
	SKIP_UNIT_TEST_FUNCTION(ose_makeBlob, "");
	SKIP_UNIT_TEST_FUNCTION(ose_pushBundle, "");

	/**************************************************
	 * Arithmetic
	 **************************************************/
	SKIP_UNIT_TEST_FUNCTION(ose_add, "");
	SKIP_UNIT_TEST_FUNCTION(ose_sub, "");
	SKIP_UNIT_TEST_FUNCTION(ose_mul, "");
	SKIP_UNIT_TEST_FUNCTION(ose_div, "");
	SKIP_UNIT_TEST_FUNCTION(ose_mod, "");
	SKIP_UNIT_TEST_FUNCTION(ose_neg, "");
	SKIP_UNIT_TEST_FUNCTION(ose_eql, "");
	SKIP_UNIT_TEST_FUNCTION(ose_lte, "");
	SKIP_UNIT_TEST_FUNCTION(ose_lt, "");
	SKIP_UNIT_TEST_FUNCTION(ose_and, "");
	SKIP_UNIT_TEST_FUNCTION(ose_or, "");
	
	finalize();
	return 0;
}
