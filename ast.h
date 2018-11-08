
#ifndef AST_H_
#define AST_H_ 1
#include <stdlib.h>

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
class EmptyStatement;

/*--------------------------------Expression related---------------------*/
class Expression;
class ConstructorExpression;
class FloatLiteralExpression;
class IntLiteralExpression;
class BoolLiteralExpression;
class Constructor;
class UnaryExpression;
class BinaryExpression;
class VariableExpression;
class FunctionExpression;

class Function;
class IdentifierNode;
class VectorVariable;
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
    EMPTY_STATEMENT_NODE = (1 << 2) | (1 << 7),

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
} ExpressionType;

class Visitor
{
  public:
    virtual void visit(Scope *scope);
    virtual void visit(Declaration *decl);
    virtual void visit (Declarations *decls);
    virtual void visit(Type *type);

    virtual void visit(Statement *stmt);
    virtual void visit(Statements *stmts);
    virtual void visit(AssignStatement *as_stmt);
    virtual void visit(IfStatement *if_statement);
    virtual void visit(NestedScope *ns);
    virtual void visit(EmptyStatement *es);

    virtual void visit(ConstructorExpression *ce);
    virtual void visit(FloatLiteralExpression *fle);
    virtual void visit(BoolLiteralExpression *ble);
    virtual void visit(IntLiteralExpression *ile);
    virtual void visit(UnaryExpression *ue);
    virtual void visit(BinaryExpression *be);
    virtual void visit(VariableExpression *ve);
    virtual void visit(FunctionExpression *fe);

    virtual void visit(Function *f);
    virtual void visit(Constructor *c);
    virtual void visit(Arguments *args);
    virtual void visit(IdentifierNode *var);
    virtual void visit(VectorVariable *vec_var);
};

class PrintVisitor : Visitor {



};

class Node
{
  public:
    virtual void visit(Visitor &vistor) = 0;
};

node *ast_allocate(NodeKind type, ...);

void ast_print(node *ast_root);
#endif /* AST_H_ */
