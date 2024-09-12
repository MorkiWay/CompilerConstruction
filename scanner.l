%{
#include <stdio.h>
int total_tokens=0; //we love global variables
int line_no=1;

void inc(){
	total_tokens++;
};

%}

whitespace [ \t]+

%%


[0-9]+[a-zA-Z]+[0-9a-zA-Z]*	{; /*this token should not exist, and is removed */}
[0-9]+ 				{printf("%d\tinteger constant:\t %s\n",			line_no, yytext); inc();}
"<="|">="|"<"|">"|"="	{printf("%d\tarithmetic comparison:\t %s\n",		line_no, yytext); inc();}
"print"				{printf("%d\tprogram entry:\t\t %s\n", 			line_no, yytext); inc();}
[\+\-\*]				{printf("%d\tarithmetic operator:\t %s\n",		line_no, yytext); inc();}
"define-fun"			{printf("%d\tfunction definition keyword:\t %s\n",line_no, yytext); inc();}
"get-int"				{printf("%d\tpredefined function:\t %s\n",		line_no, yytext); inc();}
"if"					{printf("%d\tconditional operator:\t %s\n",		line_no, yytext); inc();}
"("					{printf("%d\tleft parentheses:\t %s\n",			line_no, yytext); inc();}
")"					{printf("%d\tright parentheses:\t %s\n",		line_no, yytext); inc();}
[a-zA-Z][a-zA-Z0-9]*	{printf("%d\tvariable/function:\t %s\n",		line_no, yytext); inc();}
{whitespace}			{;}
\n					{line_no++;}

%%

int main()
{
	yylex();

	if (total_tokens==0)
	{
		printf("ERROR: No tokens found\n");
	}

	return 0;
}
