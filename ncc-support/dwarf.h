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

// DWARF v6 spec:
// https://dwarfstd.org/doc/DWARF5.pdf

// DWARF v1 spec:
// https://dwarfstd.org/doc/dwarf_1_1_0.pdf

/* Minimal DWARF v2 constants used by ncc’s DWARF writer.                */
/* --------------------------------------------------------------------- */
/* TAGS (DW_TAG_*) — subset used here                                    */
#define DW_TAG_array_type                0x01
#define DW_TAG_class_type                0x02
#define DW_TAG_enumeration_type          0x04
#define DW_TAG_formal_parameter          0x05
#define DW_TAG_lexical_block             0x0b
#define DW_TAG_member                    0x0d
#define DW_TAG_pointer_type              0x0f
#define DW_TAG_reference_type            0x10
#define DW_TAG_compile_unit              0x11
#define DW_TAG_structure_type            0x13
#define DW_TAG_subroutine_type           0x15
#define DW_TAG_typedef                   0x16
#define DW_TAG_union_type                0x17
#define DW_TAG_unspecified_parameters    0x18
#define DW_TAG_inheritance               0x1c
#define DW_TAG_ptr_to_member_type        0x1f
#define DW_TAG_subrange_type             0x21
#define DW_TAG_base_type                 0x24
#define DW_TAG_const_type                0x26
#define DW_TAG_enumerator                0x28
#define DW_TAG_friend                    0x2a
#define DW_TAG_subprogram                0x2e
#define DW_TAG_variable                  0x34
#define DW_TAG_volatile_type             0x35

/* User range (DWARF v2) */
#define DW_TAG_lo_user                  0x4080
#define DW_TAG_hi_user                  0xffff

/* --------------------------------------------------------------------- */
/* ATTRIBUTES (DW_AT_*) — subset used here                                */
#define DW_AT_sibling                    0x01
#define DW_AT_location                   0x02
#define DW_AT_name                       0x03
#define DW_AT_byte_size                  0x0b
#define DW_AT_bit_offset                 0x0c
#define DW_AT_bit_size                   0x0d
#define DW_AT_stmt_list                  0x10
#define DW_AT_low_pc                     0x11
#define DW_AT_high_pc                    0x12
#define DW_AT_language                   0x13
#define DW_AT_containing_type            0x1d
#define DW_AT_default_value              0x1e
#define DW_AT_producer                   0x25
#define DW_AT_external                   0x3f
#define DW_AT_type                       0x49
#define DW_AT_friend                     0x41
#define DW_AT_data_member_location       0x38
#define DW_AT_const_value                0x1c
#define DW_AT_declaration                0x3c
#define DW_AT_virtuality                 0x4c
#define DW_AT_vtable_elem_location       0x4d
#define DW_AT_encoding                   0x3e
#define DW_AT_artificial                 0x34
#define DW_AT_specification              0x47
#define DW_AT_upper_bound                0x2f   /* standard v2 */

/* User attribute range */
#define DW_AT_lo_user                    0x2000
#define DW_AT_hi_user                    0x3fff

/* Norcroft-local extension carried in the user range */
#define DW_AT_proc_body                  (DW_AT_lo_user + 0x00)

/* --------------------------------------------------------------------- */
/* FORMS (DW_FORM_*) — subset used here                                   */
#define DW_FORM_addr                     0x01
#define DW_FORM_block2                   0x03
#define DW_FORM_block4                   0x04
#define DW_FORM_data2                    0x05
#define DW_FORM_data4                    0x06
#define DW_FORM_data8                    0x07
#define DW_FORM_string                   0x08
#define DW_FORM_block                    0x09
#define DW_FORM_block1                   0x0a
#define DW_FORM_data1                    0x0b
#define DW_FORM_flag                     0x0c
#define DW_FORM_sdata                    0x0d
#define DW_FORM_strp                     0x0e
#define DW_FORM_udata                    0x0f
#define DW_FORM_ref_addr                 0x10
#define DW_FORM_ref1                     0x11
#define DW_FORM_ref2                     0x12
#define DW_FORM_ref4                     0x13
#define DW_FORM_ref8                     0x14
#define DW_FORM_ref_udata                0x15
#define DW_FORM_indirect                 0x16

/* --------------------------------------------------------------------- */
/* OPERATIONS (DW_OP_*) — subset used here                                */
#define DW_OP_addr                       0x03
#define DW_OP_const4u                    0x0a
#define DW_OP_plus                       0x22
#define DW_OP_plus_uconst                0x23
#define DW_OP_reg0                       0x50
#define DW_OP_breg0                      0x70
#define DW_OP_regx                       0x90
#define DW_OP_fbreg                      0x91
#define DW_OP_bregx                      0x92

/* Convenience macros for register op families */
#define DW_OP_REG0 DW_OP_reg0
#define DW_OP_BREG0 DW_OP_breg0

