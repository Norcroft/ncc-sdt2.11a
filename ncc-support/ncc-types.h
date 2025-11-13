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

#include <ctype.h>
//#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__CC_NORCROFT)
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef signed int ssize_t;
#  define inline __inline
#  define INT64_MAX        9223372036854775807LL
#  define UINT64_MAX       18446744073709551615ULL
#  define INT64_MIN        (-INT64_MAX-1)
#else
#  include <sys/types.h>
#endif

// No support for stdbool.h yet.
typedef unsigned char bool;
#define false 0
#define true  1

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;

typedef uint8_t unsigned8;
typedef uint16_t unsigned16;
typedef uint32_t unsigned32;

typedef void* VoidStar;
typedef const void* ConstVoidStar;
typedef uint32_t Uint;

typedef ssize_t IPtr; // Holds 'int' or host-sized pointer.
typedef size_t  UPtr; // Holds 'unsigned32' or host-sized pointer.
