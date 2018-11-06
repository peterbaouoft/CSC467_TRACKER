#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0


/*========================================Initial Declarations===================*/
class Type : public Node {
    public:
        virtual void visit(Visitor &visitor){
            visitor.visit(this);
        }
        std::string type_name;

        Type(std::string type){
            this->type_name = type;
        }
};

class Identifier : public Node {
    public:
        std::string name;
        virtual void visit(Visitor &visitor){
            visitor.visit(this);
        }
        Identifier(std::string val){
            this->name = val;
        }
};

class Declaration : public Node {
    public:
        Type *t;
        Identifier *i;
        std::string initial_val;
        virtual void visit(Visitor &visitor) {
            visitor.visit(this);
            // i->visit(visitor);
            t->visit(visitor);
            if (initial_val.length() != 0)
                cout << initial_val;
            printf(")\n");
        }
};

class Declarations : public Node {
    public:
        Declarations *declarations;
        Declaration *declaration;
        virtual void visit(Visitor &visitor) {
            if (declaration != NULL)
                declaration->visit(visitor);
            if (declarations != NULL)
                declarations->visit(visitor);
        }
};


class  Scope: public Node {
    public:
        Declarations* d;
        virtual void visit(Visitor &visitor) {
            visitor.visit(this);
            if (d)
                d->visit(visitor);
            //s->visit(visitor);
            printf(")\n");
        }
};
/*=========================================END OF INITIAL DECLARATIONS======================*/

/*=========================Beginning of STATEMENT class===================================*/
class Statements {

    public:
        Statements *statements;
        Statement  *statement;
};

class Statement {
    public:
        StatementType statement_type;
        Statement(StatementType st){
            this->statement_type = st;
        }
};

class AssignStatement : public Statement {
    public:
        Variable *variable;
        Expression *expression;
};

class IfStatement : public Statement {
    public:
        Expression *expression;
        Statement *statement;
        Statement *else_statement;

};
class Nested_scope : public Statement {
    public:
        Statement *statement;
};
/*================END OF STATEMENT CLASS================*/

/*================Start of Expression classes===========*/
class Expression : public Node {
    public:
        ExpressionType expression_type;
        Expression(ExpressionType et){
            this->expression_type = et;
        }
};

class ConstructorExpression : public Expression {
    public:
        Constructor *constructor;
};

class LiteralExpression : public Expression {
    public:
        int int_literal;
        float float_literal;
        bool bool_literal;
};

class UnaryExpression : public Node {
    public:
        string op;
        Expression *right_expression;
};

class BinaryExpression : public UnaryExpression {
    public:
        Expression *left_expression;
};

class Constructor : public Node {
    public:
        Type *type;
        Arguments *args;
};

class Arguments : public Node {
    public:
        Arguments *args;
        Expression *expression;
};

class Variable : public Node {
    public:
        Identifier *id;
};
/*===========================End of expression classes========================*/

node* ast = NULL;

node* ast_allocate(NodeKind type, ...){

    va_list args;

    node *ret_node = NULL;
    va_start(args, type);
    switch(type) {
        case SCOPE_NODE: {
            Scope* scope = new Scope();
            scope->d = va_arg(args, Declarations*);

            ret_node = scope;
            break;
        }

        case DECLARATIONS_NODE:{
            Declarations *declarations = new Declarations();
            declarations->declarations = va_arg(args, Declarations*);
            declarations->declaration =  va_arg(args, Declaration*);

            ret_node = declarations;
            break;
        }

        case DECLARATION_NODE: {
            Declaration *declaration = new Declaration();
            declaration->i = va_arg(args, Identifier*);
            declaration->t = va_arg(args, Type*);

            ret_node = declaration;
            break;
        }
        case IDENT_NODE: {
            string val = static_cast<string>(va_arg(args, char*));
            ret_node = new Identifier(val);
            break;
        }
        case TYPE_NODE: {
            int type_index = va_arg(args, int);
            int dimen_index = va_arg(args, int);
            string type_list[3][4]  =  {{"int", "ivec2", "ivec3", "ivec4"},
                                       {"bool", "bvec2", "bvec3", "bvec4"},
                                       { "float", "vec2", "vec3", "vec4"}};
            if (type_index > 2 || dimen_index > 3)
                assert(true && "Index for type, please check for bugs\n");
            string type = type_list[type_index][dimen_index - 1]; /* dim goes from 1 to 4 */
            cout << "Test instantiations\n" << type;
            ret_node = new Type(type);
            break;
        }
        default:
            ;
    }

    va_end(args);
    return ret_node;

}

void ast_print(node* root){
    Visitor visitor;
    root->visit(visitor);
}

inline void Visitor::visit(Scope *scope){
    printf("(SCOPE \n");
}

inline void Visitor::visit(Identifier *ident){
    /* C++ does not supports string for printf */
    cout << ident->name;
    ;
}

inline void Visitor::visit(Declaration *decl){
    printf("(DECLARATION ");
}

inline void Visitor::visit(Type *type){
    cout << type->type_name;
}

