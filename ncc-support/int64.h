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

#include "ncc-types.h"

typedef struct { uint32 lo; uint32 hi; } uint64;
typedef struct { uint32 lo;  int32 hi; } int64;

typedef enum {
    i64_ok = 0,        // operation succeeded
    i64_overflow,      // result overflowed
    i64_divzero        // division by zero
} I64_Status;

// Args largely should be, even for methods that can not error:
//
// I64_Status (*lunaryu)(int64 *res, uint32 n);
// I64_Status (*lunaryi)(int64 *res, int32 n);
// I64_Status (*unaryl)(int64 *res, int64 const *a1);
// I64_Status (*binaryll)(int64 *res, int64 const *a1, int64 const *a2);
// I64_Status (*binaryli)(int64 *res, int64 const *a1, Uint a2);
// I64_Status (*binarylui)(uint64 *res, uint64 const *a1, Uint a2);


extern I64_Status I64_SToI(int32 *res, int64 *i);
extern I64_Status I64_UToI(uint32 *res, uint64 *i);

extern I64_Status I64_IToS(int64 *res, int32 n);
extern I64_Status I64_IToU(uint64 *res, int32 i);

extern I64_Status I64_UAdd(uint64 *res, uint64 *a1, uint64 *a2);
extern I64_Status I64_USub(uint64 *res, uint64 *a1, uint64 *a2);
extern I64_Status I64_UMul(uint64 *res, uint64 *a1, uint64 *a2);
extern I64_Status I64_UDiv(uint64 *res, uint64 *rem, uint64 const *num, uint64 const *den);

extern I64_Status I64_SAdd(int64 *res, int64 const *a1, int64 const *a2);
extern I64_Status I64_SSub(int64 *res, int64 const *a1, int64 const *a2);
extern I64_Status I64_SMul(int64 *res, int64 const *a1, int64 const *a2);
extern I64_Status I64_SDiv(int64 *res, int64 *rem, int64 const *num, int64 const *den);

extern void I64_And(int64 *res, int64 const *a1, int64 const *a2);
extern void I64_Or(int64 *res, int64 const *a1, int64 const *a2);
extern void I64_Eor(int64 *res, int64 const *a1, int64 const *a2);
extern void I64_Not(int64 *res, int64 const *a1);

extern I64_Status I64_Neg(int64 *res, int64 const *a1);

extern I64_Status I64_Lsh(int64 *res, int64 const *a1, Uint a2);
extern I64_Status I64_URsh(uint64 *res, uint64 const *a1, Uint a2);
extern I64_Status I64_SRsh(int64 *res, int64 const *a1, Uint a2);

extern int I64_UComp(uint64 *a, uint64 *b);
extern int I64_SComp(int64 *a, int64 *b);
