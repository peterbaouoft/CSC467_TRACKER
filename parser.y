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
#include "ast.h"
#include "symbol.h"
#include "semantic.h"
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

%locations

// TODO:Modify me to add more data types
// Can access me from flex useing yyval

%union {

    /* Scanner code */
    bool as_bool;
    int as_int;
    float as_float;
    char as_id[MAX_IDENTIFIER - 1];



    int vec_dimension;
    char func_name[3];

    node *as_node; /* this field is only used for AST */
}

/*********************************************************************
 *                          TYPE DECLARATION                         *
*********************************************************************/
%type <as_node> scope
%type <as_node> declarations
%type <as_node> declaration
%type <as_node> type
%type <as_node> statement
%type <as_node> else_statement
%type <as_node> expression
%type <as_node> constructor
%type <as_node> function
%type <as_node> variable
%type <as_node> arguments_opt
%type <as_node> arguments
%type <as_node> statements

%type <as_id> ID
%type <as_bool> CONST_TYPE
%type <func_name> function_name



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
%nonassoc   DOUBLE_EQ N_EQ GREATER SMALLER S_EQ G_EQ EQ /* No associativity */
%left       PLUS MINUS/* We did something special here, for every minus, we replace it with + (-number) */
%left       TIMES DIVIDE
%right      CARET
%left       UNARY
%left       FUNCTION_CALL CONSTRUCTOR_CALL VECTOR_SUBSCRIPT /* define associativity for the brackets */

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
  : scope                                                                                                           { ast = $1;
                                                                                                                     yTRACE("program: -> scope");}
  ;
scope:
    LEFT_CURLY declarations statements RIGHT_CURLY                                                                  { $$ = ast_allocate(SCOPE_NODE, $2, $3);
                                                                                                                     yTRACE("scope: -> LEFT_CURLY declarations statements RIGHT_CURLY");}
    ;
declarations
    : declarations declaration                                                                                      {$$ = ast_allocate(DECLARATIONS_NODE, $1, $2);
                                                                                                                     yTRACE("declarations: -> declarations declaration");}
    |                                                                                                               {$$ = ast_allocate(DECLARATIONS_NODE, nullptr, nullptr);
                                                                                                                      yTRACE("declarations: -> epislon");}
    ;
statements
    : statements statement                                                                                          {$$ = ast_allocate(STATEMENTS_NODE, $1, $2);
                                                                                                                     yTRACE("statements: -> statements statement");}
    |                                                                                                               {$$ = ast_allocate(STATEMENTS_NODE, NULL, NULL);
                                                                                                                     yTRACE("statements: -> epislon");}
    ;
declaration
    : type ID SEMICOLON                                                                                             {$$ = ast_allocate(DECLARATION_NODE, $1, $2, NULL, false, &@$);
                                                                                                                     yTRACE("declaration: -> type ID SEMICOLON");}
    | type ID EQ expression SEMICOLON                                                                               {$$ = ast_allocate(DECLARATION_NODE, $1, $2, $4, false, &@$);
                                                                                                                     yTRACE("declaration: -> type ID EQ expression SEMICOLON");}
    | CONST_TYPE type ID EQ expression SEMICOLON                                                                    {$$ = ast_allocate(DECLARATION_NODE, $2, $3, $5, true, &@$);
                                                                                                                     yTRACE("declaration: -> CONST_TYPE type ID EQ expression SEMICOLON");}
    ;
statement
    : variable EQ expression SEMICOLON                                                                              {$$ = ast_allocate(ASSIGNMENT_NODE, $1, $3, &@$);
                                                                                                                     yTRACE("statement: -> variable EQ expression SEMICOLON");}
    | IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement else_statement                                     {$$ = ast_allocate(IF_STATEMENT_NODE, $3, $5, $6, &@$);
                                                                                                                     yTRACE("statement: -> IF LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement else_statement");}
    | WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement                             %prec FUNCTION_CALL {yTRACE("statement: -> WHILE LEFT_PARENTHESIS expression RIGHT_PARENTHESIS statement");}
    | scope                                                                                                         {$$ = ast_allocate(NESTED_SCOPE_NODE, $1);
                                                                                                                     yTRACE("statement: -> scope");}
    | SEMICOLON                                                                                                     {$$ = ast_allocate(EMPTY_STATEMENT_NODE);
                                                                                                                     yTRACE("statement: -> SEMICOLON");}
    ;
else_statement
    : ELSE statement                                                                                                {$$ = $2;
                                                                                                                     yTRACE("statement: -> ELSE statement");}
    |                                                                                                               {$$ = NULL;
                                                                                                                     yTRACE("statement: -> epislon");}
    ;
type
    : INT_TYPE                                                                                                      {$$ = ast_allocate(TYPE_NODE, 0, yylval.vec_dimension);
                                                                                                                     yTRACE("type: -> INT_TYPE");}
    | BOOL_TYPE                                                                                                     {$$ = ast_allocate(TYPE_NODE, 1, yylval.vec_dimension);
                                                                                                                     yTRACE("type: -> BOOL_TYPE");}
    | FLOAT_TYPE                                                                                                    {$$ = ast_allocate(TYPE_NODE, 2, yylval.vec_dimension);
                                                                                                                     yTRACE("type: -> FLOAT_TYPE");}
    ;
