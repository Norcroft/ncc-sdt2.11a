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

// Bear in mind that the default settings for 64-bit ints will be very
// inefficient as these functions are all expected to follow APCS.
// There's a (default off) pragma to ignore APCS for these int64 helper
// functions that return their result in the CPU flags.

// It's not worth spending any optimising these for ARM since the future is
// to always use the 'result in flags' pragma (-zPx) on these functions.

// Generated entirely by AI. Not checked the output.

/* 64-bit helpers with only 32-bit arithmetic (Norcroft C-safe) */

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int  u32;
typedef   signed int  s32;

/* 64-bit view with 32-bit halves, plus a 64-bit alias for ABI I/O only */
typedef union {
    struct { u32 lo, hi; } w;           /* unsigned view */
    struct { u32 lo; s32 hi; } sw;      /* signed-hi view */
    unsigned long long v;               /* ABI only: never used in exprs */
    long long          sv;              /* ABI only: signed return when needed */
} u64u;

/* ---------- helpers (32-bit only) ---------- */

static int u64_ucmp(u32 ahi, u32 alo, u32 bhi, u32 blo)
{
    if (ahi != bhi) return (ahi > bhi) ? 1 : -1;
    if (alo != blo) return (alo > blo) ? 1 : -1;
    return 0;
}

static int u64_scmp(s32 ahi, u32 alo, s32 bhi, u32 blo)
{
    if (ahi != bhi) return (ahi > bhi) ? 1 : -1;   /* signed compare on hi */
    if (alo != blo) return (alo > blo) ? 1 : -1;
    return 0;
}

/* 32x32 -> 64 (hi:lo) using 16-bit pieces, no 64-bit temps */
static void mul32x32_64(u32 x, u32 y, u32 *hi, u32 *lo)
{
    u32 x0, x1, y0, y1;
    u32 p00, p01, p10, p11;
    u32 mid, hi32;

    x0 = x & 0xFFFFu; x1 = x >> 16;
    y0 = y & 0xFFFFu; y1 = y >> 16;

    p00 = x0 * y0;
    p01 = x0 * y1;
    p10 = x1 * y0;
    p11 = x1 * y1;

    /* low 32 */
    mid = (p00 >> 16) + (p01 & 0xFFFFu) + (p10 & 0xFFFFu);
    *lo = (p00 & 0xFFFFu) | (mid << 16);

    /* high 32 */
    hi32 = p11 + (p01 >> 16) + (p10 >> 16) + (mid >> 16);
    *hi = hi32;
}

/* ---------- comparisons ---------- */

int _ll_cmpeq(long long a, long long b)
{
    u64u ua, ub;
    ua.v = (unsigned long long)a;
    ub.v = (unsigned long long)b;
    return u64_ucmp(ua.w.hi, ua.w.lo, ub.w.hi, ub.w.lo) == 0;
}

int _ll_cmpne(long long a, long long b)
{
    u64u ua, ub;
    ua.v = (unsigned long long)a;
    ub.v = (unsigned long long)b;
    return u64_ucmp(ua.w.hi, ua.w.lo, ub.w.hi, ub.w.lo) != 0;
}

int _ll_scmpgt(long long a, long long b)
{
    u64u ua, ub;
    ua.v = (unsigned long long)a;
    ub.v = (unsigned long long)b;
    return u64_scmp(ua.sw.hi, ua.sw.lo, ub.sw.hi, ub.sw.lo) > 0;
}

int _ll_scmplt(long long a, long long b)
{
    u64u ua, ub;
    ua.v = (unsigned long long)a;
    ub.v = (unsigned long long)b;
    return u64_scmp(ua.sw.hi, ua.sw.lo, ub.sw.hi, ub.sw.lo) < 0;
}

int _ll_ucmpgt(unsigned long long a, unsigned long long b)
{
    u64u ua, ub;
    ua.v = a; ub.v = b;
    return u64_ucmp(ua.w.hi, ua.w.lo, ub.w.hi, ub.w.lo) > 0;
}

int _ll_ucmplt(unsigned long long a, unsigned long long b)
{
    u64u ua, ub;
    ua.v = a; ub.v = b;
    return u64_ucmp(ua.w.hi, ua.w.lo, ub.w.hi, ub.w.lo) < 0;
}

/* ---------- shifts ---------- */

unsigned long long _ll_shift_l(unsigned long long x, int sh)
{
    u64u a, r;
    u32 nhi, nlo;

    a.v = x;
    if (sh <= 0) return x;
    if (sh >= 64) { r.w.lo = 0; r.w.hi = 0; return r.v; }

    if (sh >= 32) {
        nhi = a.w.lo << (sh - 32);
        nlo = 0;
    } else {
        nhi = (a.w.hi << sh) | (a.w.lo >> (32 - sh));
        nlo = a.w.lo << sh;
    }
    r.w.lo = nlo; r.w.hi = nhi;
    return r.v;
}

unsigned long long _ll_ushift_r(unsigned long long x, int sh)
{
    u64u a, r;
    u32 nhi, nlo;

    a.v = x;
    if (sh <= 0) return x;
    if (sh >= 64) { r.w.lo = 0; r.w.hi = 0; return r.v; }

    if (sh >= 32) {
        nlo = a.w.hi >> (sh - 32);
        nhi = 0;
    } else {
        nlo = (a.w.lo >> sh) | (a.w.hi << (32 - sh));
        nhi = a.w.hi >> sh;
    }
    r.w.lo = nlo; r.w.hi = nhi;
    return r.v;
}

