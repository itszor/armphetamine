#ifndef PHETA2_H
#define PHETA2_H 1

#include "defs.h"
#include "list.h"
#include "hash.h"
#include "decode.h"
#include "codegen.h"

typedef enum {
  pht_UNUSED,
  pht_CONSTANT,
  pht_REGISTER,
  pht_ALIAS,
  pht_TEMPORARY,
  pht_FLAG,
  pht_REGLIST   // for multiword loads/stores
} pht_datatype;

typedef struct {
  pht_datatype type;
  uint5 id;
} ph_operand;

/*

         +
        / \
       a   -
          / \
         b   c

*/

typedef union {
  struct {
    uint5 eq     : 1;
    uint5 ne     : 1;
    uint5 cs     : 1;
    uint5 cc     : 1;
    uint5 mi     : 1;
    uint5 pl     : 1;
    uint5 vs     : 1;
    uint5 vc     : 1;
    uint5 hi     : 1;
    uint5 ls     : 1;
    uint5 ge     : 1;
    uint5 lt     : 1;
    uint5 gt     : 1;
    uint5 le     : 1;
    uint5 al     : 1;
    uint5 nv     : 1;
    uint5 aux_eq : 1;
    uint5 aux_ne : 1;
    uint5 aux_cs : 1;
    uint5 aux_cc : 1;
    uint5 aux_mi : 1;
    uint5 aux_pl : 1;
    uint5 aux_vs : 1;
    uint5 aux_vc : 1;
    uint5 aux_hi : 1;
    uint5 aux_ls : 1;
    uint5 aux_ge : 1;
    uint5 aux_lt : 1;
    uint5 aux_gt : 1;
    uint5 aux_le : 1;
    uint5 aux_al : 1;
    uint5 aux_nv : 1;
  } ccode;
  uint5 value;
} ccodemask;

// single-flag conditions
#define EQCODE 0x0001
#define ZSCODE 0x0001
#define NECODE 0x0002
#define ZCCODE 0x0002
#define CSCODE 0x0004
#define CCCODE 0x0008
#define MICODE 0x0010
#define NSCODE 0x0010
#define PLCODE 0x0020
#define NCCODE 0x0020
#define VSCODE 0x0040
#define VCCODE 0x0080

// multi-flag conditions
#define HICODE 0x0100
#define LSCODE 0x0200
#define GECODE 0x0400
#define LTCODE 0x0800
#define GTCODE 0x1000
#define LECODE 0x2000
#define ALCODE 0x4000
#define NVCODE 0x8000

// everything (stripped afterwards?)
#define ALLCODES 0xffff

// auxiliary bit
#define AUXCODE 0x10000

#define CAFFECT (CSCODE|CCCODE|HICODE|LSCODE)
#define VAFFECT (VSCODE|VCCODE|GECODE|LTCODE|GTCODE|LECODE)
#define NAFFECT (MICODE|PLCODE|GECODE|LTCODE|GTCODE|LECODE)
#define ZAFFECT (EQCODE|NECODE|HICODE|LSCODE|GTCODE|LECODE)

typedef struct {
  uint3 operator;
  ph_operand rd, rm, rn, ro;
  ccodemask sets, requires, destroys;
} ph_node;

typedef struct {
  ph_node* base;
  uint5 length, size;
  uint5 tempno;
} ph_block;

// if ORred with a psopcode, reads an extra 'flags affected' arg in pseudo_emit
#define PSSETFLAGS (1U<<7)
#define NOFLAGS (1U<<8)
#define CFLAG  0x01
#define VFLAG  0x02
#define ZFLAG  0x04
#define NFLAG  0x08
#define ICFLAG 0x10
#define ALLFLAGS 15

#endif
