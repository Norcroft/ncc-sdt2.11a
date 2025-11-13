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

#include "msg.h"

#include <stdio.h>
#include <string.h>

void msg_printf(msg_t msg, const char* str)
{
    size_t len;

    fprintf(stderr, msg, str);

    // Print a newline if the msg doesn't end with one (some help texts have
    // '\n' on each line, some don't).
    len = msg ? strlen(msg) : 0;
    if (len == 0 || msg[len - 1] != '\n')
        fputc('\n', stderr);
}

void msg_sprintf(char* buf, msg_t msg, const char* str)
{
    // Delightful. No idea what the size is.
    sprintf(buf, msg, str);
}

char* msg_lookup(msg_t msg)
{
    return msg;
}

