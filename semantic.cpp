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

std::string get_base_type (const std::string type){
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
    /* Add a class member to track all of the semantic errors */

    public:
        virtual void visit(VectorVariable *vv){
            if (vv->get_id_type() == nullptr)
                return; /* We already reported errors on symbol table creation */
            std::string type_name = vv->get_id_type()->type_name;

            int vec_dimension = get_type_dimension(type_name) - 1; /* array index is always one less than dimension */
            if (vv->vector_index > vec_dimension || vv->vector_index < 0)
                printf("Error: vector index out of bounds (vector: %s, index: %d, bound: 0-%d)",
                        vv->id.c_str(), vv->vector_index, vec_dimension); /* TODO: add line number */
        }

        virtual void visit(ConstructorExpression *ce){

        }
        virtual void visit(FloatLiteralExpression *fle){
            fle->set_expression_type("float");
        }
        virtual void visit(BoolLiteralExpression *ble){
            ble->set_expression_type("bool");
        }

        virtual void visit(IntLiteralExpression *ile){
            ile->set_expression_type("int");
        }

        virtual void visit(UnaryExpression *ue){

        }
        virtual void visit(BinaryExpression *be){
        }

        virtual void visit(VariableExpression *ve){
            ve->id_node->visit(*this);

            Type *variable_type = ve->id_node->get_id_type(); /* Note: due to the nature of parser, id_node exists by default */
            if (variable_type != nullptr){
                std::string base_type = get_base_type(ve->id_node->get_id_type()->type_name);
                ve->set_expression_type(base_type); /* Set the expression type to be the base type of an id */
            }
        }
        virtual void visit(FunctionExpression *fe){

        }

        virtual void visit(AssignStatement *assign_stmt){
            /* Visit the members to set the inference types */
            assign_stmt->variable->visit(*this);
            assign_stmt->expression->visit(*this);

            std::string rhs_type = assign_stmt->expression->get_expression_type ();
            Type *temp_type = assign_stmt->variable->get_id_type();
            std::string lhs_type =  temp_type ? temp_type->type_name : "ANY_TYPE";

            if (rhs_type != lhs_type) {
                printf("Error: Can not assign a different type expression to a variable\n"); // Put line number
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
    /* This creates predefined variables, and load the source file into the scope */

    /* This performs construction of symbol table, and scope checking */
    SymbolVisitor symbol_visitor;
    PostOrderVisitor postorder_visitor;

    /* This performs type inference and Type checking */
    ast->visit(symbol_visitor);
    ast->visit(postorder_visitor);

    /* This section prints out the errors collected overall, including line numbers */

    return 0;
}
