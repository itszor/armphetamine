/*C++*/
#ifndef PHETA3_H
#define PHETA3_H 1

#include "defs.h"
#include "block.h"

class ph_node;

class ph_operand
{
public:
  ph_operand(void) {}
  ~ph_operand(void) {}
};

class ph_null : public ph_operand
{
public:
  ph_null() {}
};

class ph_constant : public ph_operand
{
  uint5 value;

public:
  ph_constant(uint5 v) { value = v; }
  void set(uint5 v) { value = v; }
  uint5 get() { return value; }
};

class ph_register : public ph_operand
{
  uint5 number;

public:
  ph_register(uint5 n) { number = n; }
  void set(uint5 n) { number = n; }
  uint5 get() { return number; }
};

class ph_temporary : public ph_operand
{
  uint5 number;

public:
  ph_temporary(uint5 n) { number = n; }
  void set(uint5 n) { number = n; }
  uint5 get() { return number; }
};

class ph_flag : public ph_operand
{
  uint5 number;

public:
  ph_flag(uint5 n) { number = n; }
  void set(uint5 n) { number = n; }
  uint5 get() { return number; }
};

class ph_reglist : public ph_operand
{
  uint5 regmask;
  
public:
  ph_reglist(uint5 rm) { regmask = rm; }
  void set(uint5 rm) { regmask = rm; }
  uint5 get() { return regmask; }
};

class ph_address : public ph_operand
{
  ph_node* address;

public:
  ph_address(ph_address** x) { address = 0; *x = this; }  // erk
  void set(ph_node* x) { address = x; }
  ph_node* get() { return address; }
};

typedef union {
  struct {
    uint5 eq       : 1;
    uint5 ne       : 1;
    uint5 cs       : 1;
    uint5 cc       : 1;
    uint5 mi       : 1;
    uint5 pl       : 1;
    uint5 vs       : 1;
    uint5 vc       : 1;
    uint5 hi       : 1;
    uint5 ls       : 1;
    uint5 ge       : 1;
    uint5 lt       : 1;
    uint5 gt       : 1;
    uint5 le       : 1;
    uint5 al       : 1;
    uint5 nv       : 1;
    uint5 aux      : 1;
    uint5 realc    : 1;
    uint5 realinvc : 1;
    uint5 unused   : 13;
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
#define AUXCODE      0x10000
#define REALCCODE    0x20000
#define REALINVCCODE 0x40000

#define ZFLG 0x03
#define CFLG 0x0c
#define NFLG 0x30
#define VFLG 0xc0

#define CAFFECT (CSCODE|CCCODE|HICODE|LSCODE)
#define VAFFECT (VSCODE|VCCODE|GECODE|LTCODE|GTCODE|LECODE)
#define NAFFECT (MICODE|PLCODE|GECODE|LTCODE|GTCODE|LECODE)
#define ZAFFECT (EQCODE|NECODE|HICODE|LSCODE|GTCODE|LECODE)

class ph_node
{
  uint3 op;
  ph_operand rd, rm, rn;
  uint5 sets, requires;

public:
  ph_node* next;

  ph_node(uint3 oper, uint5 s, uint5 r);
  ph_node(uint3 oper, uint5 s, uint5 r, ph_operand d1);
  ph_node(uint3 oper, uint5 s, uint5 r, ph_operand d1, ph_operand d2);
  ph_node(uint3 oper, uint5 s, uint5 r, ph_operand d1, ph_operand d2,
          ph_operand d3);
  uint3 getop() { return op; }
  ph_operand getrd() { return rd; }
  ph_operand getrm() { return rm; }
  ph_operand getrn() { return rn; }
  uint5 getsetcc() { return sets; }
  uint5 getrequiredcc() { return requires; }
};

class ph_block
{
  ph_node* base, *end;
  list* writeback;
  uint5 tempno;

private:
  void emit(ph_node*);
  void comefrom(ph_address*);
  void dp(machineinfo*, instructionformat);
  void dp_imm(machineinfo*, instructionformat);
  void dp_imm_guts(machineinfo*, instructionformat, ph_operand);
  void mul(machineinfo*, instructionformat);
  void sdt(machineinfo*, instructionformat);
  void bdt(machineinfo*, instructionformat);
  void bra(machineinfo*, instructionformat);
  void swi(machineinfo*, instructionformat);
  void cdt(machineinfo*, instructionformat);
  void cdo(machineinfo*, instructionformat);
  void crt(machineinfo*, instructionformat);
  void sds(machineinfo*, instructionformat);
  void und(machineinfo*, instructionformat);

public:
  ph_block();
  ~ph_block();
  void translate(machineinfo* machine, blockinfo* block,
                 instructionformat* base);
  ph_node* start() { return base; }
  uint5 mktemp() { return tempno++; }
};

#define _T(X) ph_temporary(X)
#define _R(X) ph_register(X)
#define _C(X) ph_constant(X)
#define _F(X) ph_flag(X)
#define _A(X) ph_address(X)

#endif
