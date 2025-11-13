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

#include "int64.h"
#include "ncc-types.h"

#include <limits.h>
#include <stdint.h>

// Assumes host supports int64_t and uint64_t.

static inline uint64_t u64_from_struct(const uint64 *a)
{
    return ((uint64_t)a->hi << 32) | (uint64_t)a->lo;
}

static inline int64_t s64_from_struct(const int64 *a)
{
    return ((int64_t)a->hi << 32) | (uint64_t)a->lo;
}

static inline void u64_to_struct(uint64 *dst, uint64_t v)
{
    dst->lo = (uint32)v;
    dst->hi = (uint32)(v >> 32);
}

static inline void s64_to_struct(int64 *dst, int64_t v)
{
    dst->lo = (uint32)((uint64_t)v);
    dst->hi = (int32)(v >> 32);
}

// Conversions to 32-bit values from 64-bit structs.
I64_Status I64_SToI(int32 *res, int64 *i)
{
    // Value fits in 32-bit signed iff:
    //   hi == 0 and top bit of lo == 0, or hi == -1 and top bit of lo == 1.
    uint32 lo = i->lo;
    int32  hi = i->hi;
    if ((hi == 0  && (lo & 0x80000000u) == 0) ||
        (hi == -1 && (lo & 0x80000000u) != 0))
    {
        *res = (int32)lo;
        return i64_ok;
    }

    *res = (int32)lo;  // truncate like C.
    return i64_overflow;
}

I64_Status I64_UToI(uint32 *res, uint64 *i)
{
    if (i->hi == 0) {
        *res = i->lo;
        return i64_ok;
    }

    *res = i->lo; // truncate
    return i64_overflow;
}

// Conversions to 64-bit structs from 32-bit values.
I64_Status I64_IToS(int64 *res, int32 n)
{
    res->lo = (uint32)n;         // copy the bits
    res->hi = (n < 0) ? -1 : 0;  // fill high word with the sign
    return i64_ok;
}

I64_Status I64_IToU(uint64 *res, int32 n)
{
    res->lo = (uint32)n;
    res->hi = 0; // zero-extend
    return i64_ok;
}

// Unsigned arithmetic
I64_Status I64_UAdd(uint64 *res, uint64 *a1, uint64 *a2)
{
    uint64_t a = u64_from_struct(a1);
    uint64_t b = u64_from_struct(a2);
    uint64_t r = a + b;
    u64_to_struct(res, r);

    return (r < a) ? i64_overflow : i64_ok;
}

I64_Status I64_USub(uint64 *res, uint64 *a1, uint64 *a2)
{
    uint64_t a = u64_from_struct(a1);
    uint64_t b = u64_from_struct(a2);
    u64_to_struct(res, a - b);

    return (a < b) ? i64_overflow : i64_ok; // underflow
}

// Ugly, to work around the CPP not supporting defined()
#ifdef __CC_NORCROFT
#  define NO_BUILTIN_MUL_OVERFLOW
#elif !defined(__has_builtin) || !__has_builtin(__builtin_mul_overflow)
#  define NO_BUILTIN_MUL_OVERFLOW
#endif
I64_Status I64_UMul(uint64 *res, uint64 *a1, uint64 *a2)
{
    uint64_t a = u64_from_struct(a1);
    uint64_t b = u64_from_struct(a2);

#ifndef NO_BUILTIN_MUL_OVERFLOW
    uint64_t r;
    if (__builtin_mul_overflow((uint64_t)a, (uint64_t)b, &r)) {
        u64_to_struct(res, r);
        return i64_overflow;
    }
    u64_to_struct(res, r);
    return i64_ok;
#else
    if (a == 0 || b == 0) {
        u64_to_struct(res, 0);
        return i64_ok;
    }

    u64_to_struct(res, a * b);

    // overflow iff a != 0 (already checked) and b > UINT64_MAX / a
    if (b > UINT64_MAX / a)
        return i64_overflow;

    return i64_ok;
#endif
}

I64_Status I64_UDiv(uint64 *res, uint64 *rem, uint64 const *num, uint64 const *den)
{
    uint64_t N = u64_from_struct(num);
    uint64_t D = u64_from_struct(den);
    if (D == 0)
        return i64_divzero;

    u64_to_struct(res, N / D);
    u64_to_struct(rem, N % D);
    return i64_ok;
}

/* Signed arithmetic */
#ifdef __CC_NORCROFT
#  define NO_BUILTIN_ADD_OVERFLOW
#elif !defined(__has_builtin) || !__has_builtin(__builtin_add_overflow)
#  define NO_BUILTIN_ADD_OVERFLOW
#endif
I64_Status I64_SAdd(int64 *res, int64 const *a1, int64 const *a2)
{
    int64_t a = s64_from_struct(a1);
    int64_t b = s64_from_struct(a2);

#ifndef NO_BUILTIN_ADD_OVERFLOW
    int64_t r;
    if (__builtin_add_overflow(a, b, &r)) {
        s64_to_struct(res, r);
        return i64_overflow;
    }

    s64_to_struct(res, r);
    return i64_ok;
#else
    int64_t r = a + b;
    s64_to_struct(res, r);
    if ((b > 0 && r < a) || (b < 0 && r > a))
        return i64_overflow;
    return i64_ok;
#endif
}

