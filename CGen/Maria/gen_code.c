/* $Id: gen_code.c,v 1.10 2023/03/30 21:28:07 leavens Exp $ */
#include <string.h>
#include "utilities.h"
#include "gen_code.h"
#include "code.h"
#include "ast.h"
#include "literal_table.h"
#include "id_use.h"
#include "utilities.h"
#include "regname.h"

#define STACK_SPACE 4096

// adding new functions
// Requires: bf if open for writing in binary
// and prior to this scope checking and type checking have been done.
// Write all the instructions in cs to bf in order
void gen_code_output_seq(BOFFILE bf, code_seq cs)
{
    while (!code_seq_is_empty(cs))
    {
        bin_instr_t inst = code_seq_first(cs)->instr;
        instruction_write_bin_instr(bf, inst);
        cs = code_seq_rest(cs);
    }
}

// Return a header appropriate for the give code
BOFHeader gen_code_program_header(code_seq main_cs)
{
    BOFHeader ret;
    strncpy(ret.magic, "FBF", 4); // for FLOAT SRM
    ret.text_start_address = 0;
    // remember, the unit of length in the BOF format is a byte!
    ret.text_length = code_seq_size(main_cs) * BYTES_PER_WORD;
    int dsa = MAX(ret.text_length, 1024);
    ret.data_start_address = dsa;
    int sba = dsa + ret.data_start_address + STACK_SPACE;
    ret.stack_bottom_addr = sba;
    return ret;
}

void gen_code_output_program(BOFFILE bf, code_seq main_cs)
{
    BOFHeader bfh = gen_code_program_header(main_cs);
    bof_write_header(bf, bfh);
    gen_code_output_seq(bf, main_cs);
    bof_close(bf);
}

// Initialize the code generator
void gen_code_initialize()
{
    literal_table_initialize();
}

// Requires: bf if open for writing in binary
// Generate code for prog into bf
void gen_code_program(BOFFILE bf, block_t prog)
{
    code_seq blockCode = gen_code_block(prog);
    gen_code_output_program(bf, blockCode);
}

// Requires: bf if open for writing in binary
// Generate code for the given AST
code_seq gen_code_block(block_t blk)
{
    // Generate code for const declarations
    code_seq ret = code_seq_concat(ret, gen_code_var_decls(blk.var_decls));

    // Generate code for var declarations
    // ret = gen_code_const_decls(blk.const_decls);

    // Generate code for procedure declarations
    // ret = code_seq_concat(ret, gen_code_proc_decls(blk.proc_decls));

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
    bail_with_error("TODO: no implementation of gen_code_const_decls yet!");
    return code_seq_empty();
}

// Generate code for the const-decl, cd
code_seq gen_code_const_decl(const_decl_t cd)
{
    bail_with_error("TODO: no implementation of gen_code_const_decl yet!");
    return code_seq_empty();
}

// Generate code for the const-defs, cdfs
code_seq gen_code_const_defs(const_defs_t cdfs)
{
    bail_with_error("TODO: no implementation of gen_code_const_defs yet!");
    return code_seq_empty();
}

// Generate code for the const-def, cdf
code_seq gen_code_const_def(const_def_t cdf)
{
    bail_with_error("TODO: no implementation of gen_code_const_def yet!");
    return code_seq_empty();
}

// Generate code for the var_decls_t vds to out
// There are 2 instructions generated for each identifier declared
// (one to allocate space and another to initialize that space)
code_seq gen_code_var_decls(var_decls_t vds)
{
    code_seq ret = code_seq_empty();
    var_decl_t *vdp = vds.var_decls;
    while (vdp != NULL)
    {
        // generate these in reverse order,
        // so the addressing offsets work properly
        ret = code_seq_concat(gen_code_var_decl(*vdp), ret);
        vdp = vdp->next;
    }
    return ret;
}

// Generate code for a single <var-decl>, vd,
// There are 2 instructions generated for each identifier declared
// (one to allocate space and another to initialize that space)
code_seq gen_code_var_decl(var_decl_t vd)
{
    return gen_code_idents(vd.idents);
}

