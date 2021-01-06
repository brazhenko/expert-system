%{
#include "../expertNode.h"
#include "Parser.yy.hpp"
#include <string>



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
[A-Z]							{
									yylval.sIndex = *yytext;
									return VAR;
								}


%%

int yywrap()
{
	return 1;
}

