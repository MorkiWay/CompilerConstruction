%{
	void yyerror (char *s);
	int yylex();
	#include <stdio.h> 
%}


%start program
%type expr
%type exprs
%type term
%type termp
%type fla
%type type
%token CONSTINT
%token COMPOP
%token ENTRY
%token ARITHOP
%token FUNDEFKEY
%token PREDEFBOOL
%token PREDEFINT
%token CONDOP
%token CONSTBOOL
%token UNBOOLOP
%token BIBOOLOP
%token VARDEC
%token VARTYPE
%token PREDEF
%token LPAREN
%token RPAREN
%token VARNAME

%%

program: LPAREN ENTRY expr RPAREN | LPAREN FUNDEFKEY VARNAME VARTYPES type expr RPAREN program


VARTYPES: | LPAREN VARNAME type RPAREN VARTYPES

type: VARTYPE

expr: term | fla
exprs: | expr exprs

term: CONSTBOOL | CONSTINT | VARNAME | LPAREN PREDEF RPAREN | LPAREN ARITHOP term termp RPAREN |
	 LPAREN CONDOP fla term term RPAREN | LPAREN VARNAME exprs RPAREN | 
	 LPAREN VARDEC LPAREN VARNAME expr RPAREN term RPAREN 
termp: term | term termp

fla: CONSTBOOL | VARNAME | LPAREN PREDEFBOOL RPAREN | LPAREN COMPOP term term RPAREN |
	LPAREN UNBOOLOP fla RPAREN | LPAREN BIBOOLOP fla flap RPAREN | LPAREN CONDOP fla fla fla RPAREN |
	LPAREN VARNAME exprs RPAREN | LPAREN VARDEC LPAREN VARNAME expr RPAREN fla RPAREN
flap: fla | fla flap


%%

int main()
{
	return yyparse();
}

void yyerror(char *s) {printf("Error: %s\n",s);}

