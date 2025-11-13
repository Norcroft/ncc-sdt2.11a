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

// I think BackChat is ARM's Windows equivlant of Acorn's DDE throwback.

// [INVENTED] Made these numbers up.
typedef enum {
    BC_SEVERITY_NONE    = 0,
    BC_SEVERITY_INFO    = 1,
    BC_SEVERITY_WARN    = 2,
    BC_SEVERITY_ERROR   = 3,
    BC_SEVERITY_SERIOUS = 4,
    BC_SEVERITY_FATAL   = 5
} BC_Severity;

// [INVENTED] Made up the order of the struct.
typedef struct {
    const char      *toolname;
    const char      *filename;
    const char      *msgtext;
    BC_Severity     severity;
    unsigned short  column;
    unsigned        lineno;
    int             filepos;
} backchat_Diagnostic;

// [INVENTED] Made up the order of the struct.
typedef struct {
    const char *targetName;
    const char *dependsonName;
} backchat_InclusionDependency;

// [INVENTED] Made these numbers up.
typedef enum { BC_NULLMSG, BC_DIAGMSG, BC_INCLUDEMSG } backchat_code;

typedef int backchat_Messenger(void *handle,
                               unsigned backchat_code,
                               const void *msg);

// [INVENTED] Made up the order of the struct.
typedef struct {
    backchat_Messenger  *send;   // callback function
    void                *handle; // opaque handle passed back to callback
} backchat_Interface;
