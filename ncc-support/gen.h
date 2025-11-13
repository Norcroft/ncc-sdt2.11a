//
//  gen.h
//  ncc
//
//  Created by Piers Wombwell on 01/11/2025.
//

#pragma once

#include "mcdpriv.h"

typedef enum {
  UseFP,
  UseSP_Adjust,
  UseSP_NoAdjust
} FP_RestoreBase;

typedef struct FP_Gen {
  void (*show)(PendingOp const *p);
  void (*calleesave)(int32 mask);
  int32 (*restoresize)(int32 mask);
  void (*calleerestore)(int32 mask, int32 condition, FP_RestoreBase base, int32 offset);
  void (*saveargs)(int32);
} FP_Gen;

struct DispDesc { int32 u_d, m; RealRegister r; };

struct InlineTable { char *name; Symstr const *sym; int32 op; };

#define LABREF_BRANCH 0x00000000  /* ARM addressing modes for forw. refs. */
#define LABREF_B4096  0x01000000
#define LABREF_W256   0x02000000
#ifdef TARGET_HAS_DATA_VTABLES
#define LABREF_WORD32 0x03000000
#endif

// Make ARM-specific static functions external. This does limit VFP to ARM...
// (thumb has its own of the same name)
extern void arm_addressability(int32 n);
extern void arm_ldm_flush(void);
extern void arm_outinstr(int32 w);
extern void arm_fpdesc_notespchange(int32 n);
extern void arm_out_ldm_instr(uint32 w);
extern void arm_adjustipvalue(RealRegister r1, RealRegister r2, int32 k);
extern void arm_KillKnownRegisterValues(uint32 mask);
extern void arm_bigdisp(struct DispDesc *x, int32 m, int32 mask, RealRegister r2r);
extern void arm_gen_add_integer(RealRegister r1, RealRegister r2, int32 n, int32 flags);

extern bool is_condition_always();
extern void outinstr_vfp_simd(int32 w);

extern int32 arm_intsavewordsbelowfp();
extern int32 arm_realargwordsbelowfp();
