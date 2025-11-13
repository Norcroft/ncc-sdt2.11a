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

#define THROWBACK_WARN      0
#define THROWBACK_ERROR     1
#define THROWBACK_SERIOUS   2

// Untouched source passes in 'current', which is an unknown variable.
void dde_prefix_init(const char* infile);

void dde_sourcefile_init(void);

void dde_throwback_send(unsigned int severity, unsigned int line, const char* msg);

extern const char* dde_desktop_prefix;
extern int dde_throwback_flag;
