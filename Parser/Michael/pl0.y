 /* $Id: bison_pl0_y_top.y,v 1.1 2023/10/19 18:47:38 leavens Exp $ */
 /* This file should be named pl0.y, it won't work with other file names! */

%code top {
#include <stdio.h>
}

%code requires {

 /* Including "ast.h" must be at the top, to define the AST type */
#include "ast.h"
#include "machine_types.h"
#include "parser_types.h"
#include "lexer.h"

    /* Report an error to the user on stderr */
extern void yyerror(const char *filename, const char *msg);

}    /* end of %code requires */

%verbose
%define parse.lac full
%define parse.error detailed

 /* the following passes file_name to yyerror,
    and declares it as an formal parameter of yyparse. */
%parse-param { char const *file_name }

%token <ident> identsym
%token <number> numbersym
%token <token> plussym    "+"
%token <token> minussym   "-"
%token <token> multsym    "*"
%token <token> divsym     "/"

%token <token> periodsym  "."
%token <token> semisym    ";"
%token <token> eqsym      "="
%token <token> commasym   ","
%token <token> becomessym ":="

%token <token> constsym   "const"
%token <token> varsym     "var"
%token <token> proceduresym "procedure"
%token <token> callsym    "call"
%token <token> beginsym   "begin"
%token <token> endsym     "end"
%token <token> ifsym      "if"
%token <token> thensym    "then"
%token <token> elsesym    "else"
%token <token> whilesym   "while"
%token <token> dosym      "do"
%token <token> readsym    "read"
%token <token> writesym   "write"
%token <token> skipsym    "skip"
%token <token> oddsym     "odd"

%token <token> neqsym     "<>"
%token <token> ltsym      "<"
%token <token> leqsym     "<="
%token <token> gtsym      ">"
%token <token> geqsym     ">="
%token <token> lparensym  "("
%token <token> rparensym  ")"

%type <block> program
%type <block> block
%type <const_decls> constDecls

%type <var_decls> varDecls
%type <var_decl> varDecl
%type <idents> idents
%type <proc_decls> procDecls
%type <empty> empty
%type <const_decl> constDecl
%type <const_def> constDef
%type <const_defs> constDefs

%type <proc_decl> procDecl
%type <stmt> stmt
%type <assign_stmt> assignStmt
%type <call_stmt> callStmt
%type <begin_stmt> beginStmt
%type <if_stmt> ifStmt
%type <while_stmt> whileStmt
%type <read_stmt> readStmt
%type <write_stmt> writeStmt
%type <skip_stmt> skipStmt
%type <stmts> stmts
%type <condition> condition
%type <odd_condition> oddCondition
%type <rel_op_condition> relOpCondition
%type <expr> expr

%type <token> relOp
%type <expr> term
%type <expr> factor
%type <token> posSign

%start program

%code {
 /* extern declarations provided by the lexer */
extern int yylex(void);

 /* The AST for the program, set by the semantic action 
    for the nonterminal program. */
block_t progast; 

 /* Set the program's ast to be t */
extern void setProgAST(block_t t);
}

%%
 /* Write your grammar rules below and before the next %% */

program: block periodsym
{
    setProgAST($1);
}

block: constDecls varDecls procDecls stmt 
{
    $$ = ast_block($1, $2, $3, $4);
}

constDecls: constDecls constDecl
{
    $$ = ast_const_decls($1, $2);
}
| empty
{
    $$ = ast_const_decls_empty($1);
}

varDecls: varDecls varDecl 
{
    $$ = ast_var_decls($1, $2);
}
| empty
{
    $$ = ast_var_decls_empty($1);
}

varDecl: idents 
{
    $$ = ast_var_decl($1);
}

idents: identsym 
{
    $$ = ast_idents_singleton(ast_ident($1.file_loc));
} 
| idents identsym 
{
    $$ = ast_idents($1, ast_ident($2.file_loc, $2.text));
}

procDecls: procDecls procDecl 
{
    $$ = ast_proc_decls($1, $2);
}
| empty 
{
    $$ = ast_proc_decls_empty($1);
}

empty: 
{
    $$ = ast_empty(NULL);
}


constDecl: constDefs 
{
    $$ = ast_const_decls($1);
}

constDef: idents eqsym numbersym 
{
    $$ = ast_const_def($1, $3);
}

constDefs: constDef 
{
    $$ = ast_const_defs_singleton($1);
} 
| constDefs constDef 
{
    $$ = ast_const_defs($1, $2);
}

procDecl: proceduresym idents semisym block 
{
    $$ = ast_proc_decl($2, $4);
}

stmt: assignStmt
{
    $$ = $1;
}
| callStmt
{
    $$ = $1;
}
| beginStmt
{
    $$ = $1;
}
| ifStmt
{
    $$ = $1;
}
| whileStmt
{
    $$ = $1;
}
| readStmt
{
    $$ = $1;
}
| writeStmt
{
    $$ = $1;
}
| skipStmt
{
    $$ = $1;
}

assignStmt: idents becomessym expr 
{
    $$ = ast_assign_stmt(ast_ident($1.file_loc, $1.idents->name), $3);
}

callStmt: callsym idents 
{
    $$ = ast_call_stmt(ast_ident($2.file_loc, $2.idents->name));
}

beginStmt: beginsym stmts endsym 
{
    $$ = ast_begin_stmt($2);
}

ifStmt: ifsym condition thensym stmt elsesym stmt 
{
    $$ = ast_if_stmt($2, $4, $6);
}

whileStmt: whilesym condition dosym stmt 
{
    $$ = ast_while_stmt($2, $4);
}

readStmt: readsym idents 
{
    $$ = ast_read_stmt(ast_ident($2.file_loc, $2.idents->name));
}

writeStmt: writesym expr 
{
    $$ = ast_write_stmt($2);
}

skipStmt: skipsym 
{
    $$ = ast_skip_stmt($1.file_loc);
}

stmts: stmt 
{
    $$ = ast_stmts_singleton($1);
} 
| stmts semisym stmt 
{
    $$ = ast_stmts($1, $3);
}

condition: oddCondition | relOpCondition


oddCondition: oddsym expr
{
    $$ = ast_odd_condition($2);
}

relOpCondition: expr relOp expr
{
    $$ = ast_rel_op_condition($1, $2, $3);
}

relOp: eqsym | neqsym | ltsym | leqsym | gtsym | geqsym

expr: term | expr plussym term | expr minussym term 


term: factor | term multsym factor | term divsym factor 

factor: idents | posSign numbersym | lparensym expr rparensym 

posSign: plussym | minussym | empty

%%

// Set the program's ast to be ast
void setProgAST(block_t ast) { progast = ast; }
