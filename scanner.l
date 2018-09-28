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

RX_WS    [ \t]
RX_ENTER [\n]

RX_BOOL  (true|false)

RX_SIGN  [+-]
RX_DIGIT [0-9]
RX_INT   {RX_SIGN}?{RX_DIGIT}+
RX_FLOAT {RX_INT}"."{RX_DIGIT}*

RX_CHAR [a-zA-Z]
RX_ID   {RX_CHAR}+[{RX_CHAR}{RX_DIGIT}]*

%%
if   { yTRACE(IF); return IF; }
else { yTRACE(ELSE); return ELSE;}

{RX_WS}    ;
{RX_ENTER} {yyline++;}

{RX_BOOL}  {yylval.as_bool = yytext;
            yTRACE (TK_BOOL);
            return (TK_BOOL);}

{RX_ID}     {yTRACE (TK_ID); return (TK_ID);}

{RX_INT}   {yylval.as_int = atoi(yytext);
            yTRACE (TK_INT);
            return (TK_INT);}

{RX_FLOAT} {yylval.as_float = atof(yytext);
            yTRACE (TK_FLOAT);
            return (TK_FLOAT);}


"/*" { yERROR("Change this - comment unimplemented."); }
.    { yERROR("Unknown token"); }

%%
