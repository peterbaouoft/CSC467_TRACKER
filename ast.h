
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <list>
#include <string>
#include <assert.h>
#include <iostream>

using namespace std;
/**************************************************************************
 *                              FORWARD DECLARATIONS                      *
 *                                                                        *
 *************************************************************************/
class Scope;
class Declaration;
class Declarations;
class Vistior;
class Type;
class Node;
class Identifier;

/*----------------------------------Statement related--------------------*/
class Statement;
class Statements;
class AssignStatement;
class IfStatement;

/*--------------------------------Expression related---------------------*/
class Expression;
class ConstructorExpression;
class LiteralExpression;
class Constructor;
class UnaryExpression;
class BinaryExpression;
class Constructor;
class Variable;
class Arguments;


typedef Node node;

extern node *ast;

typedef enum {
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),

  EXPRESSION_NODE       = (1 << 2),
  UNARY_EXPRESSION_NODE  = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE= (1 << 2) | (1 << 4),
  INT_NODE              = (1 << 2) | (1 << 5),
  FLOAT_NODE            = (1 << 2) | (1 << 6),
  IDENT_NODE            = (1 << 2) | (1 << 7),
  TYPE_NODE              = (1 << 2) | (1 << 8),
  FUNCTION_NODE         = (1 << 2) | (1 << 9),
  CONSTRUCTOR_NODE      = (1 << 2) | (1 << 10),

  STATEMENT_NODE        = (1 << 1),
  IF_STATEMENT_NODE     = (1 << 1) | (1 << 11),
  WHILE_STATEMENT_NODE  = (1 << 1) | (1 << 12),
  ASSIGNMENT_NODE       = (1 << 1) | (1 << 13),
  NESTED_SCOPE_NODE     = (1 << 1) | (1 << 14),

  DECLARATIONS_NODE     = (1 << 15),
  DECLARATION_NODE      = (1 << 16),
  STATEMENTS_NODE       = (1 << 17),
  VECTOR_NODE           = (1 << 18),
  ARGUMENTS_NODE        = (1 << 19),
  VARIABLE_NODE         = (1 << 20)
} NodeKind;

typedef enum {
    BOOL_EXPRESSION,
    INT_LITERAL,
    FLOAT_LITERAL,
    CONSTRUCTOR_EXPRESSION,
    VARIABLE,
    FUNCTION,
    EXPRESSION,
} ExpressionType;

typedef enum {
    ASSIGN_STATEMENT,
    IF_STATEMENT,
    NESTED_SCOPE,
    STATEMENT,
} StatementType;

class Visitor {
public:
    void visit(Identifier *idet);
    void visit(Scope *scope);
    void visit(Declaration *decl);
    void visit(Type *type);

    void visit(Statement *stmt);
    void visit(Statements *stmts);
    void visit(AssignStatement *as_stmt);
    void visit(IfStatement *if_statement);

    void visit(ConstructorExpression *ce);
    void visit(LiteralExpression *le);
    void visit(UnaryExpression *ue);
    void visit(BinaryExpression *be);

    void visit(Constructor *c);
    void visit(Arguments *args);
    void visit(Variable *var);
};

class Node {
    public:
        virtual void visit(Visitor &vistor) = 0;
};

node *ast_allocate(NodeKind type, ...);

void ast_print(node *ast_root);
#endif /* AST_H_ */