// Generate code for the identififers in idents
// in reverse order (so the first declared are allocated last).
// There are 2 instructions generated for each identifier declared
// (one to allocate space and another to initialize that space)
code_seq gen_code_idents(idents_t idents)
{
    code_seq ret = code_seq_empty();
    ident_t *idp = idents.idents;
    while (idp != NULL)
    {
        code_seq alloc_and_init = code_seq_singleton(code_addi(SP, SP, -BYTES_PER_WORD));

        alloc_and_init = code_seq_add_to_end(alloc_and_init, code_sw(SP, 0, 0));

        // Generate these in revese order,
        // so addressing works propertly
        ret = code_seq_concat(alloc_and_init, ret);
        idp = idp->next;
    }
    return ret;
}

// (Stub for:) Generate code for the procedure declarations
code_seq gen_code_proc_decls(proc_decls_t pds)
{
    bail_with_error("TODO: no implementation of gen_code_proc_decls yet!");
    return code_seq_empty();
}

// (Stub for:) Generate code for a procedure declaration
code_seq gen_code_proc_decl(proc_decl_t pd)
{
    bail_with_error("TODO: no implementation of gen_code_proc_decl yet!");
    return code_seq_empty();
}

// Generate code for stmt
code_seq gen_code_stmt(stmt_t stmt)
{

    // there might be some statements missing (check later)

    switch (stmt.stmt_kind)
    {
    case assign_stmt:
        return gen_code_assign_stmt(stmt.data.assign_stmt);
        break;
    case begin_stmt:
        return gen_code_begin_stmt(stmt.data.begin_stmt);
        break;
    case if_stmt:
        return gen_code_if_stmt(stmt.data.if_stmt);
        break;
    case read_stmt:
        return gen_code_read_stmt(stmt.data.read_stmt);
        break;
    case write_stmt:
        return gen_code_write_stmt(stmt.data.write_stmt);
        break;
    case call_stmt:
        return gen_code_call_stmt(stmt.data.call_stmt);
        break;
    case skip_stmt:
        return gen_code_skip_stmt(stmt.data.skip_stmt);
        break;
    case while_stmt:
        return gen_code_while_stmt(stmt.data.while_stmt);
        break;
    default:
        bail_with_error("Call to gen_code_stmt with an AST that is not a statement!");
        break;
    }
    // The following can never execute, but this quiets gcc's warning
    return code_seq_empty();
}

// Generate code for stmt
code_seq gen_code_assign_stmt(assign_stmt_t stmt)
{
    bail_with_error("TODO: no implementation of gen_code_assign_stmt yet!");
    return code_seq_empty();
}

// Generate code for stmt
code_seq gen_code_call_stmt(call_stmt_t stmt)
{
    bail_with_error("TODO: no implementation of gen_code_call_stmt yet!");
    return code_seq_empty();
}

// Generate code for stmt
code_seq gen_code_begin_stmt(begin_stmt_t stmt)
{
    bail_with_error("TODO: no implementation of gen_code_begin_stmt yet!");
    return code_seq_empty();
}

// Generate code for the list of statments given by stmts
code_seq gen_code_stmts(stmts_t stmts)
{
    code_seq ret = code_seq_empty();
    stmt_t *sp = stmts.stmts;
    while (sp != NULL)
    {
        ret = code_seq_concat(ret, gen_code_stmt(*sp));
        sp = sp->next;
    }
    return ret;
}

// Generate code for the if-statment given by stmt
code_seq gen_code_if_stmt(if_stmt_t stmt)
{
    bail_with_error("TODO: no implementation of gen_code_if_stmt yet!");
    return code_seq_empty();
}

// Generate code for the if-statment given by stmt
code_seq gen_code_while_stmt(while_stmt_t stmt)
{
    bail_with_error("TODO: no implementation of gen_code_while_stmt yet!");
    return code_seq_empty();
}

