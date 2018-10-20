%{
/***********************************************************************
 *   Interface to the parser module for CSC467 course project.
 *
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>
#include "common.h"
//#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}


// TODO:Modify me to add more data types
// Can access me from flex useing yyval

%union {
	bool as_bool;
	int as_int;
	float as_float;

        int vec_dimension;
        char func_name[3];
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
%token BOOL BOOL_TYPE
%token INT INT_TYPE
%token FLOAT FLOAT_TYPE
%token CONST_TYPE

%token ID

/**********************************************************************
 *                           KEYWORDS
**********************************************************************/
%token IF ELSE

/****************************OPERATORS********************************/
%token LEFT_PARENTHESIS RIGHT_PARENTHESIS LEFT_BRACKET RIGHT_BRACKET
%token NOT
%token CARET /* ^ operator */
%token TIMES DIVIDE
%token PLUS MINUS
%token DOUBLE_EQ N_EQ GREATER SMALLER S_EQ G_EQ EQ
%token AND OR

%token COMMA SEMICOLON
%token FUNC_NAME LEFT_CURLY RIGHT_CURLY WHILE

/* The lower the precedence, the higher the priority is */
%left       OR
%left       AND
%left       PLUS MINUS/* We did something special here, for every minus, we replace it with + (-number) */
%nonassoc   DOUBLE_EQ N_EQ GREATER SMALLER S_EQ G_EQ EQ /* No associativity */
%left       TIMES DIVIDE
%right      CARET
%left       UNARY
%left       FUNCTION_CALL CONSTRUCTOR_CALL VECTOR_SUBSCRIPT

%start    program


%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  : scope                                                                                                           {yTRACE("scope");}
  ;
scope:
    LEFT_CURLY declarations statements RIGHT_CURLY                                                                  {yTRACE("LEFT_CURLY declarations statements RIGHT_CURLY");}
    ;
declarations:
    declarations declaration                                                                                        {yTRACE("declarations declaration");}
    |
    ;
statements:
    statements statement                                                                                            {yTRACE("statements statement");}
    |
    ;
declaration:
    type ID SEMICOLON                                                                                               {yTRACE("type ID SEMICOLON");}
    | type ID EQ expression SEMICOLON                                                                               {yTRACE("type ID EQ expression SEMICOLON");}
    | CONST_TYPE type ID EQ expression SEMICOLON
    ;
statement:
    variable EQ expression SEMICOLON                                                                                {yTRACE("variable EQ expression SEMICOLON");}
    | IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement else_statement
    | WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement
    | scope
    | SEMICOLON
    ;
else_statement:
    ELSE statement
    |
    ;
type:
    INT_TYPE
    | BOOL_TYPE
    | FLOAT_TYPE
    ;
expression:
    constructor
    | function
    | INT
    | FLOAT
    | BOOL
    | variable
    | NOT expression
    | MINUS expression
    | expression AND expression
    | expression OR expression
    | expression DOUBLE_EQ expression
    | expression N_EQ expression
    | expression SMALLER expression
    | expression S_EQ expression
    | expression GREATER expression
    | expression G_EQ expression
    | expression PLUS expression
    | expression MINUS expression
    | expression TIMES expression
    | expression DIVIDE expression
    | expression CARET expression
    | LEFT_PARENTHESIS expression RIGHT_PARENTHESIS
    ;

variable
    :   ID
    |   ID LEFT_BRACKET INT RIGHT_BRACKET
    ;

constructor
    :   type LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS
    ;

function
    :   function_name LEFT_PARENTHESIS arguments_opt RIGHT_PARENTHESIS
    ;

function_name
    :   FUNC_NAME
    ;

arguments_opt
    :   arguments
    ;

arguments
    :   arguments COMMA expression
    |   expression
    ;

%%
/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;

  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}

