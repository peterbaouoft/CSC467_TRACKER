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

RX_FUNC_NAMES (lit|dp3|rsq)

%%

{RX_WS}    ;

if   { yTRACE(IF); return IF; }
else { yTRACE(ELSE); return ELSE;}

{RX_ENTER} {yyline++;}
{RX_FUNC_NAMES}     {yTRACE(FUNC_NAME); strncpy(yylval.func_name, yytext, 4) ;
                     printf("The input func name is %s\n", yylval.func_name);
                     return (FUNC_NAME);}

while           {yTRACE(WHILE); return WHILE;}

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

"{"         {yTRACE (LEFT_CURLY); return LEFT_CURLY;}

"}"         {yTRACE (RIGHT_CURLY); return RIGHT_CURLY;}


"("         {yTRACE (LEFT_PARENTHESIS); return LEFT_PARENTHESIS;}

")"         {yTRACE (RIGHT_PARENTHESIS); return RIGHT_PARENTHESIS;}

"["         {yTRACE (LEFT_BRACKET); return LEFT_BRACKET;}

"]"         {yTRACE (RIGHT_BRACKET); return RIGHT_BRACKET;}

"!"         {yTRACE (NOT); return NOT;}

"^"         {yTRACE (CARET); return CARET;}

"*"         {yTRACE (TIMES); return TIMES;}

"/"         {yTRACE (DIVIDE); return DIVIDE;}

"+"         {yTRACE (PLUS); return PLUS;}

"-"         {yTRACE (MINUS); return MINUS;}

"=="        {yTRACE (DOUBLE_EQ); return DOUBLE_EQ;}

"!="        {yTRACE (N_EQ); return N_EQ;}

">"         {yTRACE (GREATER); return GREATER;}

">="        {yTRACE (G_EQ); return G_EQ;}

"<"         {yTRACE (SMALLER); return SMALLER;}

"<="        {yTRACE (S_EQ); return S_EQ;}

"&&"        {yTRACE(AND); return AND;}

"||"        {yTRACE(OR); return OR;}

","         {yTRACE (COMMA); return COMMA;}

";"         {yTRACE (SEMICOLON); return SEMICOLON;}

"/*" { yERROR("Change this - comment unimplemented."); }
.    { yERROR("Unknown token"); }

%%

