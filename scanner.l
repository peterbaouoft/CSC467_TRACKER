%{
/**********************************************************************
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "ast.h"
#include "parser.tab.h"
#include "string.h"
#include <stdlib.h>
#include <ctype.h>


#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }

void process_type();
int process_float();
int process_int();
int process_id();

int CheckInt(void);
int yyline = 1;
int yycolumn = 1;

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yyline; \
        yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng; \
        yycolumn +=yyleng;

%}
%option noyywrap

%x comment

RX_WS    [ \t\r]
RX_ENTER [\n]

RX_BOOL  (true|false)

RX_DIGIT [0-9]
RX_INT   {RX_DIGIT}+
RX_FLOAT [0-9]*\.[0-9]+([eE][-+]?[0-9]+)?
RX_FLOAT_TEMP {RX_INT}?"."{RX_SCIENTIFIC}?[eE]?{RX_DIGIT}*
RX_SCIENTIFIC {RX_DIGIT}[eE][-]?{RX_INT}


RX_CHAR [a-zA-Z_]
RX_ID   [A-Za-z_][A-Za-z0-9_]*

RX_FUNC_NAMES (lit|dp3|rsq)
RX_INT_TYPE (int|ivec2|ivec3|ivec4)
RX_BOOL_TYPE (bool|bvec2|bvec3|bvec4)
RX_FLOAT_TYPE (float|vec2|vec3|vec4)

RX_NUMBER_ERROR [0]{RX_INT}{RX_FLOAT}?

%%

{RX_WS}    ;

if                                  { yTRACE(IF); return IF; }
else                                { yTRACE(ELSE); return ELSE;}

{RX_ENTER}                          {yyline++; yycolumn = 1;}

{RX_FUNC_NAMES}                     {yTRACE(FUNC_NAME); strncpy(yylval.func_name, yytext, 4) ;
                                     return (FUNC_NAME);}

while                               {yTRACE(WHILE); return WHILE;}

{RX_BOOL}                           {yylval.as_bool = (strcmp(yytext, "true") == 0) ? true : false;
                                     yTRACE (BOOL);
                                     return (BOOL);}

{RX_INT_TYPE}                       {yTRACE(INT_TYPE); process_type();
                                     return INT_TYPE;}

{RX_BOOL_TYPE}                      {yTRACE(BOOL_TYPE); process_type();
                                     return BOOL_TYPE;}

{RX_FLOAT_TYPE}                     {yTRACE(FLOAT_TYPE); process_type();
                                     return FLOAT_TYPE;}

{RX_SCIENTIFIC}                     {yTRACE(FLOAT);
                                     process_float();
                                     return (FLOAT);}

{RX_NUMBER_ERROR}                   {yERROR("Invalid number");}

{RX_FLOAT}                          {yTRACE (FLOAT);
                                     process_float();
                                     return (FLOAT);}

{RX_INT}                            {yTRACE (INT);
                                     process_int();
                                     return (INT);}


({RX_DIGIT})E({RX_DIGIT}|{RX_FLOAT})   {//number error: 1E1.5
                                        yERROR("Invalid number");}

{RX_DIGIT}+".-"({RX_DIGIT}|{RX_CHAR})+  {//number error: 12.-3
                                         yERROR("Invalid number");}

{RX_CHAR}+"."["+""-""*""/"]+        {//id error: A.*
                                     yERROR("Invalid ID");}

{RX_DIGIT}+{RX_CHAR}+               {//id error: 0AB
                                     yERROR("Invalid ID");}

const                               {yTRACE (CONST_TYPE); return (CONST_TYPE);}

{RX_ID}                             {yTRACE (ID);
                                     process_id();
                                     return (ID);}

"="                                 {yTRACE (EQ); return EQ;}

"{"                                 {yTRACE (LEFT_CURLY); return LEFT_CURLY;}

"}"                                 {yTRACE (RIGHT_CURLY); return RIGHT_CURLY;}


"("                                 {yTRACE (LEFT_PARENTHESIS); return LEFT_PARENTHESIS;}

")"                                 {yTRACE (RIGHT_PARENTHESIS); return RIGHT_PARENTHESIS;}

"["                                 {yTRACE (LEFT_BRACKET); return LEFT_BRACKET;}

"]"                                 {yTRACE (RIGHT_BRACKET); return RIGHT_BRACKET;}

"!"                                 {yTRACE (NOT); return NOT;}

"^"                                 {yTRACE (CARET); return CARET;}

"*"                                 {yTRACE (TIMES); return TIMES;}

"/"                                 {yTRACE (DIVIDE); return DIVIDE;}

"+"                                 {yTRACE (PLUS); return PLUS;}

"-"                                 {yTRACE (MINUS); return MINUS;}

"=="                                {yTRACE (DOUBLE_EQ); return DOUBLE_EQ;}

"!="                                {yTRACE (N_EQ); return N_EQ;}

">"                                 {yTRACE (GREATER); return GREATER;}

">="                                {yTRACE (G_EQ); return G_EQ;}

"<"                                 {yTRACE (SMALLER); return SMALLER;}

"<="                                {yTRACE (S_EQ); return S_EQ;}

"&&"                                {yTRACE(AND); return AND;}

"||"                                {yTRACE(OR); return OR;}

","                                 {yTRACE (COMMA); return COMMA;}

";"                                 {yTRACE (SEMICOLON); return SEMICOLON;}

"/*"                                {BEGIN(comment);}

<comment>\n                         {yyline++; yycolumn = 1;}
<comment>[^*\n]*                    {;}
<comment>"*"+[^*/\n]*               {;}
<comment>"*"+"/"                    {BEGIN(INITIAL);}
<comment><<EOF>>                    {BEGIN(INITIAL); yERROR("No closing comment(*/) until EOF");}

.                                   { yERROR("Unknown token"); }

%%

int process_id(){
    if (strlen(yytext) > 32){
        yERROR("Size of ID > 32");
    }
    /* Save the id as one of the fields */
    strcpy(yylval.as_id, yytext);

    return 0;
}

void process_type() {
    /* Put dimension into yylval using last character
       of the type*/
    char last_char = yytext[strlen(yytext) - 1];

    if (isdigit(last_char))
        yylval.vec_dimension = last_char - '0'; /* Convert char to a int */
    else
        yylval.vec_dimension = 1;
    // printf("The vec dimension is %d\n", yylval.vec_dimension); /*Comment it out for now, can use debug later */
}

int process_float() {
    char *tmp_ptr;
    float float_num;

    float_num = strtof(yytext, &tmp_ptr);
    if (strlen(tmp_ptr) == strlen(yytext)) {
        yERROR("Float Number error");
    }
    else if (float_num >  MAX_FLOAT) {
        yERROR("Float number is greater than 1E37");
    }
    yylval.as_float = float_num;
    return 0;
}

int process_int() {
    char *tmp_ptr;
    long int_num;

    int_num = strtol(yytext, &tmp_ptr, 10);
    if (strlen(tmp_ptr) == strlen(yytext)) {
        yERROR("Integer Parse error");
    }
    else if (int_num > MAX_INTEGER) {
        yERROR("Integer greater than 32767");
    }
    yylval.as_int = (int) int_num;
    return 0;
}
