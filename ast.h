
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <list>
#include <string>
#include <assert.h>
#include <iostream>


using namespace std;
// Forward declarations
class Scope;
class Declaration;
class Declarations;
class Vistior;
class Type;
class Node;
class Identifier;

typedef enum {
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),

  EXPRESSION_NODE       = (1 << 2),
  UNARY_EXPRESION_NODE  = (1 << 2) | (1 << 3),
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
} NodeKind;


extern void* ast;

class Visitor {
public:
    void visit(Identifier *idet);
    void visit(Scope *scope);
    void visit(Declaration *decl);
    void visit(Type *type);
};

class Node {
    public:
        virtual void visit(Visitor &vistor) = 0;
};

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
        string initial_val;
        virtual void visit(Visitor &visitor) {
            visitor.visit(this);
        }
};

class Declarations : public Node {
    public:
        Declarations *declarations;
        Declaration *declaration;
        virtual void visit(Visitor &visitor) {
            visitor.visit(this);
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

void *ast_allocate(NodeKind type, ...);

void ast_print(void *ast_root);
#endif /* AST_H_ */
