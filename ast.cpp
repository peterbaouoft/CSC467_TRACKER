#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <iostream>
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
    string type_name;

    Type(const string &type)
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
  private:
    vector<Statement *> statement_list;
  public:

    const vector<Statement *> &get_statement_list() const {return statement_list;}
    virtual void push_back_statement(Statement *stmt) { statement_list.push_back(stmt); }
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
    Scope *scope;

    NestedScope(Scope *s) : scope(s) {}
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
  public:
    Function *function;

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
    Type *type;
  public:
    int operator_type;
    Expression *right_expression;

  public:
    Type *get_unary_expr_type() const {return type;}
    void  set_unary_expr_type(Type *t) { assert(t); type = t;}

    UnaryExpression(int op, Expression *rhs_expression) : operator_type(op), right_expression(rhs_expression) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
};

class BinaryExpression : public UnaryExpression
{
  private:
    Type *type;

  public:
    Expression *left_expression;

  public:
    Type *get_binary_expr_type() const {return type;}
    void  set_binary_expr_type(Type *t) { assert(t); type = t;}

    BinaryExpression(int op, Expression *rhs_expression, Expression *lhs_expression) :
        UnaryExpression(op, rhs_expression), left_expression(lhs_expression) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };


};

class Function : public Node
{
  public:
    string function_name;
    Arguments *arguments;

    Function(string func_name, Arguments *args) : function_name(func_name), arguments(args) {}
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

class Arguments : public Node
{
  private:
    vector<Expression *> m_expression_list;
  public:

    const vector<Expression *> &get_expression_list() const { return m_expression_list; }

    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };

    virtual void push_back_expression(Expression *expression) {m_expression_list.push_back(expression);}
};

class IdentifierNode : public Node
{
  private:
    Type *type;

  public:
    Type *get_id_type() const {return type;}
    void set_id_type(Type *t) {assert(t); type = t;}

  public:
    string id;

    IdentifierNode(string identifier) : id(identifier) {}
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
        string id(va_arg(args, char *));
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

        /* Here, we get the corresponding type from the given indices
         * and create a Type node based off that */
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
        Scope *scope = va_arg(args, Scope*);
        ret_node = new NestedScope(scope);
        break;
    }

    /*===========================EXPRESSION============================*/
    case EXPRESSION_NODE:
    {
        /* Here, we get the specific type for a single expression and
         * instantiate its subtype accordingly */
        ExpressionType et = static_cast<ExpressionType>(va_arg(args, int));
        switch (et)
        {

        case CONSTRUCTOR_EXPRESSION:
        {
            Constructor *ct = va_arg(args, Constructor *);
            ret_node = new ConstructorExpression(ct);
            break;
        }
        case FUNCTION:
        {
            Function *func = va_arg(args, Function *);
            ret_node = new FunctionExpression(func);
            break;
        }
        case VARIABLE:
        {
            IdentifierNode *id_node = va_arg(args, IdentifierNode*);
            ret_node = new VariableExpression(id_node);
            break;
        }

        case BOOL_EXPRESSION:
        {
            bool bool_literal = static_cast<bool>(va_arg(args, int));
            ret_node = new BoolLiteralExpression(bool_literal);
            break;
        }
        case INT_LITERAL:
        {
            int int_literal = va_arg(args, int);
            ret_node = new IntLiteralExpression(int_literal);
            break;
        }
        case FLOAT_LITERAL:
        {
            float float_literal = static_cast<float>(va_arg(args, double));
            ret_node = new FloatLiteralExpression(float_literal);
            break;
        }
        }
        break;
    }

    case UNARY_EXPRESSION_NODE:
    {
        int operator_type = va_arg(args, int);
        Expression *expression = va_arg(args, Expression*);

        UnaryExpression* unary_expr = new UnaryExpression(operator_type, expression);
        /* Below is done before the semantic analysis, the type value will be filled
         * in, once semantic analysis is complete */
        unary_expr->set_unary_expr_type(new Type("ANY_TYPE"));
        ret_node = unary_expr;
        break;
    }

    case BINARY_EXPRESSION_NODE:
    {
        Expression *lhs_expr = va_arg(args, Expression *);
        int operator_type  = va_arg(args, int);
        Expression *rhs_expr = va_arg(args, Expression *);

        BinaryExpression *bin_expr = new BinaryExpression(operator_type, rhs_expr, lhs_expr);
        /* Below is done before the semantic analysis, the type value will be filled
         * in, once semantic analysis is complete */
        bin_expr->set_binary_expr_type(new Type("ANY_TYPE"));
        ret_node = bin_expr;
        break;
    }


    case VECTOR_NODE:
    {
        /* Need to think about types from identifier node */
        break;
    }

    case FUNCTION_NODE:
    {
        /* Need to think about arguments null or non-null case */
        char *func_name = va_arg(args, char*);
        string function_name(func_name); /* The func name can not be null, thus directly cast */
        Arguments *arguments = va_arg(args, Arguments*);
        ret_node = new Function(function_name, arguments);
        break;
    }

    case CONSTRUCTOR_NODE:
    {
        Type *type = va_arg(args, Type*);
        Arguments *arguments = va_arg(args, Arguments*);

        ret_node = new Constructor(type, arguments);
        break;
    }

    case ARGUMENTS_NODE:
    {
        Arguments *arguments = va_arg(args, Arguments*);
        if (arguments == nullptr){
            arguments = new Arguments();
        }
        Expression *expression = va_arg(args, Expression*); /* Expression can not be null */
        arguments->push_back_expression(expression);

        ret_node = arguments;
        break;
    }

    case IDENTIFIER_NODE:
    {
        Type *type = new Type("ANY_TYPE");
        string id = static_cast<string>(va_arg(args, char *));
        IdentifierNode *id_node = new IdentifierNode(id);
        id_node->set_id_type(type);

        ret_node = id_node;
        break;
    }

    default:;
    }

    va_end(args);
    return ret_node;
}

