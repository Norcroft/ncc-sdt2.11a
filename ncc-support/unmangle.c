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

#include "unmangle.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>

// This is a quick mostly-AI-written attempt to match my summary (the comments)
// of what overload.c seems to do in its mangler: type_signature().

// It recognises names like:  f__1TFi      -> T::f(int)
// and more generally:        name__Q2_3Foo3BarFiPc  -> Foo::Bar::name(int, char*)
//
// If parsing fails, we return the original name pointer.

//   [C][V]F    : Function - qualifiers before ("f__CFv becomes int f() const;")

// Arrays, pointers, refs...:
//   An_        : array, n entries - if negative size(?!) replace '-' with 'n'
//   [C][V]P    : pointer (optional const/volatile)
//   [C][V]MQn  : pointer to member fn (M1A for A::*)
//   R          : reference '&'

// Qualifiers for base types:
//   C : const
//   S : signed (not ommitted for 'int', but is for 'char')
//   U : unsigned
//   V : volatile


// Nested struct, class, union or enum (up to 10):
//   Qn

// No support for 'T' and 'N' type reduction.

// Used as a temporary buffer internally - despite the name, nothing to do with
// CFront's constant.
#define MAXDBUF 256

static const char *base_type(int c) {
    switch (c) {
        case 'b': return "bool";
        case 'c': return "char";

        case 'x': return "long long";
        case 'l': return "long";
        case 'i': return "int";
        case 's': return "short";

        case 'f': return "float";
        case 'd': return "double";
        case 'r': return "long double";

        case 'v': return "void";

        case 'e': return "...";

        // case 'w': return "wchar_t"; // not yet used

        default:  return NULL;
    }
}

typedef struct { int is_const, is_volatile, is_unsigned, is_signed; } Quals;

static void out_init(char **p, size_t *rem, char *buf, size_t bufsz) {
    if (bufsz == 0) { *p = NULL; *rem = 0; return; }
    *p = buf; *rem = bufsz; **p = '\0';
}
static void out_ch(char **p, size_t *rem, char c) {
    if (*rem <= 1) return; **p = c; (*p)++; **p = '\0'; (*rem)--;
}
static void out_s(char **p, size_t *rem, const char *s) {
    while (*s) { out_ch(p, rem, *s++); if (*rem == 0) break; }
}
static void out_sn(char **p, size_t *rem, const char *s, size_t n) {
    while (n--) { out_ch(p, rem, *s++); if (*rem == 0) break; }
}


static int read_len(const char **pp) {
    const char *start = *pp; int val = 0; int neg = 0;
    if (*start == 'n') { neg = 1; start++; }
    if (!isdigit((unsigned char)*start)) return -1;
    while (isdigit((unsigned char)*start)) { val = val*10 + (*start - '0'); start++; }
    *pp = start; return neg ? -val : val;
}

static int decode_qualifiers(const char **pp, char **out, size_t *rem) {
    // Returns number of components written (for class qualification), 0 if none.
    const char *p = *pp; int wrote = 0;
    if (*p == 'Q') {
        int n, i;
        // Q<n> then n times: <len><name>
        ++p;

        n = read_len(&p); if (n <= 0) { *pp = p; return 0; }
        for (i = 0; i < n; ++i) {
            int l = read_len(&p); if (l <= 0) { *pp = p; return wrote; }
            if (wrote++) out_s(out, rem, "::");
            out_sn(out, rem, p, (size_t)l); p += l;
        }
        *pp = p; return wrote;
    }
    // Single level: <len><name>
    if (isdigit((unsigned char)*p)) {
        int l = read_len(&p); if (l > 0) {
            out_sn(out, rem, p, (size_t)l); p += l; wrote = 1; *pp = p; return wrote;
        }
    }
    return 0;
}

static void decode_arglist(const char **pp, char **out, size_t *rem) {
    const char *p = *pp;
    int first = 1;
    int is_memberptr = 0; // 1 if we saw 'M'
    char member_class[MAXDBUF];
    const char *bt;

    out_ch(out, rem, '(');
    if (*p == 'v') { // void
        ++p; out_s(out, rem, "void"); out_ch(out, rem, ')'); *pp = p; return; }
    while (*p && *p != '_' && *p != '\0') {
        int ptrs = 0, refs = 0, array_len = -1;
        Quals q = {0,0,0,0};

        // Skip cfront/Norcroft type-reduction markers: Tn or N<m><w>
        if (*p == 'T' || *p == 'N') {
            ++p; (void)read_len(&p); if (*(p-1) == 'N') { (void)read_len(&p); }
            continue;
        }
        if (*p == 'X') { // value signature until 'Y'
            ++p; while (*p && *p != 'Y') ++p; if (*p == 'Y') ++p; continue;
        }
        member_class[0] = '\0';

        if (!first)
            out_s(out, rem, ", ");
        first = 0;

        // Parse qualifiers: C/V/U/S
        while (*p == 'C' || *p == 'V' || *p == 'U' || *p == 'S') {
            if (*p == 'C') q.is_const = 1;
            else if (*p == 'V') q.is_volatile = 1;
            else if (*p == 'U') q.is_unsigned = 1;
            else if (*p == 'S') q.is_signed = 1;
            ++p;
        }

        // Array
        if (*p == 'A') {
            ++p;
            array_len = read_len(&p);
            if (*p == '_') ++p; // consume the terminator
        }

        // Pointer-to-member
        if (*p == 'M') {
            char *tmpo; size_t tmprem;

            ++p;
            // decode the class qualification that follows
            out_init(&tmpo, &tmprem, member_class, sizeof(member_class));
            (void)decode_qualifiers(&p, &tmpo, &tmprem);
            is_memberptr = 1;
        }

        // Pointers
        while (*p == 'P') { ++ptrs; ++p; }
        // Refs
        while (*p == 'R') { ++refs; ++p; }

        bt = base_type((unsigned char)*p);
        if (bt) {
            // prepend unsigned/signed where meaningful
            if (q.is_unsigned) out_s(out, rem, "unsigned ");
            else if (q.is_signed && *p == 'c') out_s(out, rem, "signed ");
            out_s(out, rem, bt);
            ++p;
        } else if (isdigit((unsigned char)*p) || *p == 'Q') {
            if (q.is_unsigned) out_s(out, rem, "unsigned ");
            else if (q.is_signed) out_s(out, rem, "signed ");
            (void)decode_qualifiers(&p, out, rem);
        } else {
            out_s(out, rem, "?"); if (*p) ++p;
        }
        if (q.is_const) out_s(out, rem, " const");
        if (q.is_volatile) out_s(out, rem, " volatile");
        while (refs--) out_ch(out, rem, '&');
        if (is_memberptr) { out_ch(out, rem, ' '); out_s(out, rem, member_class); out_s(out, rem, "::*"); }
        else { while (ptrs--) out_ch(out, rem, '*'); }

        if (array_len >= 0) {
            char tmp[32];
            out_ch(out, rem, '[');
#ifdef __riscos
            sprintf(tmp, "%d", array_len); // no snprintf() on older RISC OS.
#else
            snprintf(tmp, sizeof(tmp), "%d", array_len);
#endif
            out_s(out, rem, tmp);
            out_ch(out, rem, ']');
        }

        if (*p == 'e') { // variadic marker
            ++p; out_s(out, rem, ", ..."); break;
        }
    }
    out_ch(out, rem, ')');
    *pp = p;
}

