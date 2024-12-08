%{
	void yyerror (char *s);
	int yylex();
	#include <stdio.h>
%}

%start S
%token ID
%token COMMA
%token SEMI
%type nont

%%
S : nont SEMI;
nont : (ID)+ | nont COMMA ID;
%%

int main (void) {return yyparse(); }

void yyerror (char *s) {printf ("%s\n", s); }
