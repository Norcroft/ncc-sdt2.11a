/* Copyright 2025 Piers Wombwell
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include "host.h"

typedef union {
    uint32_t    val;

    float       f;
} FloatBin;

// Since ANSI C doesn't support anonymous structs DbleBin can't be a union yet.
typedef struct {
    uint32_t msd, lsd;
} DbleBin;

typedef union {
    struct {
    #if defined(__riscos) ^ defined(HOST_IS_BIG_ENDIAN)
        // Big endian host, or host with FPA.
        //
        // FPA stores the two words containing doubles the wrong way round,
        // at least when compared to VFP or mainstream CPUs.
        uint32_t msd, lsd;
    #else
        // Assume any non-FPA host stores doubles the sensible way round.
        uint32_t lsd, msd;
    #endif
    } b;

    double d;
} DblePun;

typedef enum {
    flt_ok = 0,         // exact, fine

    flt_bad,            // inexact / lossy but finite (e.g. dtoi truncation, parse fallback)
    flt_big_single,     // narrowing to float overflowed (finite double -> +inf/-inf as float)
    flt_small_single,   // narrowing to float underflowed/subnormal (became 0 or denorm)

    flt_very_big,       // result overflowed in double (±inf)
    flt_invalidop,      // invalid operation / NaN produced (domain error, 0/0, etc.)

    flt_inexact,
    flt_very_small
} flt_status;

// The following flt_* functions will generally match one of these function
// prototypes for the struct FPEvalFn in cseeval.c
//  int (*dunaryu)(DbleBin *res, uint32 n);
//  int (*dunaryi)(DbleBin *res, int32 n);
//  int (*unaryd)(DbleBin *res, DbleBin const *a1);
//  int (*binaryd)(DbleBin *res, DbleBin const *a1, DbleBin const *a2);
//  void (*fwiden)(FloatBin const *a1, DbleBin *res);

// I've had to guess the return values for various invalid values, though most
// functions just check for flt_ok.
extern int flt_add(DbleBin *res, DbleBin const *a1, DbleBin const *a2);
extern int flt_subtract(DbleBin *res, DbleBin const *a1, DbleBin const *a2);
extern int flt_multiply(DbleBin *res, DbleBin const *a1, DbleBin const *a2);
extern int flt_divide(DbleBin *res, DbleBin const *a1, DbleBin const *a2);

extern int flt_fmod(DbleBin *res, DbleBin const *a1, DbleBin const *a2);

extern int flt_invert(DbleBin *res, DbleBin const *a1);
extern int flt_negate(DbleBin *res, DbleBin const *a1);

// Currently, flt_compare() can return only -1, 0, or 1
extern int flt_compare(const DbleBin *a1, const DbleBin *a2);

extern int flt_dtoi(int32_t *res, const DbleBin *a1);
extern int flt_dtou(uint32_t *res, const DbleBin *a1);

extern int flt_itod(DbleBin *res, int32_t n);
extern int flt_utod(DbleBin *res, uint32_t n);

extern int flt_move(DbleBin *res, const DbleBin *a1);

extern int flt_abs(DbleBin *res, DbleBin const *a1);
extern int flt_floor(DbleBin *res, DbleBin const *a1);
extern int flt_ceil(DbleBin *res, DbleBin const *a1);

extern void fltrep_widen(FloatBin const *a, DbleBin *res);
extern int fltrep_narrow(DbleBin const *a, FloatBin *res);
extern int fltrep_narrow_round(DbleBin const *a, FloatBin *res);

extern int fltrep_stod(const char*, DbleBin *, void*);

extern void fltrep_sprintf(char *res, const char *fmt, const DbleBin *a);
