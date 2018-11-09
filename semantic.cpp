#include <string.h>
#include "symbol.h"
#include "ast.h"

class SymbolVisitor : public Visitor
{
    private:
        SymbolTablex m_symbol_table;

    public:
        virtual void visit(Scope *scope)
        {
            m_symbol_table.enter_scope();
            scope->declarations->visit(*this);
            scope->statements->visit(*this);
            m_symbol_table.exit_scope();
        }
        virtual void visit(Declaration *decl)
        {
            decl->type->visit(*this);
            if (decl->initial_val != nullptr)
            {
                decl->initial_val->visit(*this);
            }

            Declaration *temp = m_symbol_table.create_symbol(decl);
            if (temp)
            {
                printf("Error: Redeclaration of symbol %s with type %s\n",
                        temp->id.c_str(), temp->type->type_name.c_str());
            }
        }
        virtual void visit(IdentifierNode *var)
        {
            Declaration *declaration = m_symbol_table.find_symbol(var->id);
            if (declaration == nullptr){
                printf("Error: Missing declaration for symbol %s\n", var->id.c_str());
            }
            else {
                var->set_declaration(declaration);
            }
        }
};


class PostOrderVisitor : public Visitor
{

    public:
        virtual void visit(ConstructorExpression *ce){

        }
        virtual void visit(FloatLiteralExpression *fle){

        }
        virtual void visit(BoolLiteralExpression *ble){

        }
        virtual void visit(IntLiteralExpression *ile){

        }
        virtual void visit(UnaryExpression *ue){

        }
        virtual void visit(BinaryExpression *be){

        }
        virtual void visit(VariableExpression *ve){

        }
        virtual void visit(FunctionExpression *fe){

        }
};


int semantic_check(node * ast)
{
    /* This performs construction of symbol table, and scope checking */
    SymbolVisitor symbol_visitor;
    ast->visit(symbol_visitor);




    return 0;
}