/* --------------------------------------------------------------------- */
/* BASE TYPE ENCODINGS (DW_ATE_*)                                        */
#define DW_ATE_float                     0x04
#define DW_ATE_signed                    0x05
#define DW_ATE_signed_char               0x06
#define DW_ATE_unsigned                  0x07
#define DW_ATE_unsigned_char             0x08

/* --------------------------------------------------------------------- */
/* LANGUAGE CODES (DW_LANG_*)                                            */
#define DW_LANG_C89                      0x0001
#define DW_LANG_C                        0x0002
#define DW_LANG_C_plus_plus              0x0004
#define DW_LANG_C99                      0x000c
#define DW_LANG_C_plus_plus_03           0x0019
#define DW_LANG_C_plus_plus_11           0x001a
#define DW_LANG_C11                      0x001d
#define DW_LANG_C_plus_plus_14           0x0021
#define DW_LANG_C_plus_plus_17           0x002a
#define DW_LANG_C_plus_plus_20           0x002b
#define DW_LANG_C17                      0x002c
#define DW_LANG_C_plus_plus_23           0x003a
#define DW_LANG_C23                      0x003e

/* Convenience aliases used in sources */
#define LANG_C89                        DW_LANG_C89
#define LANG_C_PLUS_PLUS                DW_LANG_C_plus_plus

/* --------------------------------------------------------------------- */
/* VIRTUALITY (DW_VIRTUALITY_*)                                          */
#define DW_VIRTUALITY_none               0
#define DW_VIRTUALITY_virtual            1
#define DW_VIRTUALITY_pure_virtual       2

/* --------------------------------------------------------------------- */
/* LINE NUMBER PROGRAM (DW_LNS_*, DW_LNE_*) — DWARF v2                    */
#define DW_LNS_copy                      1
#define DW_LNS_advance_pc                2
#define DW_LNS_advance_line              3
#define DW_LNS_set_file                  4
#define DW_LNS_set_column                5
#define DW_LNS_negate_stmt               6
#define DW_LNS_set_basic_block           7
#define DW_LNS_const_add_pc              8
#define DW_LNS_fixed_advance_pc          9

/* Highest standard opcode for v2 */
#define DW_LNS_LAST                      DW_LNS_fixed_advance_pc + 1

/* Operand signature for each standard opcode (v2). */
/* 0 = no args, 1 = one LEB128 arg. (fixed_advance_pc is 2-byte uhalf) */
#define DW_LNS_OPERANDS  { \
    0 /*0 unused*/, \
    0 /*copy*/, \
    1 /*advance_pc (uLEB)*/, \
    1 /*advance_line (sLEB)*/, \
    1 /*set_file (uLEB)*/, \
    1 /*set_column (uLEB)*/, \
    0 /*negate_stmt*/, \
    0 /*set_basic_block*/, \
    0 /*const_add_pc*/, \
    1 /*fixed_advance_pc (uhalf)*/ \
}

/* Extended opcodes used here */
#define DW_LNE_end_sequence              1
#define DW_LNE_set_address               2
#define DW_LNE_define_file               3

/* --------------------------------------------------------------------- */
/* MACINFO (DWARF v2 .debug_macinfo)                                     */
#define DW_MACINFO_define                 0x01
#define DW_MACINFO_undef                  0x02
#define DW_MACINFO_start_file             0x03
#define DW_MACINFO_end_file               0x04
#define DW_MACINFO_vendor_ext             0xff

// DWARF v1 --------------------------------------------------------------------
#define TAG_padding                     0x0000
#define TAG_array_type                  0x0001
#define TAG_class_type                  0x0002
#define TAG_entry_point                 0x0003
#define TAG_enumeration_type            0x0004
#define TAG_formal_parameter            0x0005
#define TAG_global_subroutine           0x0006
#define TAG_global_variable             0x0007
#define TAG_label                       0x000a
#define TAG_lexical_block               0x000b
#define TAG_local_variable              0x000c
#define TAG_member                      0x000d
#define TAG_pointer_type                0x000f
#define TAG_reference_type              0x0010
#define TAG_compile_unit                0x0011
#define TAG_source_file                 0x0011
#define TAG_string_type                 0x0012
#define TAG_structure_type              0x0013
#define TAG_subroutine                  0x0014
#define TAG_subroutine_type             0x0015
#define TAG_typedef                     0x0016
#define TAG_union_type                  0x0017
#define TAG_unspecified_parameters      0x0018
#define TAG_variant                     0x0019
#define TAG_common_block                0x001a
#define TAG_common_inclusion            0x001b
#define TAG_inheritance                 0x001c
#define TAG_inlined_subroutine          0x001d
#define TAG_module                      0x001e
#define TAG_ptr_to_member_type          0x001f
#define TAG_set_type                    0x0020
#define TAG_subrange_type               0x0021
#define TAG_with_stmt                   0x0022
#define TAG_lo_user                     0x4080
#define TAG_hi_user                     0xffff


