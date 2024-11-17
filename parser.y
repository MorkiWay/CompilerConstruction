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
		insert_node("ENTRY",2,'u');
	}| 
	LPAREN FUNDEFKEY name VARTYPES type expr RPAREN program
	{
		insert_child($3);
		//printf("HERE: %d\n",$3);
		//if ($4) insert_child($4);
		int start = $3;
		int end = $5 - 1;
		while(start<end)
		{
			start++;
			insert_child(start);
		}
		insert_child($5);
		//printf("HERE: %d\n",$5);
		insert_child($6);
		//printf("HERE: %d\n",$6);
		insert_node("DEFINE-FUN",0,'u');
	}

main: ENTRY 
	{
		$$=insert_node("main",0,'u');
	}

VARTYPES: | 
	LPAREN VARNAME VARTYPE RPAREN VARTYPES
	{
		int temp=strlen($2)+strlen($3);
		char * temp2 = (char*)malloc((temp+2)*sizeof(char));

		strncpy(temp2, $2,temp);
		strcat(temp2, " ");
		strcat(temp2, $3);

		$$=insert_node(temp2,0,'u');
		
		
		
		//insert_child(1);
		//insert_child(3);

		//printf(temp2);

		//I do not care about a memory leak.
		// This is the price I pay for the program to work
		//free(temp2);
	}

type: VARTYPE
	{
		char * temp = (char*)malloc((strlen($1)+5)*sizeof(char));

		strcpy(temp,"ret ");
		strcat(temp,$1);
		$$=insert_node(temp,0,$1[0]);
	}

name: VARNAME
	{
		$$=insert_node($1,0,'v');
	}

expr: CONSTINT 
	{
		//printf("LOOKING AT ");
		//printf($1);
		//printf("\n"); 
		$$ = insert_node($1,0,'i');
	}|
	LPAREN PREDEFINT RPAREN 
	{
		$$ = insert_node("get-int",0,'i');
	}| 
	LPAREN SIMPARITHOP expr expr exprs RPAREN 
	{
		insert_child($3);
		insert_child($4);
		//insert_child($5);
		$$ = insert_node($2,0,'i');
	}|
	LPAREN COMPARITHOP expr expr RPAREN 
	{
		insert_child($3);
		insert_child($4);
		$$ = insert_node($2,0,'b');
	}|
	CONSTBOOL 
	{ 
		$$ = insert_node($1,0,'b');
	}|
	VARNAME
	{
		$$ =insert_node($1,0,'v');
	}|
	LPAREN PREDEFBOOL RPAREN 
	{
		$$ = insert_node("get-bool",0,'b');
	}| 
	LPAREN COMPOP expr expr RPAREN 
	{
		insert_child($3);
		insert_child($4);
		$$=insert_node($2,0,'b');
	}|
	LPAREN UNBOOLOP expr RPAREN 
	{
		insert_child($3);
		$$=insert_node("NOT",0,'b');
	}| 
	LPAREN BIBOOLOP expr expr exprs RPAREN 
	{
		insert_child($3);
		insert_child($4);
		//insert_child($5);
		$$=insert_node($2,0,'b');
	}| 
	LPAREN CONDOP expr expr expr RPAREN 
	{
		insert_child($3);
		insert_child($4);
		insert_child($5);
		$$=insert_node($2,0,'c');
	}|
	LPAREN VARNAME exprs RPAREN 
	{
		//insert_child($3);
		$$=insert_node($2,0,'f');
	}| 
	LPAREN VARDEC LPAREN name expr RPAREN expr RPAREN
	{
		//$$=insert_node($2,0);
		insert_child($4);
		insert_child($5);
		insert_child($7);
		$$=insert_node($2,0,'d');
	}

exprs: | 
	expr exprs
	{
		insert_child($1);
	}

%%

void yyerror(char *s) {printf("Error: %s\n",s);}
int yywrap(void){return 1;}
