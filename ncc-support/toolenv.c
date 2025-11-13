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

#include "toolenv.h"

#include "ncc-types.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

// Defined in driver.c
extern int toolenv_insertdefaults(ToolEnv *t);

typedef struct TE_Entry {
    char *name;
    char *val;      // include leading '=', if provided
    unsigned readonly:1;
} TE_Entry;

struct ToolEnv {
    TE_Entry *v;
    size_t n, cap;
    size_t mark_n;    // snapshot index for getdelta/merge stubs
};

static char *te_strdup(const char *s)
{
    size_t n = strlen(s)+1;

    char *p = (char*)malloc(n);

    if (p)
        memcpy(p,s,n);

    return p;
}

static int te_grow(struct ToolEnv *t, size_t need)
{
    size_t newcap;
    TE_Entry *nv;

    if (t->n + need <= t->cap)
        return 1;

    newcap = t->cap ? t->cap*2 : 8;

    while (newcap < t->n + need)
        newcap *= 2;

    nv = (TE_Entry*)realloc(t->v, newcap*sizeof *nv);

    if (!nv)
        return 0;

    t->v = nv;
    t->cap = newcap;

    return 1;
}

/* ---------------- public API ---------------- */
ToolEnv *toolenv_new(void)
{
    struct ToolEnv *t = (struct ToolEnv*)malloc(sizeof *t);

    if (!t)
        return NULL;

    t->v = NULL;
    t->n = 0;
    t->cap = 0;
    t->mark_n = 0;

    toolenv_insertdefaults(t);

    return t;
}

void toolenv_dispose(ToolEnv* t)
{
    size_t i;

    if (!t)
        return;

    for (i=0;i<t->n;i++)
    {
        free(t->v[i].name);
        free(t->v[i].val);
    }

    free(t->v);
    free(t);
}

int toolenv_merge(ToolEnv *dst, ToolEnvDelta d)
{
    UNUSED(dst); UNUSED(d);
    // FIXME: No-op.
    return 0;
}

ToolEnvDelta toolenv_mark(ToolEnv *t)
{
    if (!t)
        return NULL;

    t->mark_n = t->n;

    return (ToolEnvDelta)(uintptr_t)t->mark_n;
}

ToolEnvDelta toolenv_getdelta(ToolEnv *t)
{
    // FIXME: No-op.
    return (ToolEnvDelta)(uintptr_t)t->mark_n;
}

int toolenv_putinstallationdelta(ToolEnv* t)
{
    // FIXME: No-op.
    UNUSED(t);

    return 0;
}

ToolEnv *toolenv_copy(ToolEnv *src)
{
    ToolEnv *t;
    size_t i;

    if (!src)
        return NULL;

    t = (ToolEnv*)malloc(sizeof *t);

    if (!t)
        return NULL;

    t->n = src->n;
    t->cap = src->n;
    t->mark_n = src->mark_n;
    t->v = NULL;

    if (t->cap) {
        t->v = (TE_Entry*)malloc(t->cap * sizeof *t->v);

        if (!t->v) {
            free(t);
            return NULL;
        }
    }
    for (i = 0; i < src->n; ++i) {
        t->v[i].name = te_strdup(src->v[i].name);
        t->v[i].val  = te_strdup(src->v[i].val);
        t->v[i].readonly = src->v[i].readonly;
    }
    return t;
}

static ssize_t te_find(struct ToolEnv *t, const char *name)
{
    size_t i;

    for (i=0; i < t->n; i++) {
        if (strcmp(t->v[i].name, name)==0)
            return (ssize_t)i;
    }

    return -1;
}

const char* toolenv_lookup(const ToolEnv* t, const char* name)
{
    size_t i;

    if (!t || !name)
        return NULL;

    for (i=0; i < t->n; i++) {
        if (strcmp(t->v[i].name, name)==0)
            return t->v[i].val;
    }

    return NULL;
}

int toolenv_enumerate(const ToolEnv *t,
                      int (*fn)(void *arg, const char *name, const char *val),
                      void* arg)
{
    int rc;
    size_t i;

    if (!t || !fn)
        return 0;

    rc = 0;
    for (i = 0; i < t->n; i++) {
        rc = fn(arg, t->v[i].name, t->v[i].val);
        if (rc)
            break;
    }
    return rc;
}

int toolenv_insert(struct ToolEnv *t, const char *name, const char *value)
{
    char target_name[80];
    ssize_t i;

    if (!t || !name || !value)
        return -1;

#define TARGET_NAME "-D__TARGET_FPU_"
    // __TARGET_FPU_xxx uses whatever capitalisation is passed in on the command
    // line, or indeed the builtin 'fpa' string, which is clearly bonkers.
    // Capitalise them.

    if (strncmp(name, TARGET_NAME, sizeof(TARGET_NAME) - 1) == 0) {
        const char *s = name;
        char *t = target_name, *t_end = target_name + sizeof(target_name);
        while (t < t_end) {
            char c = *s++;
            *t++ = toupper((unsigned char)c);
            if (c == '\0')
                break;
        }
        *(t_end - 1) = '\0';

        name = target_name;
    }
#undef TARGET_NAME

    i = te_find(t, name);
    if (i >= 0) {
        char *nv = te_strdup(value);
        if (!nv)
            return -1;

        free(t->v[i].val);

        t->v[i].val = nv;

        return 0;
    }

    if (!te_grow(t,1))
        return -1;

    t->v[t->n].name = te_strdup(name);
    t->v[t->n].val  = te_strdup(value);
    t->v[t->n].readonly = 0;
    if (!t->v[t->n].name || !t->v[t->n].val)
        return -1;

    t->n++;
    return 0;
}

int toolenv_insertwithjoin(struct ToolEnv *t, const char *name, int join, const char *value)
{
    size_t vb;
    ssize_t i;
    char *nv;

    if (!t || !name || !value)
        return -1;

    /* Build the canonical stored value: '<join><value>' (e.g. "=-ansi") */
    vb = strlen(value);
    nv = (char*)malloc(vb + 2);  /* join + value + NUL */

    if (!nv)
        return -1;

    nv[0] = (char)join;
    memcpy(nv + 1, value, vb + 1);  /* include NUL */

    i = te_find(t, name);
    if (i >= 0) {
        /* Replace existing */
        free(t->v[i].val);
        t->v[i].val = nv;
        return 0;
    }

    /* Insert new */
    if (!te_grow(t, 1)) {
        free(nv);
        return -1;
    }

    t->v[t->n].name = te_strdup(name);

    if (!t->v[t->n].name) {
        free(nv);
        return -1;
    }

    t->v[t->n].val = nv;
    t->v[t->n].readonly = 0;
    t->n++;

    return 0;
}
