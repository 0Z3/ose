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

#include <inttypes.h>
#include <string.h>

#include "ose_assert.h"
#include "ose_time.h"

#if defined(__APPLE__)

ose_hptimer ose_initTimer(void)
{
	ose_hptimer t;
	kern_return_t mti_status = mach_timebase_info(&(t.tb));
	(void)mti_status;
	ose_assert(mti_status == KERN_SUCCESS);
	t.bias = mach_absolute_time();
	return t;
}

uint64_t ose_now(ose_hptimer t)
{
	return mach_absolute_time();
}

uint64_t ose_timeToMonotonicNanos(ose_hptimer t, uint64_t tt)
{
	uint32_t numer = t.tb.numer;
	uint32_t denom = t.tb.denom;
	uint64_t high = (tt >> 32) * numer;
	uint64_t low = (tt & 0xffffffffull) * numer / denom;
	uint64_t high_rem = ((high % denom) << 32) / denom;
	high /= denom;
	return (high << 32) + high_rem + low;
}

#elif defined(__unix__)
#include <unistd.h>
#ifdef _POSIX_VERSION
#include <time.h>

ose_hptimer ose_initTimer(void)
{
	ose_hptimer t;
	t.bias = ose_now(t);
	return t;
}

uint64_t ose_now(ose_hptimer t)
{
	struct timespec ts;
	memset(&ts, 0, sizeof(struct timespec));
	int r = clock_gettime(CLOCK_MONOTONIC, &ts);
	ose_assert(r == 0);
	uint64_t tt = (ts.tv_sec * 1e9 + ts.tv_nsec) - t.bias;
	return tt;
}

uint64_t ose_timeToMonotonicNanos(ose_hptimer t, uint64_t tt)
{
	return tt;
}

#endif // posix

#else

ose_hptimer ose_initTimer(void)
{
	ose_hptimer t;
	return t;
}

uint64_t ose_now(ose_hptimer t)
{
	return 0;
}

uint64_t ose_timeToMonotonicNanos(ose_hptimer t, uint64_t tt)
{
	return tt;
}

#endif
