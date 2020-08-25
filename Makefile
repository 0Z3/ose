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
BUILTINS_CFILES=ose_builtins.c
BUILTINS_HFILES=ose_builtins.h
VM_CFILES=ose_symtab.c ose_vm.c $(BUILTINS_CFILES)
VM_HFILES=ose_symtab.h ose_vm.h $(BUILTINS_HFILES)
LANG_CFILES=ose_print.c
LANG_HFILES=ose_print.h
TEST_CFILES=ut_ose_util.c ut_ose_stackops.c
TEST_HFILES=ut_ose_util.h ut_ose_stackops.h


CFLAGS_RELEASE=-I. -O3 -rdynamic -lm
CFLAGS_DEBUG=-I. -DOSE_CONF_DEBUG -O0 -glldb -fsanitize=undefined -rdynamic -lm

ose_symtab.c: ose_symtab.gperf
	gperf ose_symtab.gperf > ose_symtab.c

REPL_CFILES=$(CORE_CFILES) $(SYS_CFILES) $(VM_CFILES) $(LANG_CFILES)\
Applications/repl/ose_repl.c
REPL_HFILES=$(CORE_HFILES) $(SYS_HFILES) $(VM_HFILES) $(LANG_HFILES)
ose: CC=clang
ose: CFLAGS=$(CFLAGS_RELEASE)
ose: HOOKS=-DOSEVM_PREINPUT=ose_repl_preInput -DOSEVM_POSTINPUT=ose_repl_postInput -DOSEVM_PRECONTROL=ose_repl_preControl -DOSEVM_POSTCONTROL=ose_repl_postControl -DOSEVM_FUNCALL=ose_repl_funcall
ose: $(REPL_CFILES) $(REPL_HFILES) 
	$(CC) $(CFLAGS) -o ose \
	$(HOOKS) \
	-DOSE_CONF_VM_INPUT_SIZE=131072 \
	-DOSE_CONF_VM_STACK_SIZE=131072 \
	-DOSE_CONF_VM_ENV_SIZE=131072 \
	-DOSE_CONF_VM_CONTROL_SIZE=131072 \
	-DOSE_CONF_VM_DUMP_SIZE=131072 \
	-DOSE_CONF_VM_OUTPUT_SIZE=131072 \
	$(REPL_CFILES) -ledit -ldl

debug: CC=clang
debug: CFLAGS=$(CFLAGS_DEBUG)
debug: HOOKS=-DOSEVM_PREINPUT=ose_repl_preInput -DOSEVM_POSTINPUT=ose_repl_postInput -DOSEVM_PRECONTROL=ose_repl_preControl -DOSEVM_POSTCONTROL=ose_repl_postControl -DOSEVM_FUNCALL=ose_repl_funcall
debug: $(REPL_CFILES) $(REPL_HFILES)
	$(CC) $(CFLAGS) -o ose \
	$(HOOKS) \
	-DOSE_CONF_VM_INPUT_SIZE=131072 \
	-DOSE_CONF_VM_STACK_SIZE=131072 \
	-DOSE_CONF_VM_ENV_SIZE=131072 \
	-DOSE_CONF_VM_CONTROL_SIZE=131072 \
	-DOSE_CONF_VM_DUMP_SIZE=131072 \
	-DOSE_CONF_VM_OUTPUT_SIZE=131072 \
	$(REPL_CFILES) -ledit -ldl

OSEC_CFILES=$(CORE_CFILES) $(SYS_CFILES) $(VM_CFILES) $(LANG_CFILES) osec.c
OSEC_HFILES=$(CORE_HFILES) $(SYS_HFILES) $(VM_HFILES) $(LANG_HFILES)
osec: CC=clang
osec: CFLAGS=$(CFLAGS_DEBUG)
osec: HOOKS=-DOSEVM_ASSIGN=osec_assign \
-DOSEVM_LOOKUP=osec_lookup \
-DOSEVM_FUNCALL=osec_funcall \
-DOSEVM_QUOTE=osec_quote \
-DOSEVM_COPYCONTEXTBUNDLE=osec_copyContextBundle \
-DOSEVM_APPENDTOCONTEXTBUNDLE=osec_appendToContextBundle \
-DOSEVM_REPLACECONTEXTBUNDLE=osec_replaceContextBundle \
-DOSEVM_MOVEELEMTOCONTEXTBUNDLE=osec_moveElemToContextBundle \
-DOSEVM_EXEC=osec_exec \
-DOSEVM_EXECINCURRENTCONTEXT=osec_execInCurrentContext \
-DOSEVM_APPLY=osec_apply \
-DOSEVM_RETURN=osec_return \
-DOSEVM_DEFUN=osec_defun \
-DOSEVM_ENDDEFUN=osec_endDefun \
-DOSEVM_TOINT32=osec_toInt32 \
-DOSEVM_TOFLOAT=osec_toFloat \
-DOSEVM_TOSTRING=osec_toString \
-DOSEVM_TOBLOB=osec_toBlob \
-DOSEVM_APPENDBYTE=osec_appendByte \
-DOSEVM_DEFAULT=osec_default \
-DOSEVM_PREINPUT=osec_preInput \
-DOSEVM_POSTINPUT=osec_postInput \
-DOSEVM_PRECONTROL=osec_preControl \
-DOSEVM_POSTCONTROL=osec_postControl
osec: $(OSEC_CFILES) $(OSEC_HFILES) 
	$(CC) $(CFLAGS) -o osec \
	$(HOOKS) \
	-DOSE_CONF_SYMTAB_FNSYMS \
	$(OSEC_CFILES)

JS_CFILES=$(CORE_CFILES) $(VM_CFILES) $(LANG_CFILES) js/osejs.c
JS_HFILES=$(CORE_HFILES) $(VM_HFILES) $(LANG_HFILES)
EMSCRIPTEN_EXPORTED_FUNCTIONS=$(shell cat js/osejs_export.mk)
js/libose.js: CC=emcc
js/libose.js: CFLAGS=-I. -O3 -s MALLOC="emmalloc" -s EXPORTED_FUNCTIONS=$(EMSCRIPTEN_EXPORTED_FUNCTIONS) -s EXTRA_EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' -g0 -s 'EXPORT_NAME="libose"'
js/libose.js: $(JS_CFILES) $(JS_HFILES) js/osejs_export.mk js/ose.js js/osevm.js
	$(CC) $(CFLAGS) -o js/libose.js \
	$(JS_CFILES)

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
	rm -rf ose *.dSYM $(TESTDIR)/*.dSYM $(TESTS) docs js/libose.js js/libose.wasm osec
