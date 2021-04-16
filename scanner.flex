%{
// HEADERS
#include <stdlib.h>
#include "parser.h"

// variables maintained by the lexical analyser
int yyline = 1;
%}

%option noyywrap

%%
[ \t]+ {  }
#.*\n { yyline++; }
\n { yyline++; }

[0-9]+ { 
   yylval.intValue = atoi(yytext);
   return INT; 
}


"fn main()" { return InToken; }

"let" { return LetToken; }
"{" { return BracketOpenToken; }
"}" { return BracketCloseToken; }
"=" { return AssignToken; }
";" { return SemicolonToken; }

"println!" { return PrintlnToken; }
"read_line" { return ReadlineToken; }
"if" { return IfToken; }
"else" { return ElseToken; }
"(" { return OpenParenthToken; }
\".*\" { yylval.varName = strdup(yytext); return StringToken; }
")" { return CloseParenthToken; }
"while" { return WhileToken; }

"," { return CommaToken; }

"+" { return PLUS; }
"*" { return MULT; }
"/" { return DIV; }
"-" { return SUB; }
"%" { return MOD; }

"==" { return EQUAL_TOKEN; }
"!=" { return NOT_EQUAL_TOKEN; }
"<"  { return LOWER_THAN_TOKEN; }
">"  { return HIGHER_THAN_TOKEN; }
"<=" { return LOWEREQ_TOKEN; }
">=" { return HIGHEREQ_TOKEN; }
"true" { return TRUE_TOKEN; }
"false" { return FALSE_TOKEN; }

"&" { return CommercialToken; }

[a-zA-Z_][a-zA-Z0-9_]* { yylval.varName = strdup(yytext); return VarToken; }

.  { yyerror("unexpected character"); }
%%

