#include <iostream>


int yyparse();
extern FILE * yyin;



int main(int ac, char **av)
{
	yyin = stdin;

	yyparse();
}