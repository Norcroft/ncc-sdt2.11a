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

#include "dde.h"
#include "fname.h"

#include <stdio.h>

const char* dde_desktop_prefix = 0;
int dde_throwback_flag = 0;

#if defined(FOR_ACORN) && defined(COMPILING_ON_RISC_OS)
#include "globals.h"  // for 'sourcefile'
#include "compiler.h" // for FNAME_SUFFIXES

#include <stdlib.h>
#include <string.h>
#include <kernel.h>

#define DDEUtils_Prefix 0x42580
#define DDEUtils_ThrowbackStart 0x42587
#define DDEUtils_ThrowbackSend 0x42588

static int registered = 0;

void dde_prefix_init(const char* fname)
{
    UnparsedName un;
    char* path = 0;
    _kernel_swi_regs regs;

    fprintf(stderr, "dde_prefix_init: %s ", fname);

    fname_parse(fname, FNAME_SUFFIXES, &un);

    if (un.plen > 0) {
        path = malloc(un.plen + 1);
        memcpy(path, un.path, un.plen);
        path[un.plen-1] = '\0';
    }

    fprintf(stderr, "path: %.*s ", un.plen, un.path);

    regs.r[0] = (int)path;
    _kernel_swi(DDEUtils_Prefix, &regs, &regs);

    if (path)
        free(path);
}

void dde_sourcefile_init(void)
{
}

void dde_throwback_send(unsigned int severity, unsigned int line, const char* msg)
{
    _kernel_swi_regs regs;

    fprintf(stderr, "dde_throwback_send: line:%d:%s (%d)", line, msg, severity);

    if (!registered) {
        _kernel_swi(DDEUtils_ThrowbackStart, &regs, &regs);

        registered = true;
    }

    regs.r[0] = 1;
    regs.r[2] = (int)sourcefile;
    regs.r[3] = line;
    regs.r[4] = severity;
    regs.r[5] = (int)msg;
    _kernel_swi(DDEUtils_ThrowbackSend, &regs, &regs);
}

#endif