const char* unmangle2(const char* name, char *buf, size_t size)
{
    const char *sep;
    char *out;
    const char* p;
    size_t rem, before;
    int had_class;
    int fn_is_const = 0, fn_is_volatile = 0;

    if (!name || !buf || size == 0) return name;
    sep = strstr(name, "__");
    if (!sep) return name; // not our scheme

    out_init(&out, &rem, buf, size);

    // base name up to "__"
    out_sn(&out, &rem, name, (size_t)(sep - name));

    p = sep + 2;

    // optional class qualification
    before = (size_t)(out - buf);
    had_class = decode_qualifiers(&p, &out, &rem);
    if (had_class) { // insert scope
        const char *base;
        // Move base name after qualifiers as Class::name
        // We built as "nameClass::..."? Instead, rewrite by constructing anew.
        char saved[MAXDBUF]; strncpy(saved, buf, sizeof(saved)); saved[sizeof(saved)-1] = '\0';
        out_init(&out, &rem, buf, size);
        // qualifiers were written at start from 'before' offset; rebuild properly:
        // Decode again into fresh buffer to get qualifiers at front
        p = sep + 2; decode_qualifiers(&p, &out, &rem); out_s(&out, &rem, "::");
        // append base
        base = name; out_sn(&out, &rem, base, (size_t)(sep - base));
    }

    // function-level qualifiers before 'F'
    while (*p == 'C' || *p == 'V') {
        if (*p == 'C') fn_is_const = 1; else fn_is_volatile = 1;
        ++p;
    }

    // function signature
    if (*p == 'F') {
        ++p; decode_arglist(&p, &out, &rem);
        if (fn_is_const) out_s(&out, &rem, " const");
        if (fn_is_volatile) out_s(&out, &rem, " volatile");
    }

    return buf;
}

const char* unmangle_class(const char* name, char *buf, size_t size)
{
    const char *sep, *p;
    char *out;
    size_t rem;
    int had;

    if (!name || !buf || size == 0) return name;

    sep = strstr(name, "__");

    if (!sep) return name;

    p = sep + 2;

    out_init(&out, &rem, buf, size);
    had = decode_qualifiers(&p, &out, &rem);
    return had ? buf : name;
}

const char* unmangle_with_class(const char* name,
                                const char* class_name,
                                size_t class_len,
                                char *buf,
                                size_t size)
{
    const char *demangled;
    const char *search_base;
    const char *found;
    char pat[128];
    size_t plen;
    char tmp[MAXDBUF];
    char *out;
    size_t rem;

    if (!name || !buf || size == 0)
        return name;

    /* First, try to demangle normally. This writes into buf. */
    demangled = unmangle2(name, buf, size);

    /* If there is no class context, just return whatever unmangle2 gave us. */
    if (class_name == NULL || class_len == 0 || *class_name == '\0')
        return demangled;

    /* We’ll search for "<class_name>::" inside the demangled string. */
    plen = class_len;
    if (plen > sizeof(pat) - 3)
        plen = sizeof(pat) - 3;   /* truncate rather than overflow */

    memcpy(pat, class_name, plen);
    pat[plen]     = ':';
    pat[plen + 1] = ':';
    pat[plen + 2] = '\0';

    search_base = demangled;

    /* If unmangling failed, demangled may just be 'name', but that’s fine. */
    found = strstr(search_base, pat);
    if (found != NULL) {
        /*
         * Strip everything before the class name so we end up with
         * "Class::member(args)", which will be prefixed by outer scopes
         * from printparents().
         */
        return found;
    }

    /*
     * If we didn’t find "<class_name>::" in the demangled form, build
     * "Class::demangled" (or "Class::name" if unmangling failed) in buf.
     */

    /* Save the current demangled (or original) string, then rebuild buf. */
    strncpy(tmp, search_base, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    out_init(&out, &rem, buf, size);
    out_sn(&out, &rem, class_name, class_len);
    out_s(&out, &rem, "::");
    out_s(&out, &rem, tmp);

    return buf;
}
