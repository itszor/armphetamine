#include "core/decode.h"

void thumb_dispatch(machineinfo* machine, uint4 thumbinst, insttab* action,
                    void* data)
{
  instructionformat inst;
  switch (thumbinst & 0xfc00)
  {
    case 0x0000:  // LSL by 5-bit immediate
    case 0x0400:
    inst.instruction = (cc_AL<<28)        // condition code
               | (0<<5)                   // shift type (LSL)
               | (((thumbinst>>6)&31)<<7) // shift amount
               | ((thumbinst&7)<<12)      // rd
               | ((thumbinst>>3)&7)       // rm
               | (1<<20)                  // 's' bit
               | (dp_MOV<<21);            // opcode
    action->dp(machine, inst, data);
    break;
    
    case 0x0800:  // LSR by 5-bit immediate
    case 0x0c00:
    inst.instruction = (cc_AL<<28)        // condition code
               | (1<<5)                   // shift type (LSR)
               | (((thumbinst>>6)&31)<<7) // shift amount
               | ((thumbinst&7)<<12)      // rd
               | ((thumbinst>>3)&7)       // rm
               | (1<<20)                  // 's' bit
               | (dp_MOV<<21);            // opcode
    action->dp(machine, inst, data);
    break;

    case 0x1000:  // ASR by 5-bit immediate
    case 0x1400:
    inst.instruction = (cc_AL<<28)        // condition code
               | (2<<5)                   // shift type (ASR)
               | (((thumbinst>>6)&31)<<7) // shift amount
               | ((thumbinst&7)<<12)      // rd
               | ((thumbinst>>3)&7)       // rm
               | (1<<20)                  // 's' bit
               | (1<<25)                  // immediate bit
               | (dp_MOV<<21);            // opcode
    action->dp(machine, inst, data);
    break;
    
    case 0x1800:  // ADD/SUB w/ 3 registers
    inst.instruction = (cc_AL<<28)        // condition code
               | (0<<5)                   // shift type (LSL)
               | (0<<7)                   // shift amount
               | ((thumbinst&7)<<12)      // rd
               | (((thumbinst>>3)&7)<<16) // rn
               | ((thumbinst>>6)&7)       // rm
               | (1<<20)                  // 's' bit
               | (((thumbinst&0x0200)?dp_SUB:dp_ADD)<<21); // opcode
    action->dp(machine, inst, data);
    break;
    
    case 0x2000:  // CMP w/ 8-bit immediate
    case 0x2400:
    inst.instruction = (cc_AL<<28)        // condition code
               | (((thumbinst>>8)&7)<<16) // rn
               | (thumbinst&0xff)         // immediate value
               | (1<<20)                  // 's' bit
               | (1<<25)                  // immediate bit
               | (dp_CMP<<21);            // opcode
    action->dp_imm(machine, inst, data);
    break;
    
    case 0x2800:  // MOV w/ 8-bit immediate
    case 0x2c00:
    inst.instruction = (cc_AL<<28)        // condition code
               | (((thumbinst>>8)&7)<<12) // rd
               | (thumbinst&0xff)         // immediate value
               | (1<<20)                  // 's' bit
               | (1<<25)                  // immediate bit
               | (dp_MOV<<21);            // opcode
    action->dp_imm(machine, inst, data);
    break;
    
    case 0x3000:  // ADD w/ 8-bit immediate
    case 0x3400:
    inst.instruction = (cc_AL<<28)        // condition code
               | (((thumbinst>>8)&7)<<12) // rd
               | (((thumbinst>>8)&7)<<16) // rm
               | (thumbinst&0xff)         // immediate value
               | (1<<20)                  // 's' bit
               | (1<<25)                  // immediate bit
               | (dp_ADD<<21);            // opcode
    action->dp_imm(machine, inst, data);
    break;
    
    case 0x3800:  // SUB w/ 8-bit immediate
    case 0x3c00:
    inst.instruction = (cc_AL<<28)        // condition code
               | (((thumbinst>>8)&7)<<12) // rd
               | (((thumbinst>>8)&7)<<16) // rm
               | (thumbinst&0xff)         // immediate value
               | (1<<20)                  // 's' bit
               | (1<<25)                  // immediate bit
               | (dp_SUB<<21);            // opcode
    action->dp_imm(machine, inst, data);
    break;
    
    case 0x4000:  // multiple ALU two-address instructions
    {
      switch (thumbinst&0x03c0)
      {
        case 0x0000:  // AND rd,rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | ((thumbinst>>3)&7)       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_AND<<21);            // opcode
        action->dp(machine, inst, data);
        break;

        case 0x0040:  // EOR rd,rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | ((thumbinst>>3)&7)       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_AND<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x0080:  // LSL rd,rs
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | (0<<5)                   // shift type (LSL)
                   | (1<<4)                   // register shift
                   | (((thumbinst>>3)&7)<<8)  // rs
                   | (1<<20)                  // 's' bit
                   | (dp_MOV<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x00c0:  // LSR rd,rs
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | (1<<5)                   // shift type (LSR)
                   | (1<<4)                   // register shift
                   | (((thumbinst>>3)&7)<<8)  // rs
                   | (1<<20)                  // 's' bit
                   | (dp_MOV<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x0100:  // ASR rd,rs
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | (2<<5)                   // shift type (ASR)
                   | (1<<4)                   // register shift
                   | (((thumbinst>>3)&7)<<8)  // rs
                   | (1<<20)                  // 's' bit
                   | (dp_MOV<<21);            // opcode
        action->dp(machine, inst, data);
        break;

        case 0x0140:  // ADC rd,rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | ((thumbinst>>3)&7)       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_ADC<<21);            // opcode
        action->dp(machine, inst, data);
        break;

        case 0x0180:  // SBC rd,rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | ((thumbinst>>3)&7)       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_SBC<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x01c0:  // ROR rd,rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rn
                   | (3<<5)                   // shift type (ROR)
                   | (1<<4)                   // register shift
                   | (((thumbinst>>3)&7)<<8)  // rs
                   | (1<<20)                  // 's' bit
                   | (dp_MOV<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x0200:  // TST rm,rn
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<16)      // rn
                   | ((thumbinst>>3)&7)       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_TST<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x0240:  // NEG rd,rm (==rsb rd,rm,#0, presumably)
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | (((thumbinst>>3)&7)<<12) // rn
                   | (0<<0)                   // immediate value
                   | (1<<20)                  // 's' bit
                   | (1<<25)                  // 'i' bit
                   | (dp_RSB<<21);            // opcode
        action->dp(machine, inst, data);
        break;

        case 0x0300:  // ORR rd,rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rm
                   | ((thumbinst>>3)&7)       // rn
                   | (1<<20)                  // 's' bit
                   | (dp_ORR<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x0340:  // MUL rd,rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<16)      // rd
                   | ((thumbinst&7)<<8)       // rs
                   | ((thumbinst>>3)&7)       // rm
                   | (1<<20)                  // 's' bit
                   | (9<<4);                  // bit of opcode
        action->mul(machine, inst, data);
        break;
        
        case 0x0380:  // BIC rd.rm
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst&7)<<16)      // rm
                   | ((thumbinst>>3)&7)       // rn
                   | (1<<20)                  // 's' bit
                   | (dp_BIC<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x03c0:  // MVN
        inst.instruction = (cc_AL<<28)        // condition code
                   | ((thumbinst&7)<<12)      // rd
                   | ((thumbinst>>3)&7)       // rn
                   | (1<<20)                  // 's' bit
                   | (dp_MVN<<21);            // opcode
        action->dp(machine, inst, data);
        break;
      }
    }
    break;
    
    case 0x4400:  // hi-register operations
    {
      uint5 rd = (thumbinst&3) | ((thumbinst&0x0080)?16:0);
      uint5 rm = ((thumbinst>>3)&3) | ((thumbinst&0x0040)?16:0);
      switch (thumbinst>>8)
      {
        case 0x0:  // add
        inst.instruction = (cc_AL<<28)        // condition code
                   | (rd<<12)                 // rd
                   | (rd<<16)                 // rn
                   | rm                       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_ADD<<21);            // opcode
        action->dp(machine, inst, data);
        break;

        case 0x1:  // cmp
        inst.instruction = (cc_AL<<28)        // condition code
                   | (rd<<16)                 // rn
                   | rm                       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_CMP<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x2:  // mov
        inst.instruction = (cc_AL<<28)        // condition code
                   | (rd<<12)                 // rn
                   | rm                       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_MOV<<21);            // opcode
        action->dp(machine, inst, data);
        break;
        
        case 0x3:  // bx
        inst.instruction = (cc_AL<<28)        // condition code
                   | (rd<<12)                 // rn
                   | rm                       // rm
                   | (1<<20)                  // 's' bit
                   | (dp_MOV<<21);            // opcode
        action->dp(machine, inst, data);
        break;
      }
    }
    break;
    
    case 0x4800:  // pc-relative load
    case 0x4c00:
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | (15<<16)                 // base register
                 | (((thumbinst>>8)&7)<<12) // dest register
                 | (1<<26)                  // opcode
                 | (1<<24)                  // pre-indexed
                 | (1<<23)                  // up not down
                 | (1<<20)                  // load not store
                 | ((thumbinst&0xff)<<2);   // immediate value
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x5000:  // str/strsh
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | ((thumbinst&7)<<12)      // rd
                 | (((thumbinst>>3)&7)<<16) // rn
                 | ((thumbinst>>6)&7)       // rm
                 | (1<<26)                  // opcode
                 | (1<<25)                  // register offset
                 | (1<<24)                  // pre not post
                 | (1<<23);                 // up not down
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x5400:  // strb/strsb
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | ((thumbinst&7)<<12)      // rd
                 | (((thumbinst>>3)&7)<<16) // rn
                 | ((thumbinst>>6)&7)       // rm
                 | (1<<26)                  // opcode
                 | (1<<25)                  // register offset
                 | (1<<24)                  // pre not post
                 | (1<<23)                  // up not down
                 | (1<<22);                 // byte not word
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x5800:  // ldr/ldrsh
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | ((thumbinst&7)<<12)      // rd
                 | (((thumbinst>>3)&7)<<16) // rn
                 | ((thumbinst>>6)&7)       // rm
                 | (1<<26)                  // opcode
                 | (1<<25)                  // register offset
                 | (1<<24)                  // pre not post
                 | (1<<23)                  // up not down
                 | (1<<20);                 // load not store
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x5c00:  // ldrb/ldrsb
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | ((thumbinst&7)<<12)      // rd
                 | (((thumbinst>>3)&7)<<16) // rn
                 | ((thumbinst>>6)&7)       // rm
                 | (1<<26)                  // opcode
                 | (1<<25)                  // register offset
                 | (1<<24)                  // pre not post
                 | (1<<23)                  // up not down
                 | (1<<22)                  // byte not word
                 | (1<<20);                 // load not store
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x6000:  // str (imm. offset)
    case 0x6400:
    {
      inst.instruction = (cc_AL<<28)           // condition code
                 | ((thumbinst&7)<<12)         // dest register
                 | (((thumbinst>>3)&7)<<16)    // base register
                 | (1<<26)                     // opcode
                 | (1<<24)                     // pre-indexed
                 | (1<<23)                     // up not down
                 | (((thumbinst>>6)&0x1f)<<2); // immediate value
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x6800:  // ldr (imm. offset)
    case 0x6c00:
    {
      inst.instruction = (cc_AL<<28)           // condition code
                 | ((thumbinst&7)<<12)         // dest register
                 | (((thumbinst>>3)&7)<<16)    // base register
                 | (1<<26)                     // opcode
                 | (1<<24)                     // pre-indexed
                 | (1<<23)                     // up not down
                 | (1<<20)                     // load not store
                 | (((thumbinst>>6)&0x1f)<<2); // immediate value
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x7000:  // strb (imm. offset)
    case 0x7400:
    {
      inst.instruction = (cc_AL<<28)           // condition code
                 | ((thumbinst&7)<<12)         // dest register
                 | (((thumbinst>>3)&7)<<16)    // base register
                 | (1<<26)                     // opcode
                 | (1<<24)                     // pre-indexed
                 | (1<<23)                     // up not down
                 | (1<<22)                     // byte not word
                 | (((thumbinst>>6)&0x1f)<<2); // immediate value
      action->sdt(machine, inst, data);
    }
    break;

    case 0x7800:  // ldrb (imm. offset)
    case 0x7c00:
    {
      inst.instruction = (cc_AL<<28)           // condition code
                 | ((thumbinst&7)<<12)         // dest register
                 | (((thumbinst>>3)&7)<<16)    // base register
                 | (1<<26)                     // opcode
                 | (1<<24)                     // pre-indexed
                 | (1<<23)                     // up not down
                 | (1<<22)                     // byte not word
                 | (1<<20)                     // load not store
                 | (((thumbinst>>6)&0x1f)<<2); // immediate value
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x8000:  // strh (imm. offset)
    case 0x8400:
    {
    }
    break;
    
    case 0x8800:  // ldrh (imm. offset)
    case 0x8c00:
    {
    }
    break;
    
    case 0x9000:  // ldr (sp relative offset)
    case 0x9400:
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | (13<<16)                 // base register
                 | (((thumbinst>>8)&7)<<12) // dest register
                 | (1<<26)                  // opcode
                 | (1<<24)                  // pre-indexed
                 | (1<<23)                  // up not down
                 | (1<<20)                  // load not store
                 | ((thumbinst&0xff)<<2);   // immediate value
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0x9800:  // str (sp relative offset)
    case 0x9c00:
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | (13<<16)                 // base register
                 | (((thumbinst>>8)&7)<<12) // dest register
                 | (1<<26)                  // opcode
                 | (1<<24)                  // pre-indexed
                 | (1<<23)                  // up not down
                 | ((thumbinst&0xff)<<2);   // immediate value
      action->sdt(machine, inst, data);
    }
    break;
    
    case 0xa000:  // add rd, pc, imm
    case 0xa400:
    inst.instruction = (cc_AL<<28)        // condition code
               | (((thumbinst>>8)&7)<<12) // rd
               | (15<<16)                 // rm
               | (thumbinst&0xff)         // immediate value
               | (1<<25)                  // immediate bit
               | (dp_ADD<<21);            // opcode
    action->dp_imm(machine, inst, data);
    break;
    
    case 0xa800:  // add rd, sp, imm
    case 0xac00:
    inst.instruction = (cc_AL<<28)        // condition code
               | (((thumbinst>>8)&7)<<12) // rd
               | (13<<16)                 // rm
               | (thumbinst&0xff)         // immediate value
               | (1<<25)                  // immediate bit
               | (dp_ADD<<21);            // opcode
    action->dp_imm(machine, inst, data);
    break;
    
    case 0xb000:  // sp immediate add/subtract
    case 0xb800:
    {
      inst.instruction = (cc_AL<<28)                     // condition code
                 | (13<<12)                              // rd
                 | (13<<16)                              // rn
                 | (thumbinst&0x7f)                      // rm
                 | (15<<8)                               // rotate
                 | (1<<25)                               // immediate bit
                 | (((thumbinst&0x80)?dp_SUB:dp_ADD)<<21); // opcode
      action->dp_imm(machine, inst, data);
    }
    break;
    
    case 0xb400:  // push
    {
      inst.instruction = (cc_AL<<28)      // condition code
                 | (1<<27)                // opcode
                 | (1<<24)                // predecrement
                 | (1<<21)                // writeback
                 | (13<<16)               // base register
                 | (thumbinst&0xff)       // register list
                 | ((thumbinst&0x100)?(1<<14):0); // optionally store LR
      action->bdt(machine, inst, data);
    }
    break;
    
    case 0xbc00:  // pop
    {
      inst.instruction = (cc_AL<<28)      // condition code
                 | (1<<27)                // opcode
                 | (1<<23)                  // up not down
                 | (1<<21)                // writeback
                 | (1<<20)                // load not store
                 | (13<<16)               // base register
                 | (thumbinst&0xff)       // register list
                 | ((thumbinst&0x100)?(1<<15):0); // optionally load PC
      action->bdt(machine, inst, data);
    }
    break;
    
    case 0xc000:  // stmia
    case 0xc400:
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | (1<<27)                  // opcode
                 | (1<<23)                  // up not down
                 | (1<<21)                  // writeback
                 | (((thumbinst>>8)&7)<<16) // base register
                 | (thumbinst&0xff);        // register list
      action->bdt(machine, inst, data);
    }
    break;
    
    case 0xc800:  // ldmia
    case 0xcc00:
    {
      inst.instruction = (cc_AL<<28)        // condition code
                 | (1<<27)                  // opcode
                 | (1<<23)                  // up not down
                 | (1<<21)                  // writeback
                 | (1<<20)                  // load not store
                 | (((thumbinst>>8)&7)<<16) // base register
                 | (thumbinst&0xff);        // register list
      action->bdt(machine, inst, data);
    }
    break;
    
    case 0xd000:  // b/swi
    case 0xd400:
    case 0xd800:
    case 0xdc00:
    {
      sint5 offset = (((sint5)thumbinst)<<24)>>8;
      inst.instruction = (((thumbinst>>8)&15)<<28)
                 | (offset & 0x00ffffff);
      action->bra(machine, inst, data);
    }
    break;
    
    case 0xe000:  // b immediate offset
    case 0xe400:
    {
      sint5 offset = (((sint5)thumbinst)<<21)>>5;
      inst.instruction = (cc_AL<<28)
                 | (offset & 0x00ffffff);
      action->bra(machine, inst, data);
    }
    break;
    
    case 0xe800:  // undefined instruction
    case 0xec00:
    {
      action->und(machine, inst, data);
    }
    break;
    
    case 0xf000:  // bl target addr (+)
    case 0xf400:
    case 0xf800:  // bl target addr
    case 0xfc00:
    {
      inst.instruction = thumbinst;
      action->thumbl(machine, inst, data);
    }
    break;
  }
}
