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

#include <emscripten.h>

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
#include "ose_print.h"

EMSCRIPTEN_KEEPALIVE
char* osejs_popString(ose_bundle bundle)
{
	const char * const str = ose_peekString(bundle);
	const int32_t len = strlen(str);
	char *copy = (char *)malloc(len + 1);
	memcpy(copy, str, len + 1);
	ose_pop(bundle);
	ose_drop(bundle);
	return copy;
}

EMSCRIPTEN_KEEPALIVE
char* osejs_popBlob(ose_bundle bundle)
{
	const char * const blob = ose_peekBlob(bundle);
	const int32_t len = ose_ntohl(*((int32_t *)blob));
	char *copy = (char *)malloc(len);
	memcpy(copy, blob + 4, len);
	ose_pop(bundle);
	ose_drop(bundle);
	return copy;
}

EMSCRIPTEN_KEEPALIVE
char *osejs_pprintBundle(ose_bundle bundle)
{
	int32_t len = ose_pprintBundle(bundle, NULL, 0);
	char *buf = (char *)malloc(len + 1);
	ose_pprintBundle(bundle, buf, len + 1);
	return buf;
}

EMSCRIPTEN_KEEPALIVE
ose_bundle osejs_newBundle(int32_t size)
{
	char *bytes = (char *)calloc(1, size);
	ose_assert(bytes);
	ose_bundle bundle = ose_newBundleFromCBytes(size, bytes);
	return bundle;
}


int main()
{
	//fprintf(stdout, "ose.js loaded\n");
	return 0;
}
