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

extern FILE*	yyin;
extern int	yylex();
extern int	yylineno;
extern char*	yytext;
extern int	yychar;
extern char*	yydebug;
extern Interpreter interpreter;
int yyparse();

void yyerror(const char *msg)
{
	printf("yychar: %d\n", yychar);

	if (yyin != stdin)
	{
		std::stringstream ss;
		ss << "parse error in line: " << yylineno;
		throw std::runtime_error(ss.str());
	}

	std::cerr << "parse error" << std::endl;
	yyparse();
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

%token ASSIGN
%token QUERY
%token SHOW
%token RESET
%token ASSIGN_FALSE
%token UNKNOWN

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
		interpreter.addExpression($1);
	}

OTHERS:
	OTHER NL
	{
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
	}
	| BROPEN EXPR BRCLOSE
	{
		$$ = $2;
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
	| QUERY QUERY_VARS
	{
		do
		{
			interpreter.evalAllAsTrue();
		}
		while (interpreter.storageChanged());

		interpreter.processAllQueries();
	}
	| SHOW
	{
		do
		{
			interpreter.evalAllAsTrue();
		}
		while (interpreter.storageChanged());

		std::cout << interpreter.trees_to_string();
	}
	| RESET
	{
		interpreter.reset();
	}


ASSIGN_FALSE_VARS:
	| VAR ASSIGN_FALSE_VARS
	{
		interpreter.setVarWithValue(expert_system::Value::False, $1);
	}
	;

ASSIGN_VARS:
	| VAR ASSIGN_VARS
	{
		interpreter.setVarWithValue(expert_system::Value::True, $1);
	}
	;

QUERY_VARS:
	| VAR QUERY_VARS
	{
		interpreter.addQuery($1);
	}
	;


%%
