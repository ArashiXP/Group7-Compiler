/* $Id: scope_check.c,v 1.16 2023/11/03 16:13:46 leavens Exp leavens $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scope_check.h"
#include "id_attrs.h"
#include "file_location.h"
#include "ast.h"
#include "utilities.h"
#include "symtab.h"
#include "scope_check.h"

// Build the symbol table for prog
// and check for duplicate declarations
// or uses of undeclared identifiers
void scope_check_program(block_t prog)
{
    symtab_enter_scope();
    scope_check_varDecls(prog.var_decls);
    scope_check_constDecls(prog.const_decls);
    scope_check_procDecls(prog.proc_decls);
    scope_check_stmt(prog.stmt);
    symtab_leave_scope();
}

// build the symbol table and check the declarations in vds
void scope_check_varDecls(var_decls_t vds)
{
    var_decl_t *vdp = vds.var_decls;
    while (vdp != NULL)
    {
        scope_check_varDecl(*vdp);
        vdp = vdp->next;
    }
}

// Add declarations for the names in vd,
// reporting duplicate declarations
void scope_check_varDecl(var_decl_t vd)
{
    scope_check_idents(vd.idents, 'v');
}

extern void scope_check_constDefs(const_defs_t cdefs)
{
    const_def_t *cdp = cdefs.const_defs;
    while (cdp != NULL)
    {
        scope_check_constDef(*cdp);
        cdp = cdp->next;
    }
}

// constDef ::= ident = number
extern void scope_check_constDef(const_def_t cdef)
{
    scope_check_declare_ident(cdef.ident, 'c');
}

extern void scope_check_constDecl(const_decl_t cdcl)
{
    scope_check_constDefs(cdcl.const_defs);
}

extern void scope_check_constDecls(const_decls_t cdcls)
{
    const_decl_t *cdp = cdcls.const_decls;
    while (cdp != NULL)
    {
        scope_check_constDecl(*cdp);
        cdp = cdp->next;
    }
}

extern void scope_check_procDecls(proc_decls_t pds)
{
    proc_decl_t *pdp = pds.proc_decls;
    while (pdp != NULL)
    {
        scope_check_procDecl(*pdp);
        pdp = pdp->next;
    }
}
extern void scope_check_procDecl(proc_decl_t pd)
{
    // need to check name instead of ident
    const char *name = pd.name;
    scope_check_procedure(pd, name);
}

void scope_check_procedure(proc_decl_t pd, const char *name)
{
    scope_check_declare_procedure(pd, name);
}
void scope_check_declare_procedure(proc_decl_t pd, const char *name)
{
    if (symtab_declared_in_current_scope(name))
    {
        bail_with_prog_error(*(pd.file_loc), "procedure \"%s\" is already declared", name);
    }
    else
    {
        int ofst_cnt = symtab_scope_loc_count();
        id_attrs *attrs = create_id_attrs(*(pd.file_loc), procedure_idk, ofst_cnt);
        symtab_insert(name, attrs);
    }
}

void scope_check_call_procedure(call_stmt_t cs, const char *name)
{
    // tf
}
// Add declarations for the names in ids
// to current scope as type vt
// reporting any duplicate declarations
void scope_check_idents(idents_t ids, char type)
{
    ident_t *idp = ids.idents;
    while (idp != NULL)
    {
        scope_check_declare_ident(*idp, type);
        idp = idp->next;
    }
}

// Add declaration for id
// to current scope as type vt
// reporting if it's a duplicate declaration
void scope_check_declare_ident(ident_t id, char type)
{
    if (symtab_declared_in_current_scope(id.name))
    {
        // only variables in FLOAT
        if (type == 'v')
        {
            bail_with_prog_error(*(id.file_loc), "variable \"%s\" is already declared as a variable", id.name);
        }

        if (type == 'c')
        {
            bail_with_prog_error(*(id.file_loc), "constant \"%s\" is already declared as a constant ", id.name);
        }
    }
    else
    {
        int ofst_cnt = symtab_scope_loc_count();
        if (type == 'v')
        {
            id_attrs *attrs = create_id_attrs(*(id.file_loc), variable_idk, ofst_cnt);
            symtab_insert(id.name, attrs);
        }
        if (type == 'c')
        {
            id_attrs *attrs = create_id_attrs(*(id.file_loc), constant_idk, ofst_cnt);
            symtab_insert(id.name, attrs);
        }
    }
}

// check the statement to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
void scope_check_stmt(stmt_t stmt)
{
    switch (stmt.stmt_kind)
    {
    case assign_stmt:
        scope_check_assignStmt(stmt.data.assign_stmt);
        break;
    case begin_stmt:
        scope_check_beginStmt(stmt.data.begin_stmt);
        break;
    case if_stmt:
        scope_check_ifStmt(stmt.data.if_stmt);
        break;
    case read_stmt:
        scope_check_readStmt(stmt.data.read_stmt);
        break;
    case write_stmt:
        scope_check_writeStmt(stmt.data.write_stmt);
        break;
    case call_stmt:
        scope_check_callStmt(stmt.data.call_stmt);
    default:
        bail_with_error("Call to scope_check_stmt with an AST that is not a statement!");
        break;
    }
}

// check the statement for
// undeclared identifiers
void scope_check_assignStmt(assign_stmt_t stmt)
{
    const char *name = stmt.name;
    id_use *idu = scope_check_ident_declared(*(stmt.file_loc), name);
    assert(idu != NULL); // since would bail if not declared
    scope_check_expr(*(stmt.expr));
}

// check the statement for
// duplicate declarations and for
// undeclared identifiers
void scope_check_beginStmt(begin_stmt_t stmt)
{
    symtab_enter_scope();
    // scope_check_varDecls(stmt.var_decls);
    scope_check_stmts(stmt.stmts);
    symtab_leave_scope();
}

// check the statements to make sure that
// all idenfifiers referenced in them have been declared
// (if not, then produce an error)
void scope_check_stmts(stmts_t stmts)
{
    stmt_t *sp = stmts.stmts;
    while (sp != NULL)
    {
        scope_check_stmt(*sp);
        sp = sp->next;
    }
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
void scope_check_ifStmt(if_stmt_t stmt)
{
    // scope_check_expr(stmt.expr);
    // scope_check_stmt(*(stmt.body));
    // printf("REMEBER TO RE DO IF STMT SCOPE CHECK");
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
void scope_check_readStmt(read_stmt_t stmt)
{
    scope_check_ident_declared(*(stmt.file_loc), stmt.name);
}

// check the statement to make sure that
// all idenfifiers referenced in it have been declared
// (if not, then produce an error)
void scope_check_writeStmt(write_stmt_t stmt)
{
    scope_check_expr(stmt.expr);
}

void scope_check_callStmt(call_stmt_t stmt)
{
    scope_check_ident_declared(*(stmt.file_loc), stmt.name);
}

// check the expresion to make sure that
// all idenfifiers used have been declared
// (if not, then produce an error)
void scope_check_expr(expr_t exp)
{
    switch (exp.expr_kind)
    {
    case expr_bin:
        scope_check_binary_op_expr(exp.data.binary);
        break;
    case expr_ident:
        scope_check_ident_expr(exp.data.ident);
        break;
    case expr_number:
        // no identifiers are possible in this case, so just return
        break;
    default:
        bail_with_error("Unexpected expr_kind_e (%d) in scope_check_expr", exp.expr_kind);
        break;
    }
}

// check that all identifiers used in exp
// have been declared
// (if not, then produce an error)
void scope_check_binary_op_expr(binary_op_expr_t exp)
{
    scope_check_expr(*(exp.expr1));
    // (note: no identifiers can occur in the operator)
    scope_check_expr(*(exp.expr2));
}

// check the identifier (id) to make sure that
// all it has been declared (if not, then produce an error)
void scope_check_ident_expr(ident_t id)
{
    scope_check_ident_declared(*(id.file_loc), id.name);
}

// check that name has been declared,
// if so, then return an id_use for it
// otherwise, produce an error
id_use *scope_check_ident_declared(file_location floc, const char *name)
{
    id_use *ret = symtab_lookup(name);
    if (ret == NULL)
    {
        bail_with_prog_error(floc, "identifer \"%s\" is not declared!", name);
    }
    return ret;
}
