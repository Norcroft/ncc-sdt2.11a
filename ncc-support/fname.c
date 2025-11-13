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

#include "fname.h"

#include <ctype.h>
#include <string.h>

static int is_sep(int c) { return c=='/' || c=='\\'; }
static int match_suffix(const char *ext, size_t elen, const char *list) {
    const char *p;

    if (!ext || elen==0) return 0;
    // list like: "c C h H c++ ..." — space-separated tokens

    for (p = list; *p;) {
        const char *q;
        size_t n;

        while (*p==' ') ++p;

        q = p;

        while (*q && *q!=' ') ++q;
        n = (size_t)(q-p);
        if (n==elen && strncasecmp(ext, p, elen)==0)
            return 1;
        p = q;
    }
    return 0;
}

static void unix_fname_parse(const char *file,
                             const char *suffixlist,
                             UnparsedName *un)
{
    const char *s, *last_sep, *p, *leaf, *last_dot;
    size_t len;

    memset(un, 0, sizeof *un);
    if (!file)
        return;

    s = file;
    len = strlen(file);

    // Rooted?
    if (len && (is_sep(s[0]))) {
        un->type |= FNAME_ROOTED;
    }

    // Split path / leaf at last separator
    last_sep = NULL;
    p = NULL;
    for (p = s; *p; ++p) {
        if (is_sep(*p))
            last_sep = p;
    }

    leaf = last_sep ? last_sep+1 : s;
    un->path = s;
    un->plen = (size_t)(leaf - s);

    // Split root/ext at last '.'
    last_dot = NULL;
    for (p = leaf; *p; ++p) {
        if (*p == '.')
            last_dot = p;
    }

    if (last_dot && last_dot != leaf && last_dot[1] != '\0') {
        un->root = leaf;
        un->rlen = (size_t)(last_dot - leaf);
        un->extn = last_dot + 1;
        un->elen = strlen(un->extn);
    } else {
        un->root = leaf;
        un->rlen = strlen(leaf);
        un->extn = NULL;
        un->elen = 0;
    }

    // Optionally: only “recognised” include suffixes count as extn
    if (suffixlist && un->extn && !match_suffix(un->extn, un->elen, suffixlist)) {
        // treat as “no extension” from the compiler’s POV
        un->root = leaf;
        un->rlen = strlen(leaf);
        un->extn = NULL;
        un->elen = 0;
    }

    un->un_pathlen = 0;
}

#ifndef COMPILING_ON_RISC_OS
void fname_parse(const char *fname, const char *suffixlist, UnparsedName *un)
{
    return unix_fname_parse(fname, suffixlist, un);
}

int fname_unparse(UnparsedName *un,
                  unparsedFNameType how,
                  char *out,
                  size_t maxName)
{
    size_t n = 0;
    if (!out || maxName==0) return 0;

    // path
    if (un->plen) {
        size_t m = un->plen;
        if (m > maxName-1) m = maxName-1;
        memcpy(out, un->path, m);
        n += m;
    }
    if (how == FNAME_AS_PATH) {
        /* If it looks like a directory (no extension), keep the leaf too. */
        if (un->elen == 0 && un->rlen > 0) {
            size_t m;

            if (n && out[n-1] != '/' && out[n-1] != '\\' && n < maxName-1)
                out[n++] = '/';
            m = un->rlen;
            if (m > maxName-1-n)
                m = maxName-1-n;
            memcpy(out+n, un->root, m);
            n += m;

            out[n++] = '/';
        }

        out[n] = '\0';
        return (int)n;
    }

    // name = root [ "." ext ]
    if (un->rlen && n < maxName-1) {
        size_t m = un->rlen; if (m > maxName-1-n) m = maxName-1-n;
        memcpy(out+n, un->root, m); n += m;
    }
    if (un->elen && n+1 < maxName-1) {
        size_t m;
        out[n++] = '.';
        m = un->elen; if (m > maxName-1-n) m = maxName-1-n;
        memcpy(out+n, un->extn, m); n += m;
    }
    out[n] = '\0';

    // Tell callers where the path ends in THIS buffer
    un->un_pathlen = un->plen > n ? n : un->plen;
    return (int)n;
}

#else

