
#ifndef AST_H_
#define AST_H_ 1
#include <stdlib.h>
#include <string>
#include <vector>
#include <cassert>

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

class Expression : public Node
{
  private:
    std::string type = "ANY_TYPE";
  public:
    virtual std::string get_expression_type() const {return type;}
    virtual void set_expression_type(std::string type_str) {type = type_str;}
};

class Declaration : public Node
{
  private:
    bool is_const = false;
    bool is_read_only = false;
    bool is_write_only = false;
  public:
    Type *type = nullptr;
    std::string id;
    Expression *initial_val = nullptr;

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

    bool get_is_const() const {return is_const;}
    void set_is_const(const bool &const_val) { is_const = const_val;}
    bool get_is_read_only() const {return is_read_only;}
    void set_is_read_only(const bool &read_only_val) { is_read_only = read_only_val;}
    bool get_is_write_only() const {return is_write_only;}
    void set_is_write_only(const bool &write_only_val) { is_write_only = write_only_val;}

};

class IdentifierNode : public Node
{
  private:
    Declaration *declaration = nullptr;

  public:
    virtual Type *get_id_type() const {return declaration ? declaration->type : NULL;}
    void set_declaration(Declaration *decl) {declaration = decl;}
    virtual Declaration *get_declaration() const {return declaration;}
    virtual void set_id_type(Type *type) {}

  public:
    std::string id;

    IdentifierNode(const std::string &identifier) : id(identifier) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class VectorVariable : public IdentifierNode
{
  private:
    Type *type = nullptr;
  public:
    int vector_index;

    VectorVariable(std::string id_node, int v_index) :
        IdentifierNode(id_node), vector_index(v_index) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };

    Type *get_id_type() const{
        return type ?: IdentifierNode::get_id_type();
    }

    void set_id_type(Type *t) {
        type = t;
    }
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
    virtual void push_front_declaration(Declaration *decl) { declaration_list.insert(declaration_list.begin(), decl);}
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
/*=========================Beginning of STATEMENT class===================================*/
class Statement : public Node
{
  public:
    virtual void visit(Visitor &visitor) = 0;
};

class AssignStatement : public Statement
{
  public:
    IdentifierNode *variable = nullptr;
    Expression *expression = nullptr;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class IfStatement : public Statement
{
  public:
    Expression *expression = nullptr;
    Statement *statement = nullptr;
    Statement *else_statement = nullptr;

    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};


/*****************************************Expression Definitions**********************/
class Arguments : public Node
{
  private:
    std::vector<Expression *> m_expression_list;
  public:

    const std::vector<Expression *> &get_expression_list() const { return m_expression_list; }

    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };

    virtual void push_back_expression(Expression *expression) {m_expression_list.push_back(expression);}
};

class Function : public Node
{
  public:
    std::string function_name;
    Arguments *arguments;

    Function(std::string func_name, Arguments *args) : function_name(func_name), arguments(args) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};
class Constructor : public Node
{
  public:
    Type *type;
    Arguments *args;

    Constructor (Type *t, Arguments *arguments) : type(t), args(arguments) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class ConstructorExpression : public Expression
{
  public:
    Constructor *constructor;

    ConstructorExpression(Constructor *ct) : constructor(ct) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class FunctionExpression : public Expression
{
  private:
    std::string resolved_type_name;

  public:
    Function *function;

    std::string &get_resolved_type_name(){return resolved_type_name;} 

    FunctionExpression(Function *func) : function(func) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class VariableExpression : public Expression
{
  public:
    IdentifierNode *id_node;

    VariableExpression(IdentifierNode *id) : id_node(id) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class IntLiteralExpression : public Expression
{
  public:
    int int_literal;

    IntLiteralExpression(int int_val) : int_literal(int_val) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class BoolLiteralExpression : public Expression
{
  public:
    bool bool_literal;

    BoolLiteralExpression(bool bool_val) : bool_literal(bool_val) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class FloatLiteralExpression : public Expression
{
  public:
    float float_literal;

    FloatLiteralExpression(float float_val) : float_literal(float_val) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class UnaryExpression : public Node
{
  private:
    std::string type = "ANY_TYPE"; /* default to any type */
  public:
    int operator_type;
    Expression *right_expression;

  public:
    std::string get_unary_expr_type() const {return type;}
    void  set_unary_expr_type(std::string t) { type = t;}

    UnaryExpression(int op, Expression *rhs_expression) : operator_type(op), right_expression(rhs_expression) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class BinaryExpression : public UnaryExpression
{
  private:
    std::string type = "ANY_TYPE";

  public:
    Expression *left_expression;

  public:
    std::string get_binary_expr_type() const {return type;}
    void  set_binary_expr_type(std::string t) { type = t;}

    BinaryExpression(int op, Expression *rhs_expression, Expression *lhs_expression) :
        UnaryExpression(op, rhs_expression), left_expression(lhs_expression) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

node *ast_allocate(NodeKind type, ...);
void ast_print(node *ast_root);
int semantic_check(node * ast);

#endif /* AST_H_ */
