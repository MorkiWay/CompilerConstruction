%{
  #include <stdio.h>
  void yyerror (char *s);
  int yylex();
%}

%union {int val; char opr;}
%start program
%token<val> NUMBERS
%token<opr> PLUS
%token<opr> MINUS
%token<opr> OPAREN
%token<opr> CPAREN

%type<val> expr

%%

program: expr {printf("Result: %d\n",$1);}
expr:  expr PLUS expr {$$=$1+$3;}
	| expr MINUS expr {$$=$1-$3;}
	| OPAREN expr CPAREN {$$=$2;}
	| NUMBERS {$$=$1;}
%%

void yyerror (char *s) {fprintf (stderr, "%s\n", s);}

int main()
{
  return yyparse();
}
