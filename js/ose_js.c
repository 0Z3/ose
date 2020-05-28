#include "ose_conf.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "ose.h"
#include "ose_context.h"
#include "ose_util.h"
#include "ose_stackops.h"
#include "ose_assert.h"
#include "ose_vm.h"
#include "ose_parse.h"
#include "ose_symtab.h"
#include "ose_print.h"

ose_bundle makebundle(int ac, char **av)
{
	char *bytes = (char *)malloc(1000000);
	ose_bundle bundle = ose_newBundleFromCBytes(1000000, bytes);
	return bundle;
}

void pprintBundle(ose_bundle bundle)
{
	char buf[8192];
	ose_pprintBundle(bundle, buf, 8192);
	printf("%s\n", buf);
}
