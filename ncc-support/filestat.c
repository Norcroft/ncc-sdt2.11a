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

#include "filestat.h"

#ifdef __riscos
int filestat_istty(FILE *fp) {
    return 0;
}

#else

#include <unistd.h>

#define filestat_fileno fileno
#define filestat_isatty isatty

int filestat_istty(FILE *fp)
{
    if (fp == NULL) return 0;
    int fd = filestat_fileno(fp);
    if (fd < 0) return 0;
    return filestat_isatty(fd);
}
#endif
