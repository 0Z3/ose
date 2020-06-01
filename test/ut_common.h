#define UNIT_TEST_FUNCTION(fn) TEST_FUNCTION(ut_, fn)
#define SKIP_UNIT_TEST_FUNCTION(fn, reason) SKIP_TEST_FUNCTION(fn, reason)

void printb(char *b)
{
	if(b){
		// prevent load of misaligned address runtime error
		char buf[8];
		char *p = align(buf);
		for(int i = 0; i < 4; i++){
			p[i] = b[i];
		}
		int32_t s = htonl(*((int32_t *)p));
		for(int32_t i = 0; i < s + 4; i++){
			if(b[i] <= 32 || b[i] > 127){
				printf("\\x%02x", b[i]);
			}else{
				printf("%c", b[i]);
			}
		}
	}else{
		printf("NULL");
	}
}

#define UNIT_TEST_IMPL(test, expected_result, desc, file, func, line)	\
	{								\
		testctr++;						\
		int32_t r = ASSERTION_FAILED;				\
		if(!setjmp(env)){					\
			r = test;					\
		}							\
		if(r != expected_result){				\
			printf(ANSI_COLOR_RED				\
			       "TEST %d FAILED: "			\
			       ANSI_COLOR_RESET				\
			       "%s:%s:%d:\n%s\n"			\
			       "expected %d, but got %d\n",		\
			       testctr,					\
			       file, func, line,			\
			       #test, expected_result, r);		\
			failctr++;					\
		}else if(verbose){					\
			printf(ANSI_COLOR_GREEN				\
			       "TEST %d PASSED:\n"			\
			       ANSI_COLOR_RESET				\
			       "%s = %d\n",				\
			       testctr, #test, expected_result);	\
		}							\
	}								\

#define UNIT_TEST(test, expected_result, desc)		\
	UNIT_TEST_IMPL(test, expected_result, desc,	\
		       __FILE__, __func__, __LINE__);
	

#define UNIT_TEST_WITH_BUNDLE(b, test, expected_result, desc)	\
	{							\
		char buf[MAX_BNDLSIZE];				\
		char *p = NULL;					\
		ose_bundle bundle;				\
		if(b){						\
			p = align(buf);				\
			memcpy(p, b, sizeof(b));		\
			bundle = ose_makeBundle(p + 4);		\
		}else{						\
			bundle = ose_makeBundle(p);		\
		}						\
		if(verbose){					\
			printf("bundle = ");			\
			printb(b);				\
			printf("\n");				\
		}						\
		UNIT_TEST(test, expected_result, desc);		\
	}

#define UNIT_TEST_WITH_INITIALIZED_BUNDLE(b, test, expected_result, desc) \
	{								\
		char buf[MAX_BNDLSIZE];					\
		ose_bundle bundle = ose_newBundleFromCBytes(MAX_BNDLSIZE, buf); \
		if(b){							\
			int32_t sizeofb = sizeof(b);			\
			int32_t size1 = ose_readInt32(bundle, -4);	\
			int32_t size2 = ose_readInt32(bundle, size1);	\
			ose_writeInt32(bundle, size1, 0);		\
			char *p = ose_getBundlePtr(bundle);		\
			memcpy(p - 4, b, sizeofb);			\
			ose_writeInt32(bundle, sizeofb - 5, size2 - (sizeofb - 20)); \
		}else{							\
		}							\
		if(verbose){						\
			printf("bundle = ");				\
			printb(b);					\
			printf("\n");					\
		}							\
		UNIT_TEST(test, expected_result, desc);			\
	}

#define SKIP_UNIT_TEST_WITH_BUNDLE(b, test, expected_result, desc)	\
	SKIP_TEST(test, expected_result, desc)

#define SKIP_UNIT_TEST(test, expected_result, desc)	\
	SKIP_TEST(test, expected_result, desc)
