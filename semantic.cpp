#include <string.h>
#include <iostream>
#include "symbol.h"
#include "ast.h"
#include "common.h"
#include "parser.tab.h"

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
            std::string base_type = get_base_type(type_name);
            vv->set_id_type(new Type(base_type)); /* Set the vector variable's type into base type */
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
            /* Set types accordingly */
            ue->right_expression->visit(*this);

            std::string type = ue->right_expression->get_expression_type();
            if (type == "ANY_TYPE") /* We reported earlier for any type errors */
                return;

            /* Do Operator checking */
            int operator_type = ue->operator_type;
            std::string base_type = get_base_type(type);
            switch (operator_type) {
                case NOT:
                {
                    if (base_type !=  "bool") {
                        printf("Error: Logical operators only work for boolean types\n");
                        type = "ANY_TYPE"; /* for upper error handling */
                    }
                    break;
                }
                case MINUS:
                {
                    if (base_type == "bool") {
                        printf("Error: Arithmatic operators only work for operator types\n");
                        type = "ANY_TYPE";
                    }
                    break;
                }
            }
            ue->set_unary_expr_type(type);
        }


        virtual void visit(BinaryExpression *be){
            be->left_expression->visit(*this);
            be->right_expression->visit(*this);

            std::string lhs_expr_type = be->left_expression->get_expression_type();
            std::string rhs_expr_type = be->right_expression->get_expression_type();

            printf("LHS type is %s\n", lhs_expr_type.c_str());
            printf("RHS type is %s\n", rhs_expr_type.c_str());


            /* goes back to the caller, with any type as default */
            if (lhs_expr_type == "ANY_TYPE" || rhs_expr_type == "ANY_TYPE")
                return;
            std::string lhs_base_type = get_base_type(lhs_expr_type);
            std::string rhs_base_type = get_base_type(rhs_expr_type);
            if (lhs_base_type != rhs_base_type){
                printf("Both operands of a binary operator must have exactly same base type\n"); /*TODO, put line number */
                return;
            }

            int operator_type = be->operator_type;
            int lhs_vec_dimen = get_type_dimension(lhs_expr_type);
            int rhs_vec_dimen = get_type_dimension(rhs_expr_type);

            /* Declare flag variables here which will be used in the follwoing */
            bool is_arithmetic = lhs_base_type !="bool";
            bool is_logical = !is_arithmetic;
            bool matching_dimen = lhs_vec_dimen == rhs_vec_dimen;
            bool is_lhs_scalar = lhs_vec_dimen == 1;
            bool is_rhs_scalar = rhs_vec_dimen == 1;

            /* ret_type to capture result */
            std::string ret_type = "ANY_TYPE";
            if (operator_type == AND || operator_type == OR){ /* Early returns */
                if (is_arithmetic)  {
                    printf("Logical operators only work for boolean types\n");
                    return;
                }
                if (!matching_dimen) {
                    printf("The expression dimension mismatches, one is %d, and the other is %d", lhs_vec_dimen, rhs_vec_dimen);
                    return;
                }
                ret_type = lhs_expr_type; /* Either left or right expression is a match */
                be->set_binary_expr_type(ret_type);
                return;
            }

            if (is_logical) {
                printf("Arithmetic operators only work for arithmetic types\n");
                return;
            }

            /* Plus and minus, we have ss and vv */
            if (operator_type == PLUS || operator_type == MINUS) {
                if (!matching_dimen) {
                    printf("The expression dimension mismatches, one is %d, and the other is %d\n", lhs_vec_dimen, rhs_vec_dimen);
                    return;
                }
                printf("WOT, the LHS vec dimension is %d\n", lhs_vec_dimen);
                ret_type = lhs_vec_dimen; /* LHS DIM = RHS DIM and both scalars and vectors work */
            }

            else if (operator_type == TIMES) {
                if (is_lhs_scalar && is_rhs_scalar) /* ss => s */
                    ret_type = lhs_expr_type; /* Both lhs and rhs works */
                else if (is_lhs_scalar && !is_rhs_scalar) /* sv => v */
                   ret_type = rhs_expr_type;
                else if (!is_lhs_scalar && is_rhs_scalar) /* vs => v */
                   ret_type = lhs_expr_type;
                else { /* vv => v */
                    if (!matching_dimen) {
                        printf("The expression dimension mismatches, one is %d, and the other is %d", lhs_vec_dimen, rhs_vec_dimen);
                        return;
                    }
                    ret_type = lhs_expr_type; /* Both lhs and rhs works */
                }
            }

            else if (operator_type == CARET || operator_type == DIVIDE) {
                if (!is_lhs_scalar || !is_rhs_scalar) {
                    printf("Divide and Caret operator only works on scalars \n"); /* Set line number */
                    return;
               }
                ret_type = lhs_expr_type;
            }

            else if (operator_type == GREATER || operator_type == SMALLER  || operator_type == S_EQ
                     || operator_type == G_EQ)
            {
                if (!is_lhs_scalar || !is_rhs_scalar) {
                    printf(" <, <=, >, >= operators only works on scalars \n"); /* Set line number */
                    return;
               }

                ret_type = "bool";
            }

            else if (operator_type == DOUBLE_EQ || operator_type == N_EQ)
            {
                if (!matching_dimen) {
                    printf("The expression dimension mismatches, one is %d, and the other is %d", lhs_vec_dimen, rhs_vec_dimen);
                    return;
                }
                ret_type = "bool";
            }

            else {
                printf("Unknown binary operator type\n");
                return;
            }

            be->set_binary_expr_type(ret_type);
        }

        virtual void visit(VariableExpression *ve){
            ve->id_node->visit(*this);

            Type *variable_type = ve->id_node->get_id_type(); /* Note: due to the nature of parser, id_node exists by default */
            if (variable_type != nullptr){
                // std::string base_type = get_base_type(ve->id_node->get_id_type()->type_name);
                ve->set_expression_type(variable_type->type_name); /* TODO: think about base type.. when printing */
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

class PredefinedVariableVisitor : public Visitor
{
    public:
        virtual void visit(Scope *scope){
            scope->declarations->visit(*this); /* We only need to visit declarations to push content in */
        }
        virtual void visit(Declarations *decls)
        {
            /* We have the following predefined variables
            result vec4 gl_FragColor ;
            result bool gl_FragDepth ;
            result vec4 gl_FragCoord ;
            attribute vec4 gl_TexCoord;
            attribute vec4 gl_Color;
            attribute vec4 gl_Secondary;
            attribute vec4 gl_FogFragCoord;
            uniform vec4 gl_Light_Half ;
            uniform vec4 gl_Light_Ambient ;
            uniform vec4 gl_Material_Shininess ;
            uniform vec4 env1;
            uniform vec4 env2;
            uniform vec4 env3;*/

            /* Result predefined variables */
            Declaration *gl_FragColor = new Declaration(new Type("vec4"), "gl_FragColor", nullptr, false);
            Declaration *gl_FragDepth = new Declaration(new Type("bool"), "gl_FragDepth", nullptr, false);
            Declaration *gl_FragCoord = new Declaration(new Type("vec4"), "gl_FragCoord", nullptr, false);
            gl_FragColor->set_is_write_only(true); /* Result type classes are all write only */
            gl_FragDepth->set_is_write_only(true);
            gl_FragCoord->set_is_write_only(true);

            /* Attribute Predefined variables */
            Declaration *gl_TexCoord = new Declaration(new Type("vec4"), "gl_TexCoord", nullptr, false);
            Declaration *gl_Color = new Declaration(new Type("vec4"), "gl_Color", nullptr, false);
            Declaration *gl_Secondary = new Declaration(new Type("vec4"), "gl_Secondary", nullptr, false);
            Declaration *gl_FogFradCoord = new Declaration(new Type("vec4"), "gl_FogFradCoord", nullptr, false);
            gl_TexCoord->set_is_read_only(true);
            gl_Color->set_is_read_only(true);
            gl_Secondary->set_is_read_only(true);
            gl_FogFradCoord->set_is_read_only(true);


            /* Uniform Predefined variables */
            Declaration *gl_Light_Half = new Declaration(new Type("vec4"), "gl_Light_Half", nullptr, true);
            Declaration *gl_Light_Ambient = new Declaration(new Type("vec4"), "gl_Light_Ambient", nullptr, true);
            Declaration *gl_Material_Shininess = new Declaration(new Type("vec4"), "gl_Material_Shininess", nullptr, true);
            Declaration *env1 = new Declaration(new Type("vec4"), "env1", nullptr, true);
            Declaration *env2 = new Declaration(new Type("vec4"), "env2", nullptr, true);
            Declaration *env3 = new Declaration(new Type("vec4"), "env3", nullptr, true);
            gl_Light_Half->set_is_read_only(true);
            gl_Light_Ambient->set_is_read_only(true);
            gl_Material_Shininess->set_is_read_only(true);
            env1->set_is_read_only(true);
            env2->set_is_read_only(true);
            env3->set_is_read_only(true);

            /* We now push all of them into our ast node, in this case, it is the declarations */
            decls->push_front_declaration(gl_FragColor);
            decls->push_front_declaration(gl_FragDepth);
            decls->push_front_declaration(gl_FragCoord);
            decls->push_front_declaration(gl_TexCoord);
            decls->push_front_declaration(gl_Color);
            decls->push_front_declaration(gl_Secondary);
            decls->push_front_declaration(gl_FogFradCoord);
            decls->push_front_declaration(gl_Light_Half);
            decls->push_front_declaration(gl_Light_Ambient);
            decls->push_front_declaration(gl_Material_Shininess);
            decls->push_front_declaration(env1);
            decls->push_front_declaration(env2);
            decls->push_front_declaration(env3);
        }
};


int semantic_check(node * ast)
{

    PredefinedVariableVisitor predefined_visitor;
    SymbolVisitor symbol_visitor;
    PostOrderVisitor postorder_visitor;

    /* This creates predefined variables, and load the source file into the scope */
    ast->visit(predefined_visitor);

    /* This performs construction of symbol table, and scope checking */
    ast->visit(symbol_visitor);

    /* This performs type inference and Type checking */
    ast->visit(postorder_visitor);

    /* This section prints out the errors collected overall, including line numbers */

    return 0;
}
