all: ose

TESTDIR=./test
UNITTESTS=$(TESTDIR)/ut_ose_util $(TESTDIR)/ut_ose_stackops
TESTS=$(UNITTESTS)

ALLTYPES=-DOSE_CONF_PROVIDE_TYPE_SYMBOL \
-DOSE_CONF_PROVIDE_TYPE_DOUBLE \
-DOSE_CONF_PROVIDE_TYPE_INT8 \
-DOSE_CONF_PROVIDE_TYPE_UINT8 \
-DOSE_CONF_PROVIDE_TYPE_UINT32 \
-DOSE_CONF_PROVIDE_TYPE_INT64 \
-DOSE_CONF_PROVIDE_TYPE_UINT64 \
-DOSE_CONF_PROVIDE_TYPE_TIMETAG \
-DOSE_CONF_PROVIDE_TYPE_TRUE \
-DOSE_CONF_PROVIDE_TYPE_FALSE \
-DOSE_CONF_PROVIDE_TYPE_NULL \
-DOSE_CONF_PROVIDE_TYPE_INFINITUM

CORE_CFILES=ose.c ose_util.c ose_stackops.c ose_context.c ose_match.c ose_assert.c
CORE_HFILES=ose.h ose_conf.h ose_util.h ose_stackops.h ose_context.h ose_match.h ose_assert.h
SYS_CFILES=sys/ose_load.c
SYS_HFILES=sys/ose_load.h
VM_CFILES=ose_symtab.c ose_builtins.c ose_vm.c
VM_HFILES=ose_symtab.h ose_builtins.h ose_vm.h
LANG_CFILES=ose_print.c
LANG_HFILES=ose_print.h
TEST_CFILES=ut_ose_util.c ut_ose_stackops.c
TEST_HFILES=ut_ose_util.h ut_ose_stackops.h


CFLAGS_RELEASE=-I. -O3 -rdynamic
CFLAGS_DEBUG=-I. -DOSE_CONF_DEBUG -O0 -glldb -fsanitize=undefined -rdynamic

ose_symtab.c: ose_symtab.gperf
	gperf ose_symtab.gperf > ose_symtab.c

REPL_CFILES=$(CORE_CFILES) $(SYS_CFILES) $(VM_CFILES) $(LANG_CFILES)\
Applications/repl/ose_repl.c
REPL_HFILES=$(CORE_HFILES) $(SYS_HFILES) $(VM_HFILES) $(LANG_HFILES)
ose: CC=clang
ose: CFLAGS=$(CFLAGS_RELEASE)
ose: $(REPL_CFILES) $(REPL_HFILES) 
	$(CC) $(CFLAGS) -o ose \
	-DOSE_CONF_VM_SIZE=1000000 \
	$(REPL_CFILES) -ledit -ldl

debug: CC=clang
debug: CFLAGS=$(CFLAGS_DEBUG)
debug: $(REPL_CFILES) $(REPL_HFILES)
	$(CC) $(CFLAGS) -o ose \
	-DOSE_CONF_VM_SIZE=1000000 \
	$(REPL_CFILES) -ledit -ldl

JS_CFILES=$(CORE_CFILES) $(VM_CFILES) $(LANG_CFILES) js/osejs.c
JS_HFILES=$(CORE_HFILES) $(VM_HFILES) $(LANG_HFILES)
EMSCRIPTEN_EXPORTED_FUNCTIONS=$(shell cat js/osejs_export.mk)
js/libose.js: CC=emcc
js/libose.js: CFLAGS=-I. -O3 -s MALLOC="emmalloc" -s EXPORTED_FUNCTIONS=$(EMSCRIPTEN_EXPORTED_FUNCTIONS) -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -g0 -s 'EXPORT_NAME="libose"'
js/libose.js: $(JS_CFILES) $(JS_HFILES) js/osejs_export.mk js/ose.js js/osevm.js
	$(CC) $(CFLAGS) -o js/libose.js \
	$(JS_CFILES)
#	-DOSE_CONF_VM_SIZE=1000000 \

.PHONY: js
js: js/libose.js

$(TESTDIR)/%: CFLAGS=$(CFLAGS_DEBUG)
$(TESTDIR)/%: $(CORE_CFILES) $(CORE_HFILES) $(TESTDIR)/%.c $(TESTDIR)/common.h
	clang $(CFLAGS) $(ALLTYPES) -o $@ \
	$(CORE_CFILES) ose_print.c $@.c

.PHONY: test
test: $(TESTS) test/common.h test/ut_common.h

.PHONY: clean
clean:
	rm -rf ose *.dSYM $(TESTDIR)/*.dSYM $(TESTS) docs js/libose.js js/libose.wasm