long long _ll_sshift_r(long long x, int sh)
{
    u64u a, r;
    u32 nlo;
    s32 nhi;
    int neg;

    a.v = (unsigned long long)x;
    if (sh <= 0) return x;
    if (sh >= 64) {
        neg = (a.sw.hi < 0);
        r.sw.hi = neg ? -1 : 0;
        r.sw.lo = (u32)r.sw.hi;
        return r.sv;
    }

    if (sh >= 32) {
        nlo = (u32)((u32)a.sw.hi >> (sh - 32));      /* logical part; sign in hi */
        nhi = (a.sw.hi < 0) ? -1 : 0;
    } else {
        nlo = (a.sw.lo >> sh) | ((u32)a.sw.hi << (32 - sh));
        nhi = a.sw.hi >> sh;                         /* arithmetic */
    }
    r.sw.lo = nlo; r.sw.hi = nhi;
    return r.sv;
}

/* ---------- multiply: low 64 bits of 64x64 ---------- */

unsigned long long _ll_mul(unsigned long long x, unsigned long long y)
{
    u64u a, b, out;
    u32 ac_hi, ac_lo;
    u32 ad_lo, bc_lo;
    u32 hi, s;
    u32 c1;

    a.v = x; b.v = y;

    /* ac = a.lo * b.lo */
    mul32x32_64(a.w.lo, b.w.lo, &ac_hi, &ac_lo);

    /* ad_lo = low32(a.lo * b.hi) */
    {
        u32 t_hi, t_lo;
        mul32x32_64(a.w.lo, b.w.hi, &t_hi, &t_lo);
        ad_lo = t_lo;
    }

    /* bc_lo = low32(a.hi * b.lo) */
    {
        u32 t_hi, t_lo;
        mul32x32_64(a.w.hi, b.w.lo, &t_hi, &t_lo);
        bc_lo = t_lo;
    }

    /* low64(x*y):
     lo = ac_lo
     hi = ac_hi + ad_lo + bc_lo   (overflow beyond 32 is discarded)
     */
    out.w.lo = ac_lo;

    hi  = ac_hi;
    s   = hi + ad_lo; c1 = (s < hi);
    hi  = s + bc_lo;  (void)c1;  /* carry into bit64 is discarded */

    out.w.hi = hi;

    return out.v;
}

/* ---------- division: classic restoring (unsigned), then signed wrapper ---------- */

unsigned long long _ll_udiv(unsigned long long n, unsigned long long d)
{
    u64u N, D, Q, R;
    int i;
    u32 newRhi, newRlo, qcarry;
    int ge;
    u32 old_lo;

    N.v = n; D.v = d;
    Q.w.lo = 0; Q.w.hi = 0;
    R.w.lo = 0; R.w.hi = 0;

    /* quick outs without 64-bit compares */
    if (D.w.hi == 0 && D.w.lo == 0) return ~0u;                /* /0 -> all-ones */
    if (u64_ucmp(N.w.hi, N.w.lo, D.w.hi, D.w.lo) < 0) return 0;
    if (D.w.hi == 0 && D.w.lo == 1) return n;

    for (i = 0; i < 64; ++i) {
        /* R <<= 1, bring down msb of N */
        newRhi = (R.w.hi << 1) | (R.w.lo >> 31);
        newRlo = (R.w.lo << 1) | ((N.w.hi & 0x80000000u) ? 1u : 0u);
        R.w.hi = newRhi; R.w.lo = newRlo;

        /* N <<= 1 (for feeding next bit) */
        N.w.hi = (N.w.hi << 1) | (N.w.lo >> 31);
        N.w.lo <<= 1;

        ge = (R.w.hi > D.w.hi) || (R.w.hi == D.w.hi && R.w.lo >= D.w.lo);

        qcarry = (Q.w.lo >> 31);
        Q.w.lo = (Q.w.lo << 1) | (ge ? 1u : 0u);
        Q.w.hi = (Q.w.hi << 1) | qcarry;

        if (ge) {
            old_lo = R.w.lo;
            R.w.lo -= D.w.lo;
            R.w.hi = R.w.hi - D.w.hi - (R.w.lo > old_lo ? 1u : 0u);
        }
    }
    return Q.v;
}

long long _ll_sdiv(long long n, long long d)
{
    u64u un, ud, uq, out;
    int neg;

    /* handle signs in 32-bit pieces */
    neg = 0;

    if (d == 0) {
        /* crude: saturate to +all-ones / -1 depending on n sign */
        return (n < 0) ? 1 : (long long)~0u;
    }

    un.v = (unsigned long long)n;
    ud.v = (unsigned long long)d;

    if (un.sw.hi < 0) { /* n < 0 */
        /* un = -n (64-bit) using 32-bit negation */
        u32 lo = ~un.sw.lo + 1u;
        u32 hi = ~((u32)un.sw.hi) + (lo == 0 ? 1u : 0u);
        un.w.lo = lo; un.w.hi = hi;
        neg ^= 1;
    }
    if (ud.sw.hi < 0) { /* d < 0 */
        u32 lo = ~ud.sw.lo + 1u;
        u32 hi = ~((u32)ud.sw.hi) + (lo == 0 ? 1u : 0u);
        ud.w.lo = lo; ud.w.hi = hi;
        neg ^= 1;
    }

    uq.v = _ll_udiv(un.v, ud.v);

    if (neg) {
        /* out = -uq */
        u32 lo = ~uq.w.lo + 1u;
        u32 hi = ~uq.w.hi + (lo == 0 ? 1u : 0u);
        out.w.lo = lo; out.w.hi = hi;
        return out.sv;
    }
    return uq.sv;
}