#define OP_BASEREG 3  /* v1 pseudo: base register (does not map 1:1 to v2) */
#define OP_CONST   DW_OP_const4u   /* push 32-bit unsigned constant */
#define OP_ADD     DW_OP_plus      /* integer addition */

// Made up values so it compiles...
#define FMT_ET         0xC0  /* “encoding tag” sentinel for Norcroft extras */
#define FMT_FT_C_X     0xC1  /* plain ‘char’ (implementation-defined) */
#define FMT_FT_C_C     0xC2  /* ‘signed char’ */

// [INVENTED ] Fundamental types
enum {
    FT_signed_char = 1,
    FT_unsigned_char,
    FT_signed_short,
    FT_unsigned_short,
    FT_signed_integer,
    FT_unsigned_integer,
    FT_signed_long,
    FT_signed_long_long,
    FT_unsigned_long,
    FT_unsigned_long_long,
    FT_float,
    FT_dbl_prec_float,
    FT_boolean,
    FT_pointer,        // fallback code for ‘pointer to T’
    FT_void
};

// ---------------------------------------------------------------------

// Many mip/dwarf1.c call sites build attribute codes as:
//   (attr_code << 4) | FORM_*  and then switch on (attr & 0xF).

#define FORM_ADDR       0x1
#define FORM_REF        0x2
#define FORM_BLOCK2     0x3
#define FORM_BLOCK4     0x4
#define FORM_DATA2      0x5
#define FORM_DATA4      0x6
#define FORM_DATA8      0x7
#define FORM_STRING     0x8

#define AT_sibling              (0x0010u | FORM_REF)
#define AT_location             (0x0020u | FORM_BLOCK2)
#define AT_name                 (0x0030u | FORM_STRING)
#define AT_fund_type            (0x0050u | FORM_DATA2)
#define AT_mod_fund_type        (0x0060u | FORM_BLOCK2)
#define AT_user_def_type        (0x0070u | FORM_REF)
#define AT_mod_u_d_type         (0x0080u | FORM_BLOCK2)
#define AT_ordering             (0x0090u | FORM_DATA2)
#define AT_subscr_data          (0x00a0u | FORM_BLOCK2)
#define AT_byte_size            (0x00b0u | FORM_DATA4)
#define AT_bit_offset           (0x00c0u | FORM_DATA2)
#define AT_bit_size             (0x00d0u | FORM_DATA4)
#define AT_element_list         (0x00f0u | FORM_BLOCK4)
#define AT_stmt_list            (0x0100u | FORM_DATA4)
#define AT_low_pc               (0x0110u | FORM_ADDR)
#define AT_high_pc              (0x0120u | FORM_ADDR)
#define AT_language             (0x0130u | FORM_DATA4)
#define AT_member               (0x0140u | FORM_REF)
#define AT_discr                (0x0150u | FORM_REF)
#define AT_discr_value          (0x0160u | FORM_BLOCK2)
#define AT_string_length        (0x0190u | FORM_BLOCK2)
#define AT_common_reference     (0x01a0u | FORM_REF)
#define AT_comp_dir             (0x01b0u | FORM_STRING)
#define AT_const_value          (0x01c0u | FORM_STRING)
#define AT_containing_type      (0x01d0u | FORM_REF)
#define AT_friends              (0x01f0u | FORM_BLOCK2)

#define AT_producer             (0x0250u | FORM_STRING)
#define AT_virtual              (0x0300u | FORM_STRING)
#define AT_lo_user              (0x2000u)

#define AT_MAKE(code, form) (((code) << 8) | form)
#define AT_ADDR(code)       AT_MAKE((code), FORM_ADDR)
#define AT_REF(code)        AT_MAKE((code), FORM_REF)
#define AT_BLOCK2(code)     AT_MAKE((code), FORM_BLOCK2)
#define AT_BLOCK4(code)     AT_MAKE((code), FORM_BLOCK4)
#define AT_DATA2(code)      AT_MAKE((code), FORM_DATA2)
#define AT_DATA4(code)      AT_MAKE((code), FORM_DATA4)
#define AT_DATA8(code)      AT_MAKE((code), FORM_DATA8)
#define AT_STRING(code)     AT_MAKE((code), FORM_STRING)

// [INVENTED] Made up values...
#define AT_proc_body            (AT_lo_user      | FORM_REF)
#define AT_vtable_offset        (AT_lo_user+0x10 | FORM_DATA2)

// ---------------------------------------------------------------------

/* [INVENTED? Forgotten!] Type qualifier/modifier codes */
#define MOD_pointer_to 1
#define MOD_reference_to 2
#define MOD_volatile 3
#define MOD_const 4

#define OP_ADDR 1
#define OP_REG  2

// [INVENTED]
#define DW_OP_plus                       0x22   /* standard v2 */
#define DW_OP_const4u                    0x0a   /* standard v2 */
