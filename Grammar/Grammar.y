%{

#include "../expertNode.h"
#include "../Interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <iostream>
#include <exception>
#include <string>
#include <sstream>
#include <iostream>

extern int	yylex();
extern int	yylineno;
extern char*	yytext;
extern int	yychar;
extern char*	yydebug;
extern Interpreter interpreter;
bool assigned = false;


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
//%token VAR

%token ASSIGN
%token QUERY
%token SHOW
%token ASSIGN_FALSE

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
	| OTHERS S
	;


FULLEXPR:
	EXPR NL
	{
//		std::cout << "Expression done!" << std::endl;
		interpreter.addExpression($1);
	}

OTHERS:
	OTHER NL
	{
//		std::cout << "Other" << std::endl;
	}

EXPR:
	EXPR EQU EXPR
	{
		$$ = new expert_system::Equ($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			delete $$;
			$$ = t;
		}
		interpreter.repeatDestroyer_[$$->to_string()] = $$;

//		std::cout << $$->to_string() << std::endl;
	}
	| EXPR IMPLIES EXPR
	{
		$$ = new expert_system::Implication($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			delete $$;
			$$ = t;
		}
		interpreter.repeatDestroyer_[$$->to_string()] = $$;

		// std::cout << $$->to_string() << std::endl;
	}
 	| EXPR XOR EXPR
 	{
		$$ = new expert_system::Xor($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			delete $$;
			$$ = t;
		}
		interpreter.repeatDestroyer_[$$->to_string()] = $$;

//		std::cout << $$->to_string() << std::endl;
 	}
	| EXPR OR EXPR
	{
		$$ = new expert_system::Or($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			delete $$;
			$$ = t;
		}
		interpreter.repeatDestroyer_[$$->to_string()] = $$;

//		std::cout << $$->to_string() << std::endl;
	}
	| EXPR AND EXPR
	{
		$$ = new expert_system::And($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			delete $$;
			$$ = t;
		}
		interpreter.repeatDestroyer_[$$->to_string()] = $$;

//		std::cout << $$->to_string() << std::endl;
	}
	| NOT EXPR
	{
		$$ = new expert_system::Not($2);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			delete $$;
			$$ = t;
		}
		interpreter.repeatDestroyer_[$$->to_string()] = $$;

//		std::cout << $$->to_string() << std::endl;
	}
	| BROPEN EXPR BRCLOSE
	{
		$$ = $2;
//		std::cout << $$->to_string() << std::endl;
	}
	| VAR
	{
		$$ = new expert_system::Var($1);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			delete $$;
			$$ = t;
		}
		interpreter.repeatDestroyer_[$$->to_string()] = $$;

//		std::cout << $$->to_string() << std::endl;
	}
	;


OTHER:
	ASSIGN ASSIGN_VARS
	{
		interpreter.evalAllAsTrue();
	}
	| ASSIGN_FALSE ASSIGN_FALSE_VARS
	{
		interpreter.evalAllAsTrue();
	}
	| QUERY QUERY_VARS
	{
		interpreter.processAllQueries();
	}
	| SHOW
	{
		std::cout << "Showing debug info" << std::endl;
		std::cout << interpreter.trees_to_string() << std::endl;
	}


ASSIGN_FALSE_VARS:
	| VAR ASSIGN_FALSE_VARS
	{
		std::cout << $1 << " assigned false" << std::endl;

		try {
			interpreter.setVarWithValue(expert_system::Value::False, $1);
			interpreter.commitChanges();
		} catch (const std::exception &e) {
			std::cerr << e.what() << std::endl;
			interpreter.discardChanges();
		}
	}
	;

ASSIGN_VARS:
	| VAR ASSIGN_VARS
	{
		std::cout << $1 << " assigned true" << std::endl;

		interpreter.setVarWithValue(expert_system::Value::True, $1);
		interpreter.commitChanges();

	}
	;

QUERY_VARS:
	| VAR QUERY_VARS
	{
		std::cout << "QUERY VAR" << std::endl;
		std::cout << $1 << " what?" << std::endl;
		interpreter.addQuery($1);
	}
	;


%%
