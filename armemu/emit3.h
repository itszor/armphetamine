/*C++*/
#ifndef EMIT3_H
#define EMIT3_H 1

typedef enum {
  p86_ADC,
  p86_ADD,
  p86_AND,
  p86_BT,
  p86_CALL,
  p86_CLC,
  p86_CMC,
  p86_CMP,
  p86_DEC,
  p86_IMUL,
  p86_INC,
  p86_JA,
  p86_JAE,
  p86_JB,
  p86_JBE,
  p86_JC,
  p86_JE,
  p86_JG,
  p86_JGE,
  p86_JL,
  p86_JLE,
  p86_JNA,
  p86_JNB,
  p86_JNBE,
  p86_JNC,
  p86_JNE,
  p86_JNG,
  p86_JNGE,
  p86_JNL,
  p86_JNLE,
  p86_JNO,
  p86_JNP,
  p86_JNS,
  p86_JNZ,
  p86_JO,
  p86_JP,
  p86_JPE,
  p86_JPO,
  p86_JS,
  p86_JZ,
  p86_JMP,
  p86_MOV,
  p86_NEG,
  p86_NOP,
  p86_NOT,
  p86_OR,
  p86_POP,
  p86_POPF,
  p86_PUSH,
  p86_PUSHF,
  p86_RCL,
  p86_RCR,
  p86_RET,
  p86_ROL,
  p86_ROR,
  p86_SHL,
  p86_SAR,
  p86_SBB,
  p86_SETA,
  p86_SETAE,
  p86_SETB,
  p86_SETBE,
  p86_SETC,
  p86_SETE,
  p86_SETG,
  p86_SETGE,
  p86_SETL,
  p86_SETLE,
  p86_SETNA,
  p86_SETNB,
  p86_SETNBE,
  p86_SETNC,
  p86_SETNE,
  p86_SETNG,
  p86_SETNGE,
  p86_SETNL,
  p86_SETNLE,
  p86_SETNO,
  p86_SETNP,
  p86_SETNS,
  p86_SETNZ,
  p86_SETO,
  p86_SETP,
  p86_SETPE,
  p86_SETPO,
  p86_SETS,
  p86_SETZ,
  p86_SHR,
  p86_STC,
  p86_SUB,
  p86_TEST,
  p86_XADD,
  p86_XCHG,
  p86_XOR
} p86_opcode;

class p86_node {
  p86_opcode op;
  ph_operand rd, rs;
  uint5 sets, requires, destroys;

public:
  p86_node* next;

  p86_node(p86_opcode o, uint5 sets, uint5 requires);
  p86_node(p86_opcode o, uint5 sets, uint5 requires, ph_operand rn);
  p86_node(p86_opcode o, uint5 sets, uint5 requires, ph_operand rs,
           ph_operand rd);
};

class p86_block {
  p86_node* base, *end;
  list* reloc;

private:
  void emit(p86_node*);

public:
  p86_block();
  ~p86_block();
  void translate(ph_block* in);
}; 

#endif
