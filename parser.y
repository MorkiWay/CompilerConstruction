%{
	void yyerror (char *s);
	int yylex();
	#include <stdio.h>
	#include "ast.h"
	#include <string.h>
	#include <stdlib.h>
%}

%union {char* str; int num;}
%start program
%type<num> expr
%type<num> exprs
%type<num> type
%type<num> main
%type<num> name
%type<num> VARTYPES
%token<str> CONSTINT
%token<str> COMPOP
%token<str> ENTRY
%token<str> SIMPARITHOP
%token<str> COMPARITHOP
%token FUNDEFKEY
%token PREDEFBOOL
%token PREDEFINT
%token<str> CONDOP
%token<str> CONSTBOOL
%token<str> UNBOOLOP
%token<str> BIBOOLOP
%token<str> VARDEC
%token<str> VARTYPE
%token PREDEF
%token LPAREN
%token RPAREN
%token<str> VARNAME

%%

program: LPAREN main expr RPAREN 
	{
		insert_child($2); 
		insert_child($3);
		insert_node("ENTRY",2);
	}| 
	LPAREN FUNDEFKEY name VARTYPES type expr RPAREN program
	{
		insert_child($3);
		if ($4) insert_child($4);
		insert_child($5);
		insert_child($6);
		insert_node("DEFINE-FUN",0);
	}

main: ENTRY 
	{
		$$=insert_node("main",0);
	}

VARTYPES: | 
	LPAREN VARNAME VARTYPE RPAREN VARTYPES
	{
		int temp=strlen($2)+strlen($3);
		char * temp2 = (char*)malloc((temp+2)*sizeof(char));

		strncpy(temp2, $2,temp);
		strcat(temp2, " ");
		strcat(temp2, $3);

		$$=insert_node(temp2,0);

		printf(temp2);

		//I do not care about a memory leak.
		// This is the price I pay for the program to work
		//free(temp2);
	}

type: VARTYPE
	{
		char * temp = (char*)malloc((strlen($1)+5)*sizeof(char));

		strcpy(temp,"ret ");
		strcat(temp,$1);
		$$=insert_node(temp,0);
	}

name: VARNAME
	{
		$$=insert_node($1,0);
	}

expr: CONSTINT 
	{
		//printf("LOOKING AT ");
		//printf($1);
		//printf("\n"); 
		$$ = insert_node($1,0);
	}|
	LPAREN PREDEFINT RPAREN 
	{
		$$ = insert_node("get-int",0);
	}| 
	LPAREN SIMPARITHOP expr expr exprs RPAREN 
	{
		insert_child($3);
		insert_child($4);
		insert_child($5);
		$$ = insert_node($2,0);
	}|
	LPAREN COMPARITHOP expr expr RPAREN 
	{
		insert_child($3);
		insert_child($4);
		$$ = insert_node($2,0);
	}|
	CONSTBOOL 
	{ 
		$$ = insert_node($1,0);
	}|
	VARNAME
	{
		$$ =insert_node($1,0);
	}|
	LPAREN PREDEFBOOL RPAREN 
	{
		$$ = insert_node("get-bool",0);
	}| 
	LPAREN COMPOP expr expr RPAREN 
	{
		insert_child($3);
		insert_child($4);
		$$=insert_node($2,0);
	}|
	LPAREN UNBOOLOP expr RPAREN 
	{
		insert_child($3);
		$$=insert_node("NOT",0);
	}| 
	LPAREN BIBOOLOP expr expr exprs RPAREN 
	{
		insert_child($3);
		insert_child($4);
		insert_child($5);
		$$=insert_node($2,0);
	}| 
	LPAREN CONDOP expr expr expr RPAREN 
	{
		insert_child($3);
		insert_child($4);
		insert_child($5);
		$$=insert_node($2,0);
	}|
	LPAREN VARNAME exprs RPAREN 
	{
		insert_child($3);
		$$=insert_node($2,0);
	}| 
	LPAREN VARDEC LPAREN name expr RPAREN expr RPAREN
	{
		insert_child($5);
		insert_child($4);
		insert_child($7);
		$$=insert_node($2,0);
	}

exprs: | 
	expr exprs
	{
		insert_child($1);
	}

%%

void yyerror(char *s) {printf("Error: %s\n",s);}
int yywrap(void){return 1;}
