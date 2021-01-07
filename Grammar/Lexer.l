%{
#include "../expertNode.h"
#include "Parser.yy.hpp"
#include <string>
#include <string>
#include <sstream>
#include <iostream>


%}

%option yylineno
%option nounput
%option noinput

%%

"#".*							{ ; }
[\n]							{ return NL; }

[\t\v\r\f ]+					{ ; }
"=>"							{ return IMPLIES; }
"+"								{ return AND; }
"|"								{ return OR; }
"!"								{ return NOT; }
"^"								{ return XOR; }
"<=>"							{ return EQU; }
"("								{ return BROPEN; }
")"								{ return BRCLOSE; }
"="								{ return ASSIGN; }
"?"                             { return QUERY; }
"show"							{ return SHOW; }
"reset"							{ return RESET; }

[A-Z]							{
									yylval.sIndex = *yytext;
									return VAR;
								}

.					        	{
									YY_FLUSH_BUFFER;
									if (yyin != stdin)
										throw std::runtime_error("Unexpected symbol");
									else
										std::cerr << "Unexpected symbol" << std::endl;
								}

%%

int yywrap()
{
	return 1;
}