// RISC OS file parsing. Probably somewhat broken as I'm doing it from memory.
void fname_parse(const char *fname, const char *suffixlist, UnparsedName *un)
{
    const char *p, *last_sep = NULL, *prev_sep = NULL;
    size_t len;

    memset(un, 0, sizeof *un);
    if (!fname)
        return;

    if (fname[0] == '/' || fname[0] == '\\') {
        unix_fname_parse(fname, suffixlist, un);
        return;
    }

    len = strlen(fname);
    if (len == 0)
        return;

    p = fname + len - 1;
    while (p >= fname) {
        if (*p == '.' || *p == ':') {
            if (last_sep == NULL) {
                last_sep = p;
                if (*p == ':')
                    break;
            } else {
                prev_sep = p;
                break; // got all we're looking for.
            }
        }

        p--;
    }
    // Note: if either separator is a colon, that colon needs preserving.

    // Determine if rooted. This will be a '$', or '@' either at the start
    // of the path, or after the last colon.
    if (len && (fname[0] == '$' || fname[0] == '@')) {
        un->type |= FNAME_ROOTED;
    }

    if (!last_sep) {
        /* No separator at all: no path, no extn, whole string is root */
        un->root = fname;
        un->rlen = len;

#if 0
        fprintf(stderr, "fname_parse: %s p'%.*s' f'%.*s' e'%.*s' t:%d\n",
                fname, un->plen, un->path, un->rlen, un->root, un->elen, un->extn, un->type);
#endif
        return;
    }

    // Leaf root is after last_sep
    un->root = last_sep + 1;
    un->rlen = (size_t)(fname + len - (last_sep + 1));

    // adjust last_sep if it's a colon to preseve it. It now points at the char
    // after that segment regardless of it it's a colon or dot. In the case
    // of it being a colon, there can't be an extension.
    if (*last_sep == ':') {
        last_sep++;
    } else {
        // A filename consists of leaf + ext. We don't know which is which
        // yet.

        // First component (p) is either the string from prev_sep to last_sep,
        // or from the start of the passed in fname to last_sep.
        p = prev_sep ? prev_sep + 1 : fname;

        // adjust prev_sep if it's a colon to preserve it (after suffix matching)
        if (prev_sep && *prev_sep == ':')
            prev_sep++;

        if (suffixlist && match_suffix(p, last_sep - p, suffixlist)) {
            // ext.filename
            un->extn = p;
            un->elen = last_sep - p;

            un->path = fname;
            un->plen = prev_sep ? prev_sep - fname : 0;

#if 0
            fprintf(stderr, "fname_parse: %s p'%.*s' f'%.*s' e'%.*s' t:%d\n",
                    fname, un->plen, un->path, un->rlen, un->root, un->elen, un->extn, un->type);
#endif
            return;
        } else if (suffixlist && match_suffix(un->root, un->rlen, suffixlist)) {
            // filename.ext: What we thought was the leafname is a valid suffix.
            un->extn = un->root;
            un->elen = un->rlen;

            un->root = p;
            un->rlen = last_sep - p;

            un->path = fname;
            un->plen = prev_sep ? prev_sep - fname : 0;

#if 0
            fprintf(stderr, "fname_parse: %s p'%.*s' f'%.*s' e'%.*s' t:%d\n",
                    fname, un->plen, un->path, un->rlen, un->root, un->elen, un->extn, un->type);
#endif
            return;
        }
    }

    // No extension.
    un->path = fname;
    un->plen = last_sep - fname;

#if 0
    fprintf(stderr, "fname_parse: %s p'%.*s' f'%.*s' e'%.*s' t:%d\n",
            fname, un->plen, un->path, un->rlen, un->root, un->elen, un->extn, un->type);
#endif
}

#define APPEND(c) do { *t++ = (c); if (t == t_max) goto end; } while (0);

// driver.c gives this delightful example:
/*  ^ -> .. (several times) + up to 2 extra path separators + a NUL. */
int fname_unparse(UnparsedName *un,
                  unparsedFNameType how,
                  char *out,
                  size_t maxName)
{
    const char *s, *s_end, *t_max = out + maxName;
    char *t = out;

    un->un_pathlen = 0;

    // Path. Map ['/' to '.'], ['.' to '/'], ['^' to '..']
    s = un->path;
    s_end = s + un->plen;

    while (s < s_end) {
        char c = *s++;

#if 0
        if (c == '/') {
            APPEND('/');
        } else if (c == '.') {
            APPEND('.');
        } else if (c == '^') {
            APPEND('.');
            APPEND('.');
        } else {
            APPEND(c);
        }
#endif
        APPEND(c);

    }

    // If last character (if there was one) wasn't a colon, append '.' then
    // leafname (root)
    if (t - out > 0 && t[-1] != ':')
        APPEND('.');

    // Append any extension.
    if (un->elen > 0) {
        s = un->extn;
        s_end = s + un->elen;

        while (s < s_end) {
            char c = *s++;

            APPEND(c);
        }

        APPEND('.');
    }

    s = un->root;
    s_end = s + un->rlen;

    while (s < s_end) {
        char c = *s++;

        if (c == '/') {
            APPEND('/');
        } else if (c == '.') {
            APPEND('.');
        } else {
            APPEND(c);
        }
    }

    if (how == FNAME_AS_PATH || un->rlen == 0) {
        APPEND('\0');

#if 0
        fprintf(stderr, "fname_unparse %d: p%u'%.*s' f%u'%.*s' e%u'%.*s' munged to: %s\n",
                how, un->plen, un->plen, un->path, un->rlen, un->rlen, un->root, un->elen, un->elen, un->extn, out);
#endif

        un->un_pathlen = (int)(t-out);

        return un->un_pathlen;
    }

    APPEND('\0');

end:
    out[maxName - 1] = '\0';

    if (t >= t_max)
        return -1;

#if 0
    fprintf(stderr, "fname_unparse: p%u'%.*s' f%u'%.*s' e%u'%.*s' munged to: %s\n",
            un->plen, un->plen, un->path, un->rlen, un->rlen, un->root, un->elen, un->elen, un->extn, out);
#endif

    un->un_pathlen = (int)(t-out);

    return un->un_pathlen;
}
#endif
