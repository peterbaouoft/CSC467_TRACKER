
#ifndef AST_H_
#define AST_H_ 1
#include <stdlib.h>
#include <string>
#include <vector>

/**************************************************************************
 *                              FORWARD DECLARATIONS                      *
 *                                                                        *
 *************************************************************************/
class PrintVisitor;

class Scope;
class Declaration;
class Declarations;
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

class PrintVisitor : public Visitor {
  public:
    virtual void visit(Scope *scope);
    virtual void visit(Declaration *decl);
    virtual void visit(Declarations *decls);
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


class Node
{
  public:
    virtual void visit(Visitor &vistor) = 0;
};

class Declaration : public Node
{
  public:
    Type *type = nullptr;
    std::string id;
    Expression *initial_val = nullptr;
    bool is_const = false;

  public:
    Declaration(Type *type, const std::string &id, Expression *init_val, bool is_const)
    {
        this->type = type;
        this->id = id;
        this->initial_val = init_val;
        this->is_const = is_const;
    }

    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    }
};

class IdentifierNode : public Node
{
  private:
    Declaration *declaration = nullptr;

  public:
    Type *get_id_type() const {return declaration ? declaration->type : NULL;}
    void set_declaration(Declaration *decl) {declaration = decl;}

  public:
    std::string id;

    IdentifierNode(const std::string &identifier) : id(identifier) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class Scope : public Node
{
  public:
    Declarations *declarations = nullptr;
    Statements *statements = nullptr;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    }
};

class Declarations : public Node
{
  public:
    std::vector<Declaration *> declaration_list;

  public:
    virtual void push_back_declaration(Declaration *decl) { declaration_list.push_back(decl); }
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    }
};

class Statements : public Node
{
  private:
    std::vector<Statement *> statement_list;
  public:

    const std::vector<Statement *> &get_statement_list() const {return statement_list;}
    virtual void push_back_statement(Statement *stmt) { statement_list.push_back(stmt); }
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class Type : public Node
{
  public:
    std::string type_name;

    Type(const std::string &type)
    {
        this->type_name = type;
    }
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    }
};

class Expression : public Node
{
  public:
    ExpressionType expression_type;
};


node *ast_allocate(NodeKind type, ...);
void ast_print(node *ast_root);
int semantic_check(node * ast);

#endif /* AST_H_ */
