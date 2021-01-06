%{

#include "../expertNode.h"
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
%nonassoc NOT
%left XOR
%left EQU
%left VAR
%left BROPEN
%left BRCLOSE


%union {
	expertNodes::iExpertNode* Node;
	char sIndex; /* symbol table index */
}

%token <sIndex> VAR
%type <Node> EXPR


%%


S:
	| EXPR S
	;

EXPR:
	EXPR EQU EXPR
	{
		$$ = new expertNodes::Equ($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
	| EXPR IMPLIES EXPR
	{
		$$ = new expertNodes::Implication($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
 	| EXPR XOR EXPR
 	{
		$$ = new expertNodes::Xor($1, $3);
		std::cout << $$->to_string() << std::endl;
 	}
	| EXPR OR EXPR
	{
		$$ = new expertNodes::Or($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
	| EXPR AND EXPR
	{
		$$ = new expertNodes::And($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
	| NOT EXPR
	{
		$$ = new expertNodes::Not($2);
		std::cout << $$->to_string() << std::endl;
	}
	| BROPEN EXPR BRCLOSE
	{
		$$ = $2;
		std::cout << $$->to_string() << std::endl;
	}
	| VAR
	{
		$$ = new expertNodes::Var($1);
		std::cout << $$->to_string() << std::endl;
	}
	;

%%
