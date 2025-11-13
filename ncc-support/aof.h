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

// ARM Object format. Acorn's spec is at:
// http://www.riscos.com/support/developers/prm/objectformat.html

// ARM's spec:
// https://developer.arm.com/documentation/dui0041/c/ARM-Object-Format/Symbol-Table-Chunk-Format--OBJ-SYMT-/Symbol-attributes?lang=en

#include <stddef.h>

// aof_header is followed by an array of aof_area records.

#define AOF_RELOC     0xC5E2D080    // Object FIle Type
#define AOF_VERSION   310           // From ARM's 2.50 SDT docs.

typedef struct aof_area {
    int32 area_name;        // offset into string variable
    int32 area_attributes;  // alignment/flags (see AOF_* below)
    int32 area_size;
    int32 area_nrelocs;
    int32 area_base;        // must be 0.
} aof_area;

typedef struct aof_header {
    int32 aof_type;         // Object file type (AOF_RELOC)
    int32 aof_vsn;          // Version id (AOF_VERSION)
    int32 aof_nareas;
    int32 aof_nsyms;
    int32 aof_entryarea;    // Entry Address area. Unused for relocatable objs.
    int32 aof_entryoffset;  // Entry Address offset. Unused for relocatable objs.
    aof_area aof_areas[1];  // aof_areas[aof_nareas];
} aof_header;

// Area attribute - byte-sized in Acorn's day, but ARM have extended it.
// Acorn say the word-sized value consists of four bytes, [0, 0, AT, AL].
// AL must be 2. Don't include the 2 as aaof.c does.

#define AOF_CODEAT       0x000200
#define aof_COMDEFAT     0x000400
#define AOF_COMREFAT     0x000800
#define AOF_0INITAT      0x001000
#define AOF_RONLYAT      0x002000
#define AOF_PICAT        0x004000
#define AOF_DEBUGAT      0x008000
#define AOF_32bitAT      0x010000
#define AOF_REENTAT      0x020000
#define AOF_FP3AT        0x040000
#define AOF_NOSWSTKCK    0x080000
#define AOF_THUMB        0x100000
#define AOF_INTERWORK    0x400000
#define AOF_GNU_LINKONCE 0x10000000

// [INVENTED] These are made up values as I have no documentation covering them...
#define AOF_BASEDAT      0x100000 // drlink implies = AOF_THUMB, iff data area.
#define AOF_BASESHIFT    24 // (<reg num> << BASESHIFT) - put in top byte.

#define AOF_COMDEFAT     (aof_COMDEFAT | AOF_GNU_LINKONCE)

typedef struct aof_symbol {
    int32 sym_name;      // index into string table
    int32 sym_AT;        // attribute type
    int32 sym_value;
    int32 sym_areaname;
} aof_symbol;

// Symbol AT (attribute type)
#define SYM_LOCALDEFAT   0x00000001  // defined, local
#define SYM_REFAT        0x00000002  // undefined/external
#define SYM_GLOBALDEFAT  0x00000003  // defined, global
#define SYM_WEAKAT       0x00000010  // weak reference
#define SYM_DATAAT       0x00000100  // data-in-code etc.
#define SYM_FPREGAT      0x00000200  // uses FP regs
#define SYM_LEAFAT       0x00000800  // leaf function (doesn't use the sb reg)
#define SYM_THUMB        0x00001000  // Thumb symbol

typedef struct aof_reloc {
    int32 rel_offset;
    int32 rel_flags;     // REL_* flags, below.
} aof_reloc;

// From ARM's spec, format of type 2 relocation directives:
// [1, II, B, A, R, FT, 24-bit SID]

#define REL_TYPE2        0x80000000  // Type 2 relocation directive
#define REL_B            0x10000000  // Based relocation
#define REL_A            0x08000000  // Additive type
#define REL_R            0x04000000  // PC-relative
#define REL_LONG         0x02000000  // Four byte relocation. FT = 10.
#define REL_INSTR        0x03000000  // Instruction encoding. FT = 11.
