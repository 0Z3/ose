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

#ifndef OSE_TIME_H
#define OSE_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#define OSE_HAVE_HPTIMER

#ifdef __APPLE__
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>
typedef struct ose_hptimer
{
	mach_timebase_info_data_t tb;
	uint64_t bias;
} ose_hptimer;

#elif defined(__unix__)
#include <unistd.h>
#ifdef _POSIX_VERSION

typedef struct ose_hptimer
{
	uint64_t bias;
} ose_hptimer;
#endif

#else

#undef OSE_HAVE_HPTIMER
typedef struct ose_hptimer {} ose_hptimer;

#endif

typedef struct ose_hptimer ose_hptimer;
ose_hptimer ose_initTimer(void);
uint64_t ose_now(ose_hptimer t);
uint64_t ose_timeToMonotonicNanos(ose_hptimer t, uint64_t tt);

#ifdef __cplusplus
}
#endif

#endif
