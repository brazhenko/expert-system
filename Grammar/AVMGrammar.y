%{
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <exception>

extern int	yylex();
extern int	yylineno;
extern char*	yytext;
extern int	yychar;
extern char*	yydebug;

void yyerror(const char *msg)
{
	std::cerr << msg  << " " << yylineno << std::endl;
	printf("yychar: %d\n", yychar);
	throw std::runtime_error("parse error");
}


%}

%token N

%token NL

%left IMPLIES
%left AND
%left OR
%right NOT
%left XOR
%left EQU
%left VAR
%left BROPEN
%left BRCLOSE

%union {
	char *string;
	struct yaccValue *val;
}


%%


S:
	| EXPR S
	;

EXPR:
	EXPR EQU EXPR
	{

	}
	| EXPR IMPLIES EXPR
	{

	}
 	| EXPR XOR EXPR
 	{

 	}
	| EXPR OR EXPR
	{

	}
	| EXPR AND EXPR
	{

	}
	| NOT EXPR
	{

	}
	| BROPEN EXPR BRCLOSE
	{

	}
	| VAR
	{

	}
	;

%%
