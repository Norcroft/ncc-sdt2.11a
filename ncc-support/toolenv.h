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

#include <stddef.h>
#include <stdlib.h>

#include "backchat.h"
#include "ncc-types.h"

// Opaque handles
typedef struct ToolEnv ToolEnv;
typedef void *ToolEnvDelta;

typedef char *ArgvType;

/* Dummy HWND for non-Windows builds */
#ifndef TOOLBOX_DUMMY_HWND_DEFINED
#define TOOLBOX_DUMMY_HWND_DEFINED 1
typedef int HWND;
#endif

// ToolEnv value enumerator callback (name, value)
typedef int (ToolEnvItemFn)(void *arg, const char *name, const char *val);

typedef struct ToolEntryPoints {
    int  (*toolbox_finalise)(const struct ToolEntryPoints *te);
    int  (*toolbox_main)(int argc, ArgvType *argv, ToolEnv *t,
                         backchat_Messenger *sendmsg, void *backchathandle);
    ToolEnv *(*toolenv_new)(void);
    void     (*toolenv_dispose)(ToolEnv *);
    int      (*toolenv_merge)(ToolEnv *, ToolEnvDelta);
    ToolEnvDelta (*toolenv_mark)(ToolEnv *);
    ToolEnvDelta (*toolenv_getdelta)(ToolEnv *);
    int      (*toolenv_putinstallationdelta)(ToolEnv *);
    int      (*Tool_EditEnv)(ToolEnv *, HWND);
    void* so_far_always_null;
} ToolEntryPoints;

/* Tool-specific init entrypoint returning jump table */
extern const ToolEntryPoints *armccinit(void);

ToolEnv *toolenv_new(void);
void toolenv_dispose(ToolEnv* t);

const char* toolenv_lookup(const ToolEnv*, const char* name);

int toolenv_enumerate(const ToolEnv *t,
                      int (*fn)(void *arg, const char *name, const char *val),
                      void* arg);

ToolEnv *toolenv_copy(ToolEnv *);

/* Basic env ops used by tooledit.c */
int  toolenv_insert(struct ToolEnv *t, const char *name, const char *value);
int  toolenv_insertwithjoin(struct ToolEnv *t, const char *name, int join, const char *value);

/* Ordered enumeration (for -I. and -J.) with readonly bit */
int  Tool_OrderedEnvEnumerate(struct ToolEnv *t, const char *prefix,
                              int (*fn)(void *arg, const char *name, const char *val, bool readonly),
                              void *arg);

// The following aren't implemented.
int toolenv_merge(ToolEnv *, ToolEnvDelta);
ToolEnvDelta toolenv_mark(ToolEnv *);                // snapshot current state
ToolEnvDelta toolenv_getdelta(ToolEnv *);
int toolenv_putinstallationdelta(ToolEnv* t);
