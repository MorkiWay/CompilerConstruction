%{
#include "y.tab.h"
#include <stdio.h>
int line_no=1;
int tokens=0;
void inc();
int yylex();
%}

whitespace [ \t]+

%%


[0-9]+[a-zA-Z]+[0-9a-zA-Z]* {/*printf("%d\tlexical error, invalid token:\t %s\n", line_no, yytext)*/; exit(1);}
[0-9]+ 				{/*printf("%d\tinteger constant:\t %s\n",			line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return CONSTINT;}
"<="|">="|"<"|">"|"="	{/*printf("%d\tarithmetic comparison:\t %s\n",		line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return COMPOP;}
"print"				{/*printf("%d\tprogram entry:\t\t %s\n", 			line_no, yytext)*/; inc(); return ENTRY;}
[\+\*]				{/*printf("%d\tarithmetic operator:\t %s\n",		line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return SIMPARITHOP;}
"define-fun"			{/*printf("%d\tfunction definition keyword:\t %s\n",line_no, yytext)*/; inc(); return FUNDEFKEY;}
"get-int"				{/*printf("%d\tpredefined function:\t %s\n",		line_no, yytext)*/; inc(); return PREDEFINT;}
"if"					{/*printf("%d\tconditional operator:\t %s\n",		line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return CONDOP;}
"true"|"false"			{/*printf("%d\tboolean constant:\t %s\n",			line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return CONSTBOOL;}
"and"|"or"			{/*printf("%d\tbinary boolean operator:\t %s\n",	line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return BIBOOLOP;}
"not"				{/*printf("%d\tunary boolean operator:\t %s\n",	line_no, yytext)*/; inc(); return UNBOOLOP;}
"let"				{/*printf("%d\tlocal variable declaration:\t %s\n",	line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return VARDEC;}
"bool"|"int"			{/*printf("%d\tvarable/function type:\t %s\n",		line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return VARTYPE;}
"div"|"mod"|[\-]		{/*printf("%d\tarithmetic operations:\t %s\n",		line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return COMPARITHOP;}
"get-bool"			{/*printf("%d\tpredefined function:\t %s\n",		line_no, yytext); inc()*/; return PREDEFBOOL;}
"("					{/*printf("%d\tleft parentheses:\t %s\n",			line_no, yytext)*/; inc(); return LPAREN;}
")"					{/*printf("%d\tright parentheses:\t %s\n",		line_no, yytext)*/; inc(); return RPAREN;}
[a-zA-Z][a-zA-Z0-9]*	{/*printf("%d\tvariable/function:\t %s\n",		line_no, yytext)*/; inc(); yylval.str = strdup(yytext); return VARNAME;}
{whitespace}			{;}
\n					{line_no++;}
.					{/*printf("%d\tlexical error, invalid token:\t %s\n", line_no, yytext)*/; exit(1);}

%%

void inc()
{
	tokens++;

}
