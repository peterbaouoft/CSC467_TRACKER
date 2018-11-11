#include <string.h>
#include <iostream>
#include "semantic.h"
#include "symbol.h"
#include "ast.h"
#include "common.h"
#include "parser.tab.h"
#include <vector>

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

class ExpressionVisitor : public Visitor
{
    private:
        int expression_instance_type = -1;
    public:
        int get_expression_instance_type() const {return expression_instance_type;}
        void set_expression_instance_type(int type) {expression_instance_type = type;}

        /* Empty visiting statements, as we only need to retrieve the current level type */
        virtual void visit(Scope *scope) {}
        virtual void visit(Declaration *decl) {}
        virtual void visit (Declarations *decls) {}
        virtual void visit(Type *type) {}

        virtual void visit(Statement *stmt) {}
        virtual void visit(Statements *stmts) {}
        virtual void visit(AssignStatement *as_stmt) {}
        virtual void visit(IfStatement *if_statement) {}
        virtual void visit(NestedScope *ns) {}
        virtual void visit(EmptyStatement *es) {}

        virtual void visit(ConstructorExpression *ce) {set_expression_instance_type(CONSTRUCTOR_EXPRESSION);}
        virtual void visit(FloatLiteralExpression *fle) {set_expression_instance_type(FLOAT_LITERAL);}
        virtual void visit(BoolLiteralExpression *ble) {set_expression_instance_type(BOOL_EXPRESSION); }
        virtual void visit(IntLiteralExpression *ile) {set_expression_instance_type(INT_LITERAL);}
        virtual void visit(UnaryExpression *ue) {}
        virtual void visit(BinaryExpression *be) {}
        virtual void visit(VariableExpression *ve) {set_expression_instance_type(VARIABLE);}
        virtual void visit(FunctionExpression *fe) {set_expression_instance_type(FUNCTION);}


        virtual void visit(Function *f) {}
        virtual void visit(Constructor *c) {}

};

class PostOrderVisitor : public Visitor
{
    /* Add a class member to track all of the semantic errors */
    private:
        int if_else_scope_counter = 0;
        ExpressionVisitor expression_visitor;
    public:

        virtual void visit(Declaration *decl){
            if (decl->initial_val == nullptr) /* We don't need to check for no initalized declarations */
                return;

            /* First do a type inference */
            decl->initial_val->visit(*this);

            /* Then we want to get its correspondent expression type */
            decl->initial_val->visit(expression_visitor);

            /* We achieve that by using a visitor which only stops at the immediate next level */
            int expression_instance_type = expression_visitor.get_expression_instance_type();
            if (decl->get_is_const()) {
                bool is_valid = false;
                /* Literal initialization */
                if (expression_instance_type == FLOAT_LITERAL || expression_instance_type == BOOL_EXPRESSION ||
                    expression_instance_type == INT_LITERAL)
                    is_valid = true;

                /* Uniform variable */
                else if (expression_instance_type == VARIABLE)
                {
                    VariableExpression *ve = reinterpret_cast<VariableExpression*>(decl->initial_val); /* It is safe since we already know the expression type */
                    Declaration *variable_decl = ve->id_node->get_declaration();
                    if (variable_decl->get_is_read_only() && variable_decl->get_is_const())
                        is_valid = true;
                    else
                        is_valid = false;
                }

                if(!is_valid)
                    printf("Error: const qualified variable %s must be initalized with a literal value or"
                            " a uniform variable, not an expression\n", decl->id.c_str());
                // printf("The expression type is %d\n", expression_instance_type);
                // printf("The tested variable type is %d\n", VARIABLE);
                // printf("WOCAO\n, %s", decl->get_is_read_only() ? "true" : "false");
            }

        }

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
            ce->constructor->visit(*this);
            std::string type = ce->constructor->type->type_name;
            std::string base_type = get_base_type (ce->constructor->type->type_name);
            std::vector<Expression *> expression_list = ce->constructor->args->get_expression_list();
            int type_dimension = get_type_dimension (type);