// Generate code for the read statment given by stmt
code_seq gen_code_read_stmt(read_stmt_t stmt)
{
    bail_with_error("TODO: no implementation of gen_code_read_stmt yet!");
    return code_seq_empty();
}

// Generate code for the write statment given by stmt.
code_seq gen_code_write_stmt(write_stmt_t stmt)
{
    // put the result of stmt.expr into $a0 to get ready for PCH
    // PCH:(syscal)
    // ret contains the code to compute the expression.
    code_seq ret = gen_code_expr(stmt.expr);

    // code_pop_stack_into_reg() generates code to pop the top of the stack into register.
    // Then code_seq_concat() prepares the results of the expr to be passed as an argument to a print function

    ret = code_seq_concat(ret, code_pop_stack_into_reg(A0));
    // code_seq_add_to_end() adds the print instruction to the end of the code
    ret = code_seq_add_to_end(ret, code_pint());
    return ret;
}

// Generate code for the skip statment, stmt
code_seq gen_code_skip_stmt(skip_stmt_t stmt)
{
    return code_seq_empty();
}

// Requires: reg != T9
// Generate code for cond, putting its truth value
// on top of the runtime stack
// and using V0 and AT as temporary registers
// May modify HI,LO when executed
code_seq gen_code_condition(condition_t cond)
{
    bail_with_error("TODO: no implementation of gen_code_code_condition yet!");
    return code_seq_empty();
}

// Generate code for cond, putting its truth value
// on top of the runtime stack
// and using V0 and AT as temporary registers
// Modifies SP, HI,LO when executed
code_seq gen_code_odd_condition(odd_condition_t cond)
{
    bail_with_error("TODO: no implementation of gen_code_odd_condition yet!");
    return code_seq_empty();
}

// Generate code for cond, putting its truth value
// on top of the runtime stack
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_rel_op_condition(rel_op_condition_t cond)
{
    bail_with_error("TODO: no implementation of gen_code_rel_op_condition yet!");
    return code_seq_empty();
}

// Generate code for the rel_op
// applied to 2nd from top and top of the stack,
// putting the result on top of the stack in their place,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_rel_op(token_t rel_op)
{
    bail_with_error("TODO: no implementation of gen_code_rel_op yet!");
    return code_seq_empty();
}

// Generate code for the expression exp
// putting the result on top of the stack,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_expr(expr_t exp)
{
    switch (exp.expr_kind)
    {
    case expr_bin:
        return gen_code_binary_op_expr(exp.data.binary);
        break;
    case expr_ident:
        return gen_code_ident(exp.data.ident);
        break;
    case expr_number:
        return gen_code_number(exp.data.number);
        break;
    default:
        bail_with_error("Unexpected expr_kind_e (%d) in gen_code_expr", exp.expr_kind);
        break;
    }
    // never happens, but suppresses a warning from gcc
    return code_seq_empty();
}

// Generate code for the expression exp
// putting the result on top of the stack,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_binary_op_expr(binary_op_expr_t exp)
{
    bail_with_error("TODO: no implementation of gen_code_binary_op_expr yet!");
    return code_seq_empty();
}

// Generate code to apply arith_op to the
// 2nd from top and top of the stack,
// putting the result on top of the stack in their place,
// and using V0 and AT as temporary registers
// May also modify SP, HI,LO when executed
code_seq gen_code_arith_op(token_t arith_op)
{
    bail_with_error("TODO: no implementation of gen_code_arith_op yet!");
    return code_seq_empty();
}

// Generate code to put the value of the given identifier
// on top of the stack
// Modifies T9, V0, and SP when executed
code_seq gen_code_ident(ident_t id)
{
    bail_with_error("TODO: no implementation of gen_code_ident yet!");
    return code_seq_empty();
}

// Generate code to put the given number on top of the stack
code_seq gen_code_number(number_t num)
{
    unsigned int global_offset = literal_table_lookup(num.text, num.value);
    return code_seq_concat(code_seq_singleton(code_lw(GP, V0, global_offset)), code_push_reg_on_stack(V0));
}