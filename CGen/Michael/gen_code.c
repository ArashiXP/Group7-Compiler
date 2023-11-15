/* $Id: gen_code.c,v 1.10 2023/03/30 21:28:07 leavens Exp $ */
#include "utilities.h"
#include "gen_code.h"
#include "code.h"
#include "ast.h"

// Initialize the code generator
void gen_code_initialize()
{
    // nothing to do!
}

// Requires: bf if open for writing in binary
// Generate code for prog into bf
void gen_code_program(BOFFILE bf, block_t prog)
{
    code_seq blockCode = gen_code_block(prog);
    code_seq_write(bf, blockCode);
}

// Requires: bf if open for writing in binary
// Generate code for the given AST
code_seq gen_code_block(block_t blk)
{
    // Generate code for const declarations
    code_seq ret = gen_code_const_decls(blk.const_decls);

    // Generate code for var declarations
    ret = code_seq_concat(ret, gen_code_var_decls(blk.var_decls));

    // Generate code for procedure declarations
    ret = code_seq_concat(ret, gen_code_proc_decls(blk.proc_decls));

    // Generate code for the statement
    ret = (ret, gen_code_stmt(blk.stmt));

    // Concatenate the generated code sequences
    return ret;
}

// Generate code for the const-decls, cds
// There are 3 instructions generated for each identifier declared
// (one to allocate space and two to initialize that space)
code_seq gen_code_const_decls(const_decls_t cds)
{

}

// Generate code for the const-decl, cd
code_seq gen_code_const_decl(const_decl_t cd)
{
    
}

// Generate code for the const-defs, cdfs
code_seq gen_code_const_defs(const_defs_t cdfs)
{

}

// Generate code for the const-def, cdf
code_seq gen_code_const_def(const_def_t cdf)
{

}

// Generate code for the var_decls_t vds to out
// There are 2 instructions generated for each identifier declared
// (one to allocate space and another to initialize that space)
code_seq gen_code_var_decls(var_decls_t vds)
{

}

// Generate code for a single <var-decl>, vd,
// There are 2 instructions generated for each identifier declared
// (one to allocate space and another to initialize that space)
code_seq gen_code_var_decl(var_decl_t vd)
{

}

// Generate code for the identififers in idents
// in reverse order (so the first declared are allocated last).
// There are 2 instructions generated for each identifier declared
// (one to allocate space and another to initialize that space)
code_seq gen_code_idents(idents_t idents)
{

}

// (Stub for:) Generate code for the procedure declarations
code_seq gen_code_proc_decls(proc_decls_t pds)
{

}

// (Stub for:) Generate code for a procedure declaration
code_seq gen_code_proc_decl(proc_decl_t pd)
{

}

// Generate code for stmt
code_seq gen_code_stmt(stmt_t stmt)
{

}

// Generate code for stmt
code_seq gen_code_assign_stmt(assign_stmt_t stmt)
{

}

// Generate code for stmt
code_seq gen_code_call_stmt(call_stmt_t stmt)
{

}

// Generate code for stmt
code_seq gen_code_begin_stmt(begin_stmt_t stmt)
{

}

// Generate code for the list of statments given by stmts
code_seq gen_code_stmts(stmts_t stmts)
{

}

// Generate code for the if-statment given by stmt
code_seq gen_code_if_stmt(if_stmt_t stmt)
{

}

// Generate code for the if-statment given by stmt
code_seq gen_code_while_stmt(while_stmt_t stmt)
{

}

// Generate code for the read statment given by stmt
code_seq gen_code_read_stmt(read_stmt_t stmt)
{

}

// Generate code for the write statment given by stmt.
code_seq gen_code_write_stmt(write_stmt_t stmt)
{

}

// Generate code for the skip statment, stmt
code_seq gen_code_skip_stmt(skip_stmt_t stmt)
{

}

// Requires: reg != T9
// Generate code for cond, putting its truth value
// on top of the runtime stack
// and using V0 and AT as temporary registers
// May modify HI,LO when executed
code_seq gen_code_condition(condition_t cond)
{

}

// Generate code for cond, putting its truth value
// on top of the runtime stack
// and using V0 and AT as temporary registers
// Modifies SP, HI,LO when executed
code_seq gen_code_odd_condition(odd_condition_t cond)
{

}

// Generate code for cond, putting its truth value
// on top of the runtime stack
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_rel_op_condition(rel_op_condition_t cond)
{

}

// Generate code for the rel_op
// applied to 2nd from top and top of the stack,
// putting the result on top of the stack in their place,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_rel_op(token_t rel_op)
{

}

// Generate code for the expression exp
// putting the result on top of the stack,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_expr(expr_t exp)
{

}

// Generate code for the expression exp
// putting the result on top of the stack,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_binary_op_expr(binary_op_expr_t exp)
{

}

// Generate code to apply arith_op to the
// 2nd from top and top of the stack,
// putting the result on top of the stack in their place,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_arith_op(token_t arith_op)
{

}

// Generate code to put the value of the given identifier
// on top of the stack
// Modifies T9, V0, and SP when executed
code_seq gen_code_ident(ident_t id)
{

}

// Generate code to put the given number on top of the stack
code_seq gen_code_number(number_t num)
{

}