
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <list>
#include <string>
#include <assert.h>
#include <iostream>

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

/*----------------------------------Statement related--------------------*/
class Statement;
class Statements;
class AssignStatement;
class IfStatement;
class NestedScope;

/*--------------------------------Expression related---------------------*/
class Expression;
class ConstructorExpression;
class FloatLiteralExpression;
class IntLiteralExpression;
class BoolLiteralExpression;
class Constructor;
class UnaryExpression;
class BinaryExpression;
class IdentifierNode;
class Arguments;

typedef Node node;

extern node *ast;

typedef enum
{
    UNKNOWN = 0,

    SCOPE_NODE = (1 << 0),

    DECLARATIONS_NODE = (1 << 15),
    DECLARATION_NODE = (1 << 16),
    TYPE_NODE = (1 << 2) | (1 << 8),

    STATEMENTS_NODE = (1 << 17),
    IF_STATEMENT_NODE = (1 << 1) | (1 << 11),
    ASSIGNMENT_NODE = (1 << 1) | (1 << 13),
    NESTED_SCOPE_NODE = (1 << 1) | (1 << 14),

    EXPRESSION_NODE = (1 << 2),
    UNARY_EXPRESSION_NODE = (1 << 2) | (1 << 3),
    BINARY_EXPRESSION_NODE = (1 << 2) | (1 << 4),
    VECTOR_NODE = (1 << 18),
    FUNCTION_NODE = (1 << 2) | (1 << 9),
    CONSTRUCTOR_NODE = (1 << 2) | (1 << 10),
    ARGUMENTS_NODE = (1 << 2) | (1 << 5),
    IDENTIFIER_NODE = (1 << 2) | (1 << 6)

} NodeKind;

typedef enum
{
    BOOL_EXPRESSION,
    INT_LITERAL,
    FLOAT_LITERAL,
    CONSTRUCTOR_EXPRESSION,
    VARIABLE,
    FUNCTION,
    EXPRESSION,
} ExpressionType;

class Visitor
{
  public:
    void visit(Scope *scope);
    void visit(Declaration *decl);
    void visit (Declarations *decls);
    void visit(Type *type);

    void visit(Statement *stmt);
    void visit(Statements *stmts);
    void visit(AssignStatement *as_stmt);
    void visit(IfStatement *if_statement);
    void visit(NestedScope *ns);

    void visit(ConstructorExpression *ce);
    void visit(FloatLiteralExpression *fle);
    void visit(BoolLiteralExpression *ble);
    void visit(IntLiteralExpression *ile);
    void visit(UnaryExpression *ue);
    void visit(BinaryExpression *be);

    void visit(Constructor *c);
    void visit(Arguments *args);
    void visit(IdentifierNode *var);
};

class Node
{
  public:
    virtual void visit(Visitor &vistor) = 0;
};

node *ast_allocate(NodeKind type, ...);

void ast_print(node *ast_root);
#endif /* AST_H_ */
