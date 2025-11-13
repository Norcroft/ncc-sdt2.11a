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

// ARM Symbolic Debugging Format.

// Acorn's spec:
// http://www.riscos.com/support/developers/prm/objectformat.html

// 'debugversion' field. Acorn used up to 2, but ARM's extended it (
#define ASD_FORMAT_VERSION 2

// [INVENTED] Fileinfo short format, max line length.
// In short format, two byte are used to store 'lineinfo'. See end of Acorn's
// spec. One byte is the number of bytes of code generated, and the second
// byte is the number of source lines.
//
// However, ARM have clearly extended this is to combine col and lines
// into one byte, if "OldAsdTables" is not true (it is set when using -asd-old
// on the command line, as opposed to -asd).
//
// Make up a number for now - I suppose you're more likely to have lots of
// characters than lines, for a statement, but since it's the law that everyone
// has a line length limit of 80, I'll use that.
#define Asd_LineInfo_Short_MaxLine 80

#define LANG_ASM        0
#define LANG_C          1
#define LANG_PASCAL     2
#define LANG_FORTRAN77  3

// Item kind codes ("itemsort") written via dbg_hdr(itemsort, length).
// These values are from Acorn's ASD spec.
#define ITEMSECTION        0x0001  // section
#define ITEMPROC           0x0002  // procedure
#define ITEMENDPROC        0x0003  // endproc
#define ITEMVAR            0x0004  // variable
#define ITEMTYPE           0x0005  // type
#define ITEMSTRUCT         0x0006  // struct
#define ITEMARRAY          0x0007  // array
// [INVENTED] subrange (8) and set (9) not used as they're for Pascal. Except
// type 8 "also serves to describe enumerated types in C". As there are two enum
// types, it seems plausible 9 may be the second enum type. But which is which?
#define ITEMENUMC          0x0008  // contiguous-enum - maybe type 8?
#define ITEMENUMD          0x0009  // discontiguous-enum - maybe type 9?

#define ITEMFILEINFO       0x000A  // fileinfo

// [INVENTED] These values are made up as I have no documentation...
#define ITEMUNION          0x000B
#define ITEMCLASS          0x000C
#define ITEMBITFIELD       0x000D
#define ITEMSCOPEBEGIN     0x000E
#define ITEMSCOPEEND       0x000F
#define ITEMUNDEF          0x0010
#define ITEMDEFINE         0x0011
#define ITEMFPMAPFRAG      0x0012  // Only used if TARGET_HAS_FP_OFFSET_TABLES

// This might be a frame pointer map fragment? For stack unwinding. Who knows!
// [INVENTED] No clue what the order of this struct should be.
typedef struct ItemFPMapFragment {
    int32 marker;      // low = ITEMFPMAPFRAG; high = bytes+6*4.
    uint64_t codestart;
    uint64_t codesize;
    uint64_t saveaddr;
    int32 initoffset;
    int32 bytes;       // num of bytes that follow in b[], rounded up to a word.
    char  b[1];
} ItemFPMapFragment;

// Primitive base types. The groupings are actually in base ten, not hex.
#define TYPEVOID       0
#define TYPESBYTE     10
#define TYPESHALF     11
#define TYPESWORD     12
#define TYPEUBYTE     20
#define TYPEUHALF     21
#define TYPEUWORD     22
#define TYPEUDWORD    23 // [INVENTED] Seems the most plausible value.
#define TYPEFLOAT     30
#define TYPEDOUBLE    31
#define TYPEFUNCTION 100

// TYPE_TYPEWORD — pack type and pointer depth into one 32-bit field
#define TYPE_TYPEWORD(TYPE, PTR_COUNT) (((TYPE) << 8) | PTR_COUNT)

// Storage classes of variables.
typedef enum {
    C_EXTERN            = 1,
    C_STATIC            = 2,
    C_AUTO              = 3,
    C_REG               = 4,
    PASCAL_VAR          = 5,
    FORTRAN_ARGS        = 6,
    FORTRAN_CHAR_ARGS   = 7
} StgClass;

// No idea what asd_Address means - its only use it to create NoSaveAddr
// "#define NoSaveAddr ((asd_Address)-1)".
//
// NoSaveAddr is then only used to assign and compare against an int32.
//
// The name would imply it should be a pointer type, but then the use-cases
// fail to compile. It's most likely a 32-bit int, but it could be zero
// to create 0-1 = -1.
typedef int32_t asd_Address;
