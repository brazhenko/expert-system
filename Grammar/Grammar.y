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

%left EQU
%left IMPLIES
%left XOR

%left OR
%left AND
%nonassoc NOT
%left BROPEN
%left BRCLOSE
%left VAR


%union {
	expert_system::iExpertNode* Node;
	char sIndex; /* symbol table index */
}

%token <sIndex> VAR
%type <Node> EXPR


%%


S:
	| NL S
	| FULLEXPR S
	;


FULLEXPR:
	EXPR NL
	{
		std::cout << "Expression done!" << std::endl;
		std::cout << (int)expert_system::Value{} << std::endl;
	}

EXPR:
	EXPR EQU EXPR
	{
		$$ = new expert_system::Equ($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
	| EXPR IMPLIES EXPR
	{
		$$ = new expert_system::Implication($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
 	| EXPR XOR EXPR
 	{
		$$ = new expert_system::Xor($1, $3);
		std::cout << $$->to_string() << std::endl;
 	}
	| EXPR OR EXPR
	{
		$$ = new expert_system::Or($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
	| EXPR AND EXPR
	{
		$$ = new expert_system::And($1, $3);
		std::cout << $$->to_string() << std::endl;
	}
	| NOT EXPR
	{
		$$ = new expert_system::Not($2);
		std::cout << $$->to_string() << std::endl;
	}
	| BROPEN EXPR BRCLOSE
	{
		$$ = $2;
		std::cout << $$->to_string() << std::endl;
	}
	| VAR
	{
		$$ = new expert_system::Var($1);
		std::cout << $$->to_string() << std::endl;
	}
	;

%%