            // check dimension
            if (type_dimension != (int)expression_list.size()){
                printf ("\nError: number of arguments (%d) doesn't match type dimension (%d)\n",
                        type_dimension, (int)expression_list.size());
                return; /* We might want to have early returns, as, we don't want to report too many errors ?\n */
            }
            // check type
            for (int i=0; i< (int)expression_list.size(); i++){
                std::string arg_type = expression_list[i]->get_expression_type();
                if (arg_type != base_type){
                    printf ("Error: argument type (%s) and constructor type (%s) mismatch\n",
                            arg_type.c_str(), base_type.c_str());
                }
            }
        }

        virtual void visit(FunctionExpression *fe){
            fe->function->visit(*this);

            std::string function_name = fe->function->function_name;
            std::vector<Expression *> args = fe->function->arguments->get_expression_list();

            if (function_name == "rsq"){
                if (args.size() != 1){
                    printf("\nError: rsq function has %d argument (only 1 allowed)\n", (int)args.size());
                    return;
                }
                for(int i=0; i<(int)args.size(); i++){
                    std::string type = args[i]->get_expression_type();
                    if (type != "int" || type != "float"){
                        printf("Error: rsq function has %s type as argument (only int/float allowed)\n", type.c_str());
                        return;
                    }
                }
                fe->set_expression_type("float");
            }
            if (function_name == "dp3"){
                if (args.size() != 2){
                    printf("\nError: rsq function has %d argument (only 2 allowed)\n", (int)args.size());
                    return;
                }
                std::string type_1 = args[0]->get_expression_type();
                std::string type_2 = args[1]->get_expression_type();
                if ((type_1 != "vec3" && type_2 != "vec3") || 
                    (type_1 != "vec4" && type_2 != "vec4") ||
                    (type_1 != "ivec3" && type_2 != "ivec3") ||
                    (type_1 != "ivec4" && type_2 != "ivec4"))
                {
                    printf ("Error: dp3 function has %s, %s type as arguments (both args must be vec3/vec4/ivec3/ivec4)",
                            type_1.c_str(), type_2.c_str());
                    return;
                }
                fe->set_expression_type("float");
            }
            if (function_name == "lit"){
                if (args.size() != 1){
                    printf("\nError: lit function has %d argument (only 1 allowed)\n", (int)args.size());
                    return;
                }
                for(int i=0; i<(int)args.size(); i++){
                    std::string type = args[i]->get_expression_type();
                    if (type != "vec4"){
                        printf("Error: lit function has %s type as argument (only vec4 allowed)\n", type.c_str());
                        return;
                    }
                }
                fe->set_expression_type("vec4");
            }
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
            ue->set_expression_type(type);
        }


        virtual void visit(BinaryExpression *be){
            be->left_expression->visit(*this);
            be->right_expression->visit(*this);

            std::string lhs_expr_type = be->left_expression->get_expression_type();
            std::string rhs_expr_type = be->right_expression->get_expression_type();

            // printf("LHS type is %s\n", lhs_expr_type.c_str());
            // printf("RHS type is %s\n", rhs_expr_type.c_str());

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
                be->set_expression_type(ret_type);
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

            be->set_expression_type(ret_type);
        }

        virtual void visit(VariableExpression *ve){
            ve->id_node->visit(*this);

            Declaration *declaration = ve->id_node->get_declaration();

            if (declaration && declaration->get_is_write_only()) {
                printf("Error: Variable %s has Result type class and is write only\n", declaration->id.c_str());
                return;
            }

            Type *variable_type = ve->id_node->get_id_type(); /* Note: due to the nature of parser, id_node exists by default */
            if (variable_type != nullptr){
                // std::string base_type = get_base_type(ve->id_node->get_id_type()->type_name);
                ve->set_expression_type(variable_type->type_name); /* TODO: think about base type.. when printing */
            }
        }
        virtual void visit(AssignStatement *assign_stmt){
            /* Visit the members to set the inference types */
            assign_stmt->variable->visit(*this);
            assign_stmt->expression->visit(*this);

            std::string rhs_type = assign_stmt->expression->get_expression_type ();
            Type *temp_type = assign_stmt->variable->get_id_type();
            std::string lhs_type =  temp_type ? temp_type->type_name : "ANY_TYPE";

            // printf("LHS type is %s\n\n", lhs_type.c_str());
            // printf("RHS type is %s\n\n", rhs_type.c_str());
            if (rhs_type == "ANY_TYPE" || lhs_type == "ANY_TYPE")
                return;

            if (rhs_type != lhs_type) {
                printf("Error: Can not assign a different type expression to a variable\n"); // Put line number
                return;
            }

            /* Const + Uniform + Attribute Checking */
            Declaration *variable_declaration = assign_stmt->variable->get_declaration();
            {
                if (variable_declaration) {
                    if (variable_declaration->get_is_const())
                        printf("Error : const qualified variable %s can not be re-assigned\n", variable_declaration->id.c_str());
                    else if(variable_declaration->get_is_read_only())
                        printf("Error: Can not assign to a read only variable %s\n", variable_declaration->id.c_str());
                    else if(if_else_scope_counter != 0 && variable_declaration->get_is_write_only())
                        printf("Error: Variable %s with Result type classes can not be assigned anywhere in the scope of an if or else statement \n", variable_declaration->id.c_str());

                    return;
                }
            }
        }

        virtual void visit(IfStatement *if_statement) {
            /* You perform type inference by visiting other ones first */
            if_statement->expression->visit(*this);
            if_else_scope_counter++;
            if_statement->statement->visit(*this);
            if_else_scope_counter--;
            if (if_statement->else_statement){
                if_else_scope_counter++;
                if_statement->else_statement->visit(*this);
                if_else_scope_counter--;
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
            gl_FragColor->set_is_write_only(true); /* Result type classes are all write only */
            gl_FragDepth->set_is_write_only(true);

            /* Attribute Predefined variables */
            Declaration *gl_FragCoord = new Declaration(new Type("vec4"), "gl_FragCoord", nullptr, false);
            Declaration *gl_TexCoord = new Declaration(new Type("vec4"), "gl_TexCoord", nullptr, false);
            Declaration *gl_Color = new Declaration(new Type("vec4"), "gl_Color", nullptr, false);
            Declaration *gl_Secondary = new Declaration(new Type("vec4"), "gl_Secondary", nullptr, false);
            Declaration *gl_FogFradCoord = new Declaration(new Type("vec4"), "gl_FogFradCoord", nullptr, false);
            gl_FragCoord->set_is_read_only(true);
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
