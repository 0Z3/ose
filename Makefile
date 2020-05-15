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

MIN_CFILES=ose.c ose_util.c ose_stackops.c ose_context.c ose_match.c ose_assert.c
MIN_HFILES=ose.h ose_conf.h ose_util.h ose_stackops.h ose_context.h ose_match.h ose_assert.h
SYS_CFILES=ose_import.c
SYS_HFILES=ose_import.h
VM_CFILES=ose_symtab.c ose_builtins.c ose_vm.c
VM_HFILES=ose_symtab.h ose_builtins.h ose_vm.h
LANG_CFILES=ose_lex.c ose_parse.c ose_print.c
LANG_HFILES=ose_lex.h ose_parse.h ose_print.h
TEST_CFILES=ut_ose_util.c ut_ose_stackops.c
TEST_HFILES=ut_ose_util.h ut_ose_stackops.h


CFLAGS_RELEASE=-O3 -rdynamic
CFLAGS_DEBUG=-DOSE_CONF_DEBUG -O0 -glldb -fsanitize=undefined -rdynamic

ose_symtab.c: ose_symtab.gperf
	gperf ose_symtab.gperf > ose_symtab.c

REPL_CFILES=$(MIN_CFILES) $(SYS_CFILES) $(VM_CFILES) $(LANG_CFILES)
REPL_HFILES=$(MIN_HFILES) $(SYS_HFILES) $(VM_HFILES) $(LANG_HFILES)

ose: CFLAGS=$(CFLAGS_RELEASE)
ose: $(REPL_CFILES) $(REPL_HFILES) ose_repl.c
	clang $(CFLAGS) -o ose \
	-DOSE_CONF_VM_SIZE=1000000 \
	$(REPL_CFILES) ose_repl.c -ledit

debug: CFLAGS=$(CFLAGS_DEBUG)
debug: $(REPL_CFILES) $(REPL_HFILES) ose_repl.c
	clang $(CFLAGS) -o ose \
	-DOSE_CONF_VM_SIZE=1000000 \
	$(REPL_CFILES) ose_repl.c -ledit

min:
	echo "not implemented yet"

$(TESTDIR)/%: CFLAGS=$(CFLAGS_DEBUG)
$(TESTDIR)/%: $(MIN_CFILES) $(MIN_HFILES) $(TESTDIR)/%.c $(TESTDIR)/common.h
	clang $(CFLAGS) $(ALLTYPES) -o $@ \
	$(MIN_CFILES) ose_print.c $@.c

.PHONY: test
test: $(TESTS)

.PHONY: clean
clean:
	rm -rf ose *.dSYM $(TESTDIR)/*.dSYM $(TESTS) docs
