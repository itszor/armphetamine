#ifndef EMIT2_H
#define EMIT2_H 1

#include "defs.h"
#include "pheta2.h"
#include "x86asm.h"

typedef enum {
  p86t_UNUSED,
  p86t_UNMAPPED,
  p86t_IMMEDIATE,
  p86t_REGISTER,
  p86t_MEMDISP8,
  p86t_MEMDISP32,
  p86t_MEMSI,
  p86t_MEMSIDISP8,
  p86t_MEMSIDISP32
} p86_datatype;

typedef struct {
  p86_datatype type;

  union {
    ph_operand unmapped;
  
    struct {
      uint5 value;
    } imm;

    struct {
      uint5 rnum;
    } reg;
    
    struct {
      uint5 rnum;
      uint3 displ;
    } md8;
    
    struct {
      uint5 rnum;
      uint5 displ;
    } md32;
    
    struct {
      uint5 base;
      uint5 index;
      uint5 scale;
    } msi;

    struct {
      uint5 base;
      uint5 index;
      uint5 scale;
      uint3 displ;
    } msid8;
    
    struct {
      uint5 base;
      uint5 index;
      uint5 scale;
      uint5 displ;
    } msid32;

  } data;
} p86_operand;

typedef struct {
  p86_opcode operator;
  p86_operand rd, rm;
} p86_node;

typedef p86_operand p86_mtype;

typedef enum {
  p86l_byte,
  p86l_word,
  p86l_long
} p86_oplength;

// binary operation types
typedef void (*x86f_rr)(nativeblockinfo* blk, uint5 len, uint5 src, uint5 dest);
typedef void (*x86f_ri)(nativeblockinfo* blk, uint5 len, uint5 imm, uint5 dest);
typedef void (*x86f_rm)(nativeblockinfo* blk, uint5 len, p86_mtype* src,
                        uint5 dest);
typedef void (*x86f_mr)(nativeblockinfo* blk, uint5 len, uint5 src,
                        p86_mtype* dest);
typedef void (*x86f_mi)(nativeblockinfo* blk, uint5 len, uint5 imm,
                        p86_mtype* dest);

// 1.5ary operation types (fixed operand, ie 'cl' register)
typedef void (*x86f_rx)(nativeblockinfo* blk, uint5 len, uint5 reg);
typedef void (*x86f_mx)(nativeblockinfo* blk, uint5 len, p86_mtype* mem);

// unary operation types
typedef void (*x86f_xr)(nativeblockinfo* blk, uint5 len, uint5 reg);
typedef void (*x86f_xm)(nativeblockinfo* blk, uint5 len, p86_mtype* mem);
typedef void (*x86f_xi)(nativeblockinfo* blk, uint5 len, uint5 imm);

// nullary operation types
typedef void (*x86f_xx)(nativeblockinfo* blk, uint5 len);

typedef struct {
  void*    ii;
  x86f_ri  ri;
  x86f_mi  mi;
  x86f_xi  xi;
  
  void*    ir;
  x86f_rr  rr;
  x86f_mr  mr;
  x86f_xr  xr;
  
  void*    im;
  x86f_rm  rm;
  void*    mm;
  x86f_xm  xm;
  
  void*    ix;
  x86f_rx  rx;
  x86f_mx  mx;
  x86f_xx  xx;
} p86_fns;

typedef struct {
  uint5 sets, destroys;
  p86_fns type;
} p86_table;

#include "emit2_fns.h"

#endif
