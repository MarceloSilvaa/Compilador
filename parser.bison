// Tokens
%token
  NEG
  INT  
  EQUAL_TOKEN
  NOT_EQUAL_TOKEN
  LOWER_THAN_TOKEN
  HIGHER_THAN_TOKEN
  LOWEREQ_TOKEN
  HIGHEREQ_TOKEN
  TRUE_TOKEN
  FALSE_TOKEN
  InToken
  BracketOpenToken // {
  BracketCloseToken // }
  VarToken //variavel qualquer
  SemicolonToken // ;
  LetToken // let
  AssignToken // =
  PrintlnToken // println!
  OpenParenthToken // (
  StringToken // string
  CloseParenthToken // )
  WhileToken // while
  ReadlineToken // readline
  IfToken // if
  ElseToken // else
  CommercialToken // &
  CommaToken // ,

// Operator associativity & precedence
%left EQUAL_TOKEN NOT_EQUAL_TOKEN LOWER_THAN_TOKEN HIGHER_THAN_TOKEN LOWEREQ_TOKEN HIGHEREQ_TOKEN
%left PLUS SUB
%left MOD MULT DIV
%precedence NEG
%left BracketOpenToken BracketCloseToken
%left INT TRUE_TOKEN FALSE_TOKEN
%left IfToken WhileToken

// Root-level grammar symbol
%start program;

// Types/values in association to grammar symbols.
%union {
  int intValue;
  char* varName;
  Expr* exprValue;
  BoolExpr* boolValue;
  Cmd* cmdAttri;
  List_Cmd* commandsList;
}

%type <intValue> INT
%type <varName> VarToken
%type <varName> StringToken
%type <exprValue> expr
%type <boolValue> bexpr
%type <cmdAttri> atri
%type <cmdAttri> while
%type <cmdAttri> readline
%type <cmdAttri> if
%type <cmdAttri> cmd
%type <cmdAttri> println
%type <commandsList> list_cmd


// Use "%code requires" to make declarations go
// into both parser.c and parser.h
%code requires {
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "symbol_table.h"

SymTable st;
extern int yylex();
extern int yyline;
extern char* yytext;
extern FILE* yyin;
extern void yyerror(const char* msg);
List_Cmd* root;
}

%%
program: InToken BracketOpenToken list_cmd BracketCloseToken { root=$3; }

list_cmd: cmd list_cmd { $$=ast_cmd_list($1,$2); }
        | /* empty */ { $$=NULL; }

cmd: atri
   | println
   | while
   | readline
   | if

atri: LetToken VarToken AssignToken expr SemicolonToken {st = insert(strdup($2),st); $$ = ast_cmd_var($2,$4); }
println: PrintlnToken OpenParenthToken StringToken CloseParenthToken SemicolonToken { $$ = ast_cmd_printlnSTR($3); }
       | PrintlnToken OpenParenthToken StringToken CommaToken VarToken CloseParenthToken SemicolonToken { $$ = ast_cmd_printlnVAR($3, $5); }
while: WhileToken bexpr BracketOpenToken list_cmd BracketCloseToken { $$ = ast_cmd_while($2, $4); }
readline: ReadlineToken OpenParenthToken CommercialToken VarToken CloseParenthToken SemicolonToken { $$ = ast_cmd_readline($4); }
if: IfToken bexpr BracketOpenToken list_cmd BracketCloseToken { $$ = ast_cmd_if($2,$4); }
  | IfToken bexpr BracketOpenToken list_cmd BracketCloseToken ElseToken BracketOpenToken list_cmd BracketCloseToken { $$ = ast_cmd_if_with_else($2,$4,$8); }

expr:
  VarToken {
    $$ = ast_variable($1);
  }
  |
  INT { 
    $$ = ast_integer($1); 
  }
  | 
  expr PLUS expr { 
    $$ = ast_operation(PLUS, $1, $3);
  }
  | expr SUB expr {
    $$ = ast_operation(SUB, $1, $3);
  }
  | expr MULT expr {
    $$ = ast_operation(MULT, $1, $3);
  }
  | expr DIV expr {
    $$ = ast_operation(DIV, $1, $3);
  }
  | expr MOD expr {
    $$ = ast_operation(MOD, $1, $3);
  }
  | SUB INT %prec NEG {
    $$ = ast_integer(-$2);
  }
  | OpenParenthToken expr CloseParenthToken {
    $$ = $2;
  }
  ;

bexpr:
  TRUE_TOKEN {
    $$ = ast_bool(1);
  }
  | FALSE_TOKEN {
    $$ = ast_bool(0);
  }
  | bexpr EQUAL_TOKEN bexpr {
    $$ = ast_bop_neq(EQUAL_TOKEN, $1, $3);
  }
  | bexpr NOT_EQUAL_TOKEN bexpr {
    $$ = ast_bop_neq(NOT_EQUAL_TOKEN, $1, $3);
  }
  | expr EQUAL_TOKEN expr {
    $$ = ast_bop(EQUAL_TOKEN, $1, $3);
  }
  | expr NOT_EQUAL_TOKEN expr {
    $$ = ast_bop(NOT_EQUAL_TOKEN, $1, $3);
  }
  | expr LOWER_THAN_TOKEN expr {
    $$ = ast_bop(LOWER_THAN_TOKEN, $1, $3);
  }
  | expr HIGHER_THAN_TOKEN expr {
    $$ = ast_bop(HIGHER_THAN_TOKEN, $1, $3);
  }
  | expr LOWEREQ_TOKEN expr {
    $$ = ast_bop(LOWEREQ_TOKEN, $1, $3);
  }
  | expr HIGHEREQ_TOKEN expr {
    $$ = ast_bop(HIGHEREQ_TOKEN, $1, $3);
  }
  | OpenParenthToken bexpr CloseParenthToken {
    $$ = $2;
  }
  ;
%%

void yyerror(const char* err) {
  printf("Line %d: %s - '%s'\n", yyline, err, yytext  );
}

