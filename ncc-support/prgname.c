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

#include "prgname.h"

#include <string.h>

char *program_name(const char *path, char *buf, size_t buflen)
{
    // Called like:
    //   progname = program_name(argv[0], p, 32);
    // where 'char p[32]'

    // Scan backwards looking for any non-filename character.
    const char* leaf = path + strlen(path);
    while (leaf > path) {
        if (*--leaf == '/') {
            leaf++; // don't want the path separator.
            break;
        }
    }

    strncpy(buf, leaf, buflen - 1);
    buf[buflen - 1] = '\0';

    return buf;
}
