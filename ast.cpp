#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"
#include <cassert>

#define DEBUG_PRINT_TREE 0

using namespace std;
/*========================================Initial Declarations===================*/
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

class Declaration : public Node
{
  public:
    Type *type = nullptr;
    string id;
    Expression *initial_val = nullptr;
    bool is_const = false;

  public:
    Declaration(Type *type, string id, Expression *init_val, bool is_const)
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

class Declarations : public Node
{
  public:
    vector<Declaration *> declaration_list;

  public:
    virtual void push_back_declaration(Declaration *decl) { declaration_list.push_back(decl); }
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
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
/*=========================================END OF INITIAL DECLARATIONS======================*/

/*=========================Beginning of STATEMENT class===================================*/
class Statements : public Node
{

  public:
    vector<Statement *> statement_list;

    virtual void push_back_statement(Statement *stmt) { statement_list.push_back(stmt);}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

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
class NestedScope : public Statement
{
  public:
    Statement *statement;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};
/*================END OF STATEMENT CLASS================*/

/*================Start of Expression classes===========*/
class Expression : public Node
{
  public:
    ExpressionType expression_type;
    Expression(ExpressionType et)
    {
        this->expression_type = et;
    }
};

class ConstructorExpression : public Expression
{
  public:
    Constructor *constructor;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class IntLiteralExpression : public Expression
{
  public:
    int int_literal;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class BoolLiteralExpression : public Expression
{
  public:
    bool bool_literal;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class FloatLiteralExpression : public Expression
{
  public:
    float float_literal;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class UnaryExpression : public Node
{
  public:
    string op;
    Expression *right_expression;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class BinaryExpression : public UnaryExpression
{
  public:
    Expression *left_expression;
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
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class Arguments : public Node
{
  public:
    Arguments *args;
    Expression *expression;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class IdentifierNode : public Node
{
  public:
    string id;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class VectorVariable : public IdentifierNode
{
  public:
    int vector_index;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

/*===========================End of expression classes========================*/

node *ast = NULL;

node *ast_allocate(NodeKind type, ...)
{

    va_list args;

    node *ret_node = NULL;
    va_start(args, type);
    switch (type)
    {
    case SCOPE_NODE:
    {
        Scope *scope = new Scope();
        Declarations *d = va_arg(args, Declarations *);
        assert(d);
        scope->declarations = d;

        Statements *statements = va_arg(args, Statements *);
        assert(statements);
        scope->statements = statements;

        ret_node = scope;
        break;
    }

    case DECLARATIONS_NODE:
    {
        Declarations *declarations = va_arg(args, Declarations *);
        if (declarations == nullptr)
        {
            declarations = new Declarations();
        }
        Declaration *declaration = va_arg(args, Declaration *);
        if (declaration != nullptr)
        {
            declarations->push_back_declaration(declaration);
        }

        ret_node = declarations;
        break;
    }

    case DECLARATION_NODE:
    {
        Type *type = va_arg(args, Type *);
        string id = static_cast<string>(va_arg(args, char *));
        Expression *expression = va_arg(args, Expression *);
        bool is_const = static_cast<bool>(va_arg(args, int));

        Declaration *declaration = new Declaration(type, id, expression, is_const);

        ret_node = declaration;
        break;
    }

    case TYPE_NODE:
    {
        int type_index = va_arg(args, int);
        int dimen_index = va_arg(args, int);
        string type_list[3][4] = {{"int", "ivec2", "ivec3", "ivec4"},
                                  {"bool", "bvec2", "bvec3", "bvec4"},
                                  {"float", "vec2", "vec3", "vec4"}};
        if (type_index > 2 || dimen_index > 3)
        {
            assert(true && "Index for type, please check for bugs\n");
        }

        string type = type_list[type_index][dimen_index - 1]; /* dim goes from 1 to 4 */
        cout << "Test instantiations\n\n"
             << type;

        ret_node = new Type(type);
        break;
    }

    /*===========================STATEMENTS============================*/
    case STATEMENTS_NODE:
    {

        Statements *statements = va_arg(args, Statements *);
        if (statements == nullptr)
        {
            statements = new Statements();
        }
        Statement *statement = va_arg(args, Statement *);

        if (statement != nullptr)
        {
            statements->push_back_statement(statement);
        }
        assert(statements);
        ret_node = statements;
        break;
    }

    case IF_STATEMENT_NODE:
    {
        IfStatement *if_statement = new IfStatement();
        if_statement->expression = va_arg(args, Expression *);
        if_statement->statement = va_arg(args, Statement *);
        if_statement->else_statement = va_arg(args, Statement *);

        ret_node = if_statement;
        break;
    }

    case ASSIGNMENT_NODE:
    {
        AssignStatement *assign_statement = new AssignStatement();
        assign_statement->variable = va_arg(args, IdentifierNode *);
        assign_statement->expression = va_arg(args, Expression *);

        ret_node = assign_statement;
        break;
    }

    case NESTED_SCOPE_NODE:
    {
        NestedScope *ns = new NestedScope();
        break;
    }
        default:
            ;
    }

    va_end(args);
    return ret_node;
}

void ast_print(node *root)
{
    Visitor visitor;
    root->visit(visitor);
}

void Visitor::visit(Scope *scope)
{
    printf("(SCOPE \n");
    scope->declarations->visit(*this);
    assert(scope->statements);
    scope->statements->visit(*this);
    printf(")\n");
}

void Visitor::visit (Declarations *decl){
    printf("(DECLARATIONS ");
    for (Declaration *declaration : decl->declaration_list)
        {
            assert(declaration != nullptr);
            declaration->visit(*this);
        }
    printf(") ");
}
void Visitor::visit(Declaration *decl)
{
    printf("(DECLARATION ");
    assert(decl->type);
    printf(" %s ", decl->id.c_str());
    decl->type->visit(*this);
    if (decl->initial_val != nullptr)
        decl->initial_val->visit(*this);
    printf(") ");
}

void Visitor::visit(Type *type)
{
    cout << type->type_name;
}

void Visitor::visit(Statements *stmts)
{
    printf("(STATEMENTS");
    for (Statement *stmt : stmts->statement_list)
    {
        assert(stmt != nullptr);
        // stmt->visit(*this);
    }
    printf(")");
}

void Visitor::visit(AssignStatement *)
{
    ;
}

void Visitor::visit(IfStatement *)
{
    ;
}

void Visitor::visit(NestedScope *)
{
    ;
}