#ifdef __CC_NORCROFT
#  define NO_BUILTIN_SUB_OVERFLOW
#elif !defined(__has_builtin) || !__has_builtin(__builtin_sub_overflow)
#  define NO_BUILTIN_SUB_OVERFLOW
#endif
I64_Status I64_SSub(int64 *res, int64 const *a1, int64 const *a2)
{
    int64_t a = s64_from_struct(a1);
    int64_t b = s64_from_struct(a2);

#ifndef NO_BUILTIN_SUB_OVERFLOW
    int64_t r;
    if (__builtin_sub_overflow(a, b, &r)) {
        s64_to_struct(res, r);
        return i64_overflow;
    }

    s64_to_struct(res, r);
    return i64_ok;
#else
    int64_t r = a - b;
    s64_to_struct(res, r);
    if ((b < 0 && r < a) || (b > 0 && r > a))
        return i64_overflow;

    return i64_ok;
#endif
}

I64_Status I64_SMul(int64 *res, int64 const *a1, int64 const *a2)
{
    int64_t a = s64_from_struct(a1);
    int64_t b = s64_from_struct(a2);

#ifndef NO_BUILTIN_MUL_OVERFLOW
    int64_t r;
    if (__builtin_mul_overflow(a, b, &r)) {
        s64_to_struct(res, r);
        return i64_overflow;
    }
    s64_to_struct(res, r);

    return i64_ok;
#else
    bool of;

    if (a == 0 || b == 0) {
        s64_to_struct(res, 0);
        return i64_ok;
    }

    s64_to_struct(res, a * b);

    if ((a == INT64_MIN && b == -1) || (b == INT64_MIN && a == -1))
        return i64_overflow;

    of = false;
    if (a > 0) {
        if (b > 0)
            of = (b > INT64_MAX / a);
        else if (b < 0)
            of = (b < INT64_MIN / a);
    } else if (a < 0) {
        if (b > 0)
            of = (a < INT64_MIN / b);
        else if (b < 0)
            of = (a < INT64_MAX / b);
    }
    if (of)
        return i64_overflow;

    return i64_ok;
#endif
}

I64_Status I64_SDiv(int64 *res, int64 *rem, int64 const *num, int64 const *den)
{
    int64_t n = s64_from_struct(num);
    int64_t d = s64_from_struct(den);
    if (d == 0) {
        s64_to_struct(res, 0);
        s64_to_struct(rem, n);
        return i64_divzero;
    }

    if (n == INT64_MIN && d == -1) {
        s64_to_struct(res, n);
        s64_to_struct(rem, 0);
        return i64_overflow;
    }

    s64_to_struct(res, n /d);
    s64_to_struct(rem, n % d);

    return i64_ok;
}

// Bitwise operations
void I64_And(int64 *res, int64 const *a1, int64 const *a2)
{
    int64_t t1 = s64_from_struct(a1);
    int64_t t2 = s64_from_struct(a2);

    s64_to_struct(res, t1 & t2);
}

void I64_Or(int64 *res, int64 const *a1, int64 const *a2)
{
    int64_t t1 = s64_from_struct(a1);
    int64_t t2 = s64_from_struct(a2);

    s64_to_struct(res, t1 | t2);
}

void I64_Eor(int64 *res, int64 const *a1, int64 const *a2)
{
    int64_t t1 = s64_from_struct(a1);
    int64_t t2 = s64_from_struct(a2);

    s64_to_struct(res, t1 ^ t2);
}

void I64_Not(int64 *res, int64 const *a1)
{
    s64_to_struct(res, ~s64_from_struct(a1));
}

I64_Status I64_Neg(int64 *res, int64 const *a1)
{
    int64_t a = s64_from_struct(a1);
    if (a == INT64_MIN) {
        s64_to_struct(res, a);
        return i64_overflow;
    }
    s64_to_struct(res, -a);
    return i64_ok;
}

/* Shifts */
static inline unsigned shift_mask(unsigned s)
{
    return s & 63u;
}

I64_Status I64_Lsh(int64 *res, int64 const *a1, Uint a2)
{
    unsigned s = shift_mask(a2);
    uint64_t v = (uint64_t)s64_from_struct(a1);
    uint64_t r = v << s;
    u64_to_struct((uint64*)res, r);

    if (s == 0)
        return i64_ok;

    /* if any 1-bits are lost by the left shift, report overflow */
    return ((v >> (64 - s)) != 0) ? i64_overflow : i64_ok;
}

I64_Status I64_URsh(uint64 *res, uint64 const *a1, Uint a2)
{
    u64_to_struct(res, u64_from_struct(a1) >> shift_mask(a2));

    return i64_ok;
}

I64_Status I64_SRsh(int64 *res, int64 const *a1, Uint a2)
{
    s64_to_struct(res, s64_from_struct(a1) >> shift_mask(a2));

    return i64_ok;
}

/* Comparisons: return -1, 0, +1 */
int I64_UComp(uint64 *a, uint64 *b)
{
    uint64_t A = u64_from_struct(a);
    uint64_t B = u64_from_struct(b);

    if (A < B) return -1;
    if (A > B) return +1;
    
    return 0;
}

int I64_SComp(int64 *a, int64 *b)
{
    int64_t A = s64_from_struct(a);
    int64_t B = s64_from_struct(b);

    if (A < B) return -1;
    if (A > B) return +1;

    return 0;
}
