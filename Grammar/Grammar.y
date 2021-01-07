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


void yyerror(const char *msg)
{
	printf("yychar: %d\n", yychar);

	std::stringstream ss;
	ss << "parse error in line: " << yylineno;
	throw std::runtime_error(ss.str());
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
%token RESET
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
			interpreter.repeatUsageCount_[$$->to_string()]++;

			delete $$;
			$$ = t;
		}
		else
		{
			interpreter.repeatDestroyer_[$$->to_string()] = $$;
			interpreter.repeatUsageCount_[$$->to_string()] = 1;
		}

//		std::cout << $$->to_string() << std::endl;
	}
	| EXPR IMPLIES EXPR
	{
		$$ = new expert_system::Implication($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			interpreter.repeatUsageCount_[$$->to_string()]++;

			delete $$;
			$$ = t;
		}
		else
		{
			interpreter.repeatDestroyer_[$$->to_string()] = $$;
			interpreter.repeatUsageCount_[$$->to_string()] = 1;
		}

		// std::cout << $$->to_string() << std::endl;
	}
 	| EXPR XOR EXPR
 	{
		$$ = new expert_system::Xor($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			interpreter.repeatUsageCount_[$$->to_string()]++;

			delete $$;
			$$ = t;
		}
		else
		{
			interpreter.repeatDestroyer_[$$->to_string()] = $$;
			interpreter.repeatUsageCount_[$$->to_string()] = 1;
		}

//		std::cout << $$->to_string() << std::endl;
 	}
	| EXPR OR EXPR
	{
		$$ = new expert_system::Or($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			interpreter.repeatUsageCount_[$$->to_string()]++;

			delete $$;
			$$ = t;
		}
		else
		{
			interpreter.repeatDestroyer_[$$->to_string()] = $$;
			interpreter.repeatUsageCount_[$$->to_string()] = 1;
		}

//		std::cout << $$->to_string() << std::endl;
	}
	| EXPR AND EXPR
	{
		$$ = new expert_system::And($1, $3);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			interpreter.repeatUsageCount_[$$->to_string()]++;

			delete $$;
			$$ = t;
		}
		else
		{
			interpreter.repeatDestroyer_[$$->to_string()] = $$;
			interpreter.repeatUsageCount_[$$->to_string()] = 1;
		}

//		std::cout << $$->to_string() << std::endl;
	}
	| NOT EXPR
	{
		$$ = new expert_system::Not($2);
		if (interpreter.repeatDestroyer_.find($$->to_string()) != interpreter.repeatDestroyer_.end())
		{
			auto t = interpreter.repeatDestroyer_[$$->to_string()];
			interpreter.repeatUsageCount_[$$->to_string()]++;

			delete $$;
			$$ = t;
		}
		else
		{
			interpreter.repeatDestroyer_[$$->to_string()] = $$;
			interpreter.repeatUsageCount_[$$->to_string()] = 1;
		}

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
			interpreter.repeatUsageCount_[$$->to_string()]++;

			delete $$;
			$$ = t;
		}
		else
		{
			interpreter.repeatDestroyer_[$$->to_string()] = $$;
			interpreter.repeatUsageCount_[$$->to_string()] = 1;
		}

//		std::cout << $$->to_string() << std::endl;
	}
	;


OTHER:
	ASSIGN ASSIGN_VARS
	{
		do
		{
			interpreter.evalAllAsTrue();
		}
		while (interpreter.storageChanged());
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
		std::cout << interpreter.trees_to_string();
	}
	| RESET
	{
		interpreter.reset();
	}


ASSIGN_FALSE_VARS:
	| VAR ASSIGN_FALSE_VARS
	{
		std::cout << $1 << " assigned false" << std::endl;

		interpreter.setVarWithValue(expert_system::Value::False, $1);
	}
	;

ASSIGN_VARS:
	| VAR ASSIGN_VARS
	{
		std::cout << $1 << " assigned true" << std::endl;

		interpreter.setVarWithValue(expert_system::Value::True, $1);

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
