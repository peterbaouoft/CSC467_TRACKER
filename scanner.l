%{
/**********************************************************************
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"
#include <stdlib.h>

#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

int CheckInt(void);
int yyline = 1;

%}
%option noyywrap

WS      [ \t]
NL	[\n]
BL	[true|false]
DIGIT	[0-9]

%%
[ \t]		;

[\n]		{yyline++;}

[true|false]	{yylval.as_bool = yytext;
		 printf("boolean");
		 return (BOOL);}

[+-]?[0-9]+	{yylval.as_int = atoi(yytext);
		 return (INT);}

[0-9]+"."[0-9]*	{yylval.as_float = atof(yytext);
		 return (FLOAT);}

"/*"            { yERROR("Change this - comment unimplemented."); }

if              { yTRACE(IF); return IF; }
else            { yTRACE(ELSE); return ELSE;}

.               { yERROR("Unknown token"); }

%%
