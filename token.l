/*
 * Scan for tokens in input file.
 *
 * Chirag Kantharia <chirag@kantharia.in>
 * Mar, 2010
 */
%{
#include <stdio.h>
#include <string.h>
#include "grammar.h"
%}

month	([jJ]an|[fF]eb|[mM]ar|[aA]pr|[mM]ay|[jJ]un|[jJ]ul|[aA]ug|[sS]ep|[oO]ct|[nN]ov|[dD]ec)
digit	[0-9]
char	[A-Z]
string	[^:\n]

%%

#[^\n]*		/* ignore comment */
{month}		{ yylval.string = strdup(yytext); return MONTH;   }
{digit}+	{ yylval.number = atoi(yytext);   return AMOUNT;  }
{char}		{ yylval.c = yytext[0]; 	  return CHAR;    }
{string}+	{ yylval.string = strdup(yytext); return COMMENT; }
\n		{ return NEWLINE; }
:		{ return SEP; }

%%

#if 0
int
main (int argc, char **argv)
{
	yylex();
	return 0;
}
#endif
