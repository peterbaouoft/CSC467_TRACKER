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

        virtual void visit(VectorVariable *vec_var)
        {
            Declaration *declaration = m_symbol_table.find_symbol(vec_var->id);

            if (declaration == nullptr){
                printf("Error: Missing declaration for symbol %s\n", vec_var->id.c_str());
            }
            else {
                vec_var->set_declaration(declaration);
            }
        }
};


int get_type_dimension (const std::string &type){
    if (type == "bvec2" || type == "ivec2" || type == "vec2")
        return 2;
    else if (type == "vec3" || type == "ivec3" || type == "bvec3")
        return 3;
    else if (type == "ivec4" || type == "bvec4" || type == "vec4")
        return 4;
    else
        return 1;
}

std::string get_base_type (const std::string &type){
    if (type == "bvec2" || type == "bvec3" || type == "bvec4")
        return "bool";
    else if (type == "vec2" || type == "vec3" || type == "vec4")
        return "float";
    else if (type == "ivec2" || type == "ivec3" || type == "ivec4")
        return "int";
    else
        return type;
}


class PostOrderVisitor : public Visitor
{
    public:
        virtual void visit(VectorVariable *vv){
            std::string type_name = vv->get_id_type()->type_name;

            if(!type_name.compare("ivec2") || !type_name.compare("bvec2") || !type_name.compare("vec2") ){
                if (vv->vector_index < 0 || vv->vector_index > 1){
                    printf  ("Error: vector index out of bounds (vector: %s, index: %d, bound: 0-1)",
                            vv->id.c_str(), vv->vector_index);
                    }
            }
            if(!type_name.compare("ivec3") || !type_name.compare("bvec3") || !type_name.compare("vec3") ){
                if (vv->vector_index < 0 || vv->vector_index > 2){
                    printf  ("Error: vector index out of bounds (vector: %s, index: %d, bound: 0-1)",
                            vv->id.c_str(), vv->vector_index);
                    }
            }
            if(!type_name.compare("ivec4") || !type_name.compare("bvec4") || !type_name.compare("vec4") ){
                if (vv->vector_index < 0 || vv->vector_index > 3){
                    printf  ("Error: vector index out of bounds (vector: %s, index: %d, bound: 0-1)",
                            vv->id.c_str(), vv->vector_index);
                    }
            }
        }
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

        virtual void visit(AssignStatement *assign_stmt){

            std::string rhs_type = assign_stmt->expression->get_expression_type ();
            std::string lhs_type = "ANY_TYPE" ;
            if(rhs_type == "ANY_TYPE" || lhs_type == "ANY_TYPE"){
                printf("Error: Expression or Variable is not evaluated to any supported type\n"); // Put line number
                return;
            }
            if (rhs_type != lhs_type) {
                printf("Error: Can not assign a different type expression to a variable"); // Put line number
                return;
            }
        }

        virtual void visit(IfStatement *if_statement) {
            /* You perform type inference by visiting other ones first */
            if_statement->expression->visit(*this);
            if_statement->statement->visit(*this);
            if (if_statement->else_statement){
                if_statement->else_statement->visit(*this);
            }
            if (if_statement->expression->get_expression_type() != "bool")
              printf("Error: Condition has to be a type of boolean\n"); // Put line numbers in
        }
};


int semantic_check(node * ast)
{
    /* This performs construction of symbol table, and scope checking */
    SymbolVisitor symbol_visitor;
    PostOrderVisitor postorder_visitor;
    ast->visit(symbol_visitor);
    ast->visit(postorder_visitor);



    return 0;
}