expression
    : constructor                                                                                                   {$$ = ast_allocate(EXPRESSION_NODE, CONSTRUCTOR_EXPRESSION, $1, &@$);
                                                                                                                     yTRACE("expression: -> constructor");}
    | function                                                                                                      {$$ = ast_allocate(EXPRESSION_NODE, FUNCTION, $1, &@$);
                                                                                                                     yTRACE("expression: -> function");}
    | INT                                                                                                           {$$ = ast_allocate(EXPRESSION_NODE, INT_LITERAL, yylval.as_int, &@$);
                                                                                                                     yTRACE("expression: -> INT");}
    | FLOAT                                                                                                         {$$ = ast_allocate(EXPRESSION_NODE, FLOAT_LITERAL, yylval.as_float, &@$);
                                                                                                                     yTRACE("expression: -> FLOAT");}
    | BOOL                                                                                                          {$$ = ast_allocate(EXPRESSION_NODE, BOOL_EXPRESSION, yylval.as_bool, &@$);
                                                                                                                     yTRACE("expression: -> BOOL");}
    | variable                                                                                                      {$$ = ast_allocate(EXPRESSION_NODE, VARIABLE, $1, &@$);
                                                                                                                     yTRACE("expression: -> variable");}
    | NOT expression                                                                            %prec UNARY         {$$ = ast_allocate(UNARY_EXPRESSION_NODE, NOT, $2, &@$);
                                                                                                                     yTRACE("expression: -> NOT expression");}
    | MINUS expression                                                                          %prec UNARY         {$$ = ast_allocate(UNARY_EXPRESSION_NODE, MINUS, $2, &@$);
                                                                                                                     yTRACE("expression: -> NEGATE expression");}
    | expression AND expression                                                                                     {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, AND, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression AND expression");}
    | expression OR expression                                                                                      {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, OR, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression OR expression");}
    | expression DOUBLE_EQ expression                                                                               {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, DOUBLE_EQ, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression DOUBLE_EQ expression");}
    | expression N_EQ expression                                                                                    {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, N_EQ, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression N_EQ expression");}
    | expression SMALLER expression                                                                                 {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, SMALLER, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression SMALLER expression");}
    | expression S_EQ expression                                                                                    {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, S_EQ, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression S_EQ expression");}
    | expression GREATER expression                                                                                 {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, GREATER, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression GREATER expression");}
    | expression G_EQ expression                                                                                    {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, G_EQ, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression G_EQ expression");}
    | expression PLUS expression                                                                                    {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, PLUS, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression PLUS expression");}
    | expression MINUS expression                                                                                   {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, MINUS, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression MINUS expression");}
    | expression TIMES expression                                                                                   {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, TIMES, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression TIMES experssion");}
    | expression DIVIDE expression                                                                                  {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, DIVIDE, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression DIVIDE expression");}
    | expression CARET expression                                                                                   {$$= ast_allocate(BINARY_EXPRESSION_NODE, $1, CARET, $3, &@$);
                                                                                                                     yTRACE("expression: -> expression CARET expression");}
    | LEFT_PARENTHESIS expression RIGHT_PARENTHESIS                                                                 {$$= $2;
                                                                                                                     yTRACE("expression: -> LEFT_PARENTHESIS expression RIGHT_PARENTHESIS");}
    ;

variable
    : ID                                                                                                            {$$ = ast_allocate(IDENTIFIER_NODE, yylval.as_id, &@$); /* Will this one work tho */
                                                                                                                     yTRACE("variable: -> ID");}
    | ID LEFT_BRACKET INT RIGHT_BRACKET                                                     %prec VECTOR_SUBSCRIPT  {$$ = ast_allocate(VECTOR_NODE, $1, yylval.as_int, &@$);
                                                                                                                     yTRACE("variable: -> ID LEFT_BRACKET INT RIGHT_BRACKET");}
    ;

constructor
    : type LEFT_PARENTHESIS arguments RIGHT_PARENTHESIS                                     %prec CONSTRUCTOR_CALL  {$$ = ast_allocate(CONSTRUCTOR_NODE, $1, $3, &@$);
                                                                                                                     yTRACE("constuctor: ->  type LEFT_PARENTHESIS arguments RIGHT_PARENTHEIS");}
    ;

function
    : function_name LEFT_PARENTHESIS arguments_opt RIGHT_PARENTHESIS                        %prec FUNCTION_CALL     {$$ = ast_allocate(FUNCTION_NODE, $1, $3, &@$);
                                                                                                                     yTRACE("function: -> function_name LEFT_PARENTHESIS arguments_opt RIGHT_PARENTHESIS");}
    ;

function_name
    : FUNC_NAME                                                                                                     {strcpy($$, yylval.func_name); /* copy array content into lhs */
                                                                                                                     yTRACE("function_name: -> ('dp3'|'lit'|'rsq')");}
    ;

arguments_opt
    : arguments                                                                                                     {$$ = $1;
                                                                                                                     yTRACE("arguments_opt: -> arguments");}
    |                                                                                                               {$$ = ast_allocate(ARGUMENTS_NODE, NULL, NULL);
                                                                                                                     yTRACE("arguments_opt: -> epislon");}
    ;

arguments
    : arguments COMMA expression                                                                                    {$$ = ast_allocate(ARGUMENTS_NODE, $1, $3, &@$);
                                                                                                                     yTRACE("arguments: -> arguments COMMA expression");}
    | expression                                                                                                    {$$ = ast_allocate(ARGUMENTS_NODE, NULL, $1, &@$);
                                                                                                                     yTRACE("arguments: -> expression");}
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

