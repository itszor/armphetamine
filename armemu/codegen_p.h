#ifndef CODEGEN_P_H
#define CODEGEN_P_H 1

// extern x86node* codegen_newnode(void);
extern void codegen_merge(hashtable* ht, gen_fn rule, ...);
extern uint5 codegen_rulify(hashtable* root, hashtable* itree, blockinfo* blk, 
                            gencontext* ctxt, uint5 srcptr);
extern uint5 codegen_match(hashtable* root, hashtable* itree, blockinfo* blk, 
                           gencontext* ctxt, uint5 srcptr, gen_fn* rulefn, 
                           hashtable** writeback);
extern void codegen_recompile(machineinfo* machine, blockinfo* blk);
extern void codegen_matchblock(machineinfo* machine, blockinfo* blk);
extern void codegen_inner(machineinfo* machine, blockinfo* blk, uint5 start,
                          basicblockinfo* basic, allocationinfo* alloc);
extern sint5 codegen_getinteladdr(hashtable* basicmap, uint5 psinst,
                                  uint5 patch);
extern void codegen_addmapping(hashtable* basicmap, uint5 psinst, uint3* base,
                               sint5 ioffset);


#endif
