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

// Cfront-ish demangler for Norcroft
//
// This aims to make function names readable without completely accurately
// parsing all the cfront tables. Substantually modified (improved?) by AI.
//
// It understands common patterns like: name__ClassFiv -> Class::name(int, void)
// with a fall back to the original name.
//
// Assumes the destination is buffer is sized to MAXDBUF bytes.

#include "dem.h"

#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

// Append 's', limited to rem bytes.
static void dab_append(char **dst, size_t *rem, const char *s)
{
    size_t n;

    if (*rem == 0)
        return;

    n = strlen(s);
    if (n >= *rem)
        n = *rem - 1; // for NULL.

    memcpy(*dst, s, n);
    *dst += n;
    *(*dst) = '\0';
    *rem -= n;
}

// Append 'n' bytes from 's', limited to rem bytes.
static void dab_append_n(char **dst, size_t *rem, const char *s, size_t n)
{
    if (*rem == 0)
        return;

    if (n >= *rem)
        n = *rem - 1;

    memcpy(*dst, s, n);
    *dst += n;
    *(*dst) = '\0';
    *rem -= n;
}

/* --- type decoder for cfront codes ------------------------------------ */
static const char *decode_base_type_char(int c)
{
    switch (c) {
    case 'v': return "void";
    case 'i': return "int";
    case 'l': return "long";
    case 's': return "short";
    case 'c': return "char";
    case 'f': return "float";
    case 'd': return "double";
    case 'b': return "bool";   /* non-standard but sometimes seen */
    default:  return NULL;      /* unknown */
    }
}

/* returns new index after decoding one type; prints into dst/rem */
static size_t decode_type_list(const char *p, char **dst, size_t *rem)
{
    /* cfront encodes argument lists after 'F'. Example: FiPc -> (int, char*) */
    int first = 1;
    while (*p && *p != '_' && *p != '\0') {
        int ptr_depth, ref_depth;
        const char *bt;

        if (!first) { dab_append(dst, rem, ", "); }
        first = 0;
        ptr_depth = 0;
        ref_depth = 0;
        while (*p == 'P') { ptr_depth++; p++; }
        while (*p == 'R') { ref_depth++; p++; }
        bt = decode_base_type_char(*p);
        if (bt) { dab_append(dst, rem, bt); p++; }
        else {
            /* class/struct types may appear as an identifier token until next special */
            const char *q = p;
            while (*q && *q!='P' && *q!='R' && *q!='_' && !strchr("vilscfdb", *q)) q++;
            if (q == p) { dab_append(dst, rem, "?"); if (*p) p++; }
            else { dab_append_n(dst, rem, p, (size_t)(q-p)); p = q; }
        }
        while (ref_depth--) dab_append(dst, rem, "&");
        while (ptr_depth--) dab_append(dst, rem, "*");
    }
    return (size_t)(p - (const char*)0); /* caller doesn’t use this value */
}

void demangle(const char* name, char *sbuf)
{
    const char *dd, *base, *p, *class_start, *class_end;
    char *out;
    size_t rem, base_len;
    int has_class;

    if (!name || !sbuf) return;
    out = sbuf;
    rem = MAXDBUF;
    *out = '\0';

    dd = strstr(name, "__");
    if (!dd) { /* nothing to do */ dab_append(&out, &rem, name); return; }

    /* Split: base name before '__' */
    base = name;
    base_len = (size_t)(dd - name);

    /* After '__' there may be an optional class name, then an optional 'F' args */
    p = dd + 2;

    /* Heuristic: a class token is letters/digits/underscores up to 'F' or string end */
    class_start = p;
    class_end = class_start;
    while (*class_end && *class_end != 'F' && *class_end != '_') class_end++;
    has_class = (class_end > class_start);

    /* If the bit right after '__' is 'F', then no class qualifier */
    if (*p == 'F') { has_class = 0; class_end = p; }

    /* Render: Class::base or just base */
    if (has_class) {
        dab_append_n(&out, &rem, class_start, (size_t)(class_end - class_start));
        dab_append(&out, &rem, "::");
    }
    dab_append_n(&out, &rem, base, base_len);

    /* Arguments */
    if (*class_end == 'F') {
        p = class_end + 1;
        dab_append(&out, &rem, "(");
        decode_type_list(p, &out, &rem);
        dab_append(&out, &rem, ")");
    }

    /* If we didn’t decode anything meaningful, fall back to original */
    if (out == sbuf || sbuf[0] == '\0') {
        /* extremely defensive; shouldn’t happen */
        strncpy(sbuf, name, MAXDBUF-1);
        sbuf[MAXDBUF-1] = '\0';
    }
}