void ast_print(node *root)
{
    Visitor visitor;
    root->visit(visitor);
}
/*===============================================VISITORS=====================================*/
void Visitor::visit(Scope *scope)
{
    printf("(SCOPE \n");
    scope->declarations->visit(*this);
    assert(scope->statements);
    scope->statements->visit(*this);
    printf(")\n");
}

void Visitor::visit(Declarations *decl)
{
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
    for (Statement *stmt : stmts->get_statement_list())
    {
        assert(stmt != nullptr);
        stmt->visit(*this);
    }
    printf(")");
}

void Visitor::visit(AssignStatement *assign_stmt)
{
    printf("(ASSIGN");
    assign_stmt->variable->visit(*this);
    assign_stmt->expression->visit(*this);
    printf(")");
}

void Visitor::visit(IfStatement *if_statement)
{
    printf("(IF ");
    if_statement->expression->visit(*this);
    if_statement->statement->visit(*this);
    if (if_statement->else_statement){
        printf(" ");
        if_statement->else_statement->visit(*this);
    }
    printf(")");
}

void Visitor::visit(NestedScope *ns)
{
    ns->scope->visit(*this);
}

void Visitor::visit(FunctionExpression *fe)
{
    fe->function->visit(*this);
}
void Visitor::visit(VariableExpression *ve)
{
    ve->id_node->visit(*this);
}

void Visitor::visit(ConstructorExpression *ce)
{
    ce->constructor->visit(*this);
}
void Visitor::visit(IntLiteralExpression *ile)
{
    printf("%d", ile->int_literal);
}

void Visitor::visit(BoolLiteralExpression *ble)
{
    string a = ble->bool_literal? "true" : "false";
    printf("%s", a.c_str());
}

void Visitor::visit(FloatLiteralExpression *fle)
{
    printf("%f", fle->float_literal);
}

void Visitor::visit(UnaryExpression *ue)
{
    printf("(UNARY");
    ue->get_unary_expr_type()->visit(*this);
    printf(""); /*Fill in operator information */
    ue->right_expression->visit(*this);
    printf(")\n");
}

void Visitor::visit(BinaryExpression *be)
{
    printf("(BINARY");
    Type *t = be->get_binary_expr_type();
    t->visit(*this);
    printf(""); /*Fill in operator information */
    be->left_expression->visit(*this);
    be->right_expression->visit(*this);
    printf(")");
}


void Visitor::visit(Function *func)
{
    ;
}
void Visitor::visit(Constructor *ct)
{
    printf("(CALL");
    ct->type->visit(*this);
    ct->args->visit(*this);
    printf(")");
}

void Visitor::visit(Arguments *args)
{
    ;

}
void Visitor::visit(IdentifierNode *ident)
{
    printf(" ");
    ident->get_id_type()->visit(*this);
    printf(" %s ", ident->id.c_str());
}




