#include "ast.h"
#include "common.h"
#include <sstream>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "parser.tab.h"


class ARBAssemblyTable
{
    private:

        std::stringstream buffer;
        std::unordered_map<std::string, std::string> m_name_map;
        std::string zero_vector = "__zero__vector__";
        int temp_register_counter = 0;

    public:
        /* Init of assembly table, create name mapping, ARB assembly prefix and so on */
        ARBAssemblyTable(){
            m_name_map.emplace("gl_FragColor", "result.color");
            m_name_map.emplace("gl_FragDepth", "result.depth");
            m_name_map.emplace("gl_FragCoord", "fragment.position");

            m_name_map.emplace("gl_TexCoord", "fragment.texcoord");
            m_name_map.emplace("gl_Color", "fragment.color");
            m_name_map.emplace("gl_Secondary", "fragment.color.secondary");
            m_name_map.emplace("gl_FogFradCoord", "fragment.fogcoord");

            m_name_map.emplace("gl_Light_Half", "state.light[0].half");
            m_name_map.emplace("gl_Light_Ambient", "state.lightmodel.ambient");
            m_name_map.emplace("gl_Material_Shininess", "state.material.shininess");
            m_name_map.emplace("env1", "program.env[1]");
            m_name_map.emplace("env2", "program.env[2]");
            m_name_map.emplace("env3", "program.env[3]");
        }

        std::string insert_register_name_into_map(const std::string variable_name){
            if (get_id_to_name_mapping(variable_name) != variable_name)
                return ""; // We don't handle if and else statements in our compiler yet

            std::string created_register_name = "__" + variable_name + "__";
            m_name_map.emplace(variable_name, created_register_name);
            return created_register_name;
        }

        std::string create_temp_register_name() {
            temp_register_counter++;

            std::string temp_register_name = "temp" + std::to_string(temp_register_counter);
            std::string return_str = insert_register_name_into_map(temp_register_name);
            assert(return_str != "");

            return return_str;
        }

    public:
        /* Purely based on the literal meaning, you return the index correspondent to x,y,z,w */
        std::string get_index_to_characater_mapping(int index) const{
            switch (index)
            {
                case 0:
                    return "x";
                case 1:
                    return "y";
                case 2:
                    return "z";
                case 3:
                    return "w";
                default:
                    assert(0); // Error out,
            }

            return "ERROR";
        }

        std::string create_assembly_instruction(AssemblyInstructionType type, ...){
            va_list args;
            va_start(args, type);

            std::string result_str = "";
            switch (type)
            {
            case MOV_INSTRUCTION:
            {
                std::string output_register = va_arg(args, std::string);
                std::string input_register = va_arg(args, std::string);
                result_str = "MOV " + output_register + ", " + input_register + ";" ;
                break;
            }

            case TEMP_INSTRUCTION:
            {
                std::string register_name = va_arg(args, std::string);
                result_str = "TEMP " + register_name + ";";
                break;

            }

            case MUL_INSTURCTION:
            {
                std::string output_location = va_arg(args, std::string);
                std::string left_input = va_arg(args, std::string);
                std::string right_input = va_arg(args, std::string);

                result_str = "MUL " + output_location + ", "  + left_input + ", " + right_input + ";";
                break;
            }

            case CONST_REGISTER:
            {
                std::string const_variable_name = va_arg(args, std::string);
                std::string right_hand_result = va_arg(args, std::string);

                result_str = "PARAM " + const_variable_name + " = " + right_hand_result + ";";
                break;
            }
            }

            va_end(args); // End the va_args
            return result_str;
        }

        std::string get_assembly_translation(NodeKind type, ...){
            va_list args;
            va_start(args, type);

            std::string result_str;
            switch (type)
            {
            case SCOPE_NODE:
            {
                /* We only want the of ABVfp1.0 to appear once */
                buffer << "!!ARBfp1.0" << std::endl;
                buffer << std::endl;
                buffer << "PARAM __zero__vector__ = {0.0, 0.0, 0.0, 0.0};" << std::endl;
                break;
            }

            case DECLARATION_NODE:
            {
                Declaration *decl = va_arg(args, Declaration *);
                // Get the register name of the declaration
                std::string decl_register_name = insert_register_name_into_map(decl->id);

                if (decl_register_name == "") // Do nothing for the predefined variables or duplicated names
                    break;

                buffer << create_assembly_instruction(TEMP_INSTRUCTION, decl_register_name) << std::endl;

                if (decl->initial_val == nullptr)
                    buffer << create_assembly_instruction(MOV_INSTRUCTION, decl_register_name, zero_vector) << std::endl;
                else
                    buffer << create_assembly_instruction(MOV_INSTRUCTION, decl_register_name, \
                                                        decl->initial_val->get_result_register_name()) << std::endl;

                break;
            }

            case CONST_DECLARATION_NODE:
            {
                Declaration *decl = va_arg(args, Declaration *);
                int expression_instance_type = va_arg(args, int);

                std::string const_decl_register_name = insert_register_name_into_map(decl->id);

                if (const_decl_register_name == "")
                    break;


                if (decl->initial_val == nullptr)
                    buffer << create_assembly_instruction(CONST_REGISTER, const_decl_register_name, zero_vector) << std::endl;
                else {
                    assert(decl->initial_val->get_result_register_name() != "");
                    buffer << create_assembly_instruction(CONST_REGISTER, const_decl_register_name, \
                                                          decl->initial_val->get_result_register_name()) << std::endl;
                }
                break;

            }

            case VECTOR_NODE:
            {
                VectorVariable *vec_var = va_arg(args, VectorVariable*);

                buffer << get_id_to_name_mapping(vec_var->id);
                buffer << "." << get_index_to_characater_mapping(vec_var->vector_index);
                break;
            }

            case BINARY_EXPRESSION_NODE:
            {
                BinaryExpression *be = va_arg(args, BinaryExpression*);
                int operator_type = va_arg(args, int);
                std::string left_result_name = va_arg(args, std::string);
                std::string right_result_name = va_arg(args, std::string);

                // Create a temp register to store the expression result
                std::string result_register_name = create_temp_register_name();
                buffer << create_assembly_instruction(TEMP_INSTRUCTION, result_register_name) << std::endl;

                switch (operator_type)
                {
                    case TIMES:
                    {
                        buffer << create_assembly_instruction(MUL_INSTURCTION, result_register_name, \
                                                            left_result_name, right_result_name) << std::endl;
                        break;
                    }
                }
                // Set the result register name for future references
                be->set_result_register_name(result_register_name);
                break;
            }


            case ASSIGNMENT_NODE:
            {
                // TODO: May be we could recycle all of the used temp register within assign statement(i.e temp1, temp2, temp3 and temp4 etc)
                std::string variable_name  = va_arg(args, std::string);
                std::string right_hand_result = va_arg(args, std::string);

                std::string register_name = get_id_to_name_mapping(variable_name);
                assert(register_name != "");

                buffer << create_assembly_instruction(MOV_INSTRUCTION, register_name, right_hand_result) << std::endl;
                break;
            }

            case CONSTRUCTOR_NODE:
            {
                Constructor *constructor = va_arg(args, Constructor*);
                int num_expression = get_type_dimension(constructor->type->type_name);
                std::string constructor_expression_type = get_base_type(constructor->type->type_name);


                buffer << "{";
                for (int i = 0; i < num_expression; i++)
                {
                    // Cast to each type to access their values
                    if (constructor_expression_type == "bool") {
                        BoolLiteralExpression *ble = reinterpret_cast<BoolLiteralExpression *>(constructor->args->get_expression_list()[i]);
                        buffer << std::to_string(ble->bool_literal);
                    }
                    else if(constructor_expression_type == "int"){
                        IntLiteralExpression *ile = reinterpret_cast<IntLiteralExpression *>(constructor->args->get_expression_list()[i]);
                        buffer << std::to_string(ile->int_literal);
                    }
                    else if (constructor_expression_type == "float"){
                        FloatLiteralExpression *fle = reinterpret_cast<FloatLiteralExpression *>(constructor->args->get_expression_list()[i]);
                        buffer << std::to_string(fle->float_literal);
                    }

                    if (i != num_expression - 1)
                        buffer << ", ";
                }
                buffer << "}";
                break;
            }

            }

            va_end(args);
            result_str = buffer.str();
            buffer.str("");
            return result_str;
        }

        std::string get_id_to_name_mapping(const std::string &id){
            auto name_iter = m_name_map.find(id);
            if (name_iter == m_name_map.end())
               return id;
            else
                return name_iter->second; /* Return the mapped result */
        }


    // Destructor
    public:
        ~ARBAssemblyTable () {}
};


class codeGenVisitor : public Visitor
{
    private:
        ARBAssemblyTable assembly_table;
        std::vector<std::string> m_instruction_list;
        ExpressionVisitor expr_visitor;

    public:
        codeGenVisitor(){
            std::string scope_str = assembly_table.get_assembly_translation(SCOPE_NODE);
            push_back_instruction(scope_str);
        }

        std::string get_assembly_id_str(IdentifierNode *var){
            var->visit(expr_visitor);
            int variable_type = expr_visitor.get_expression_instance_type();

            std::string result_str = "";
            if (variable_type == TEMP_VECTOR_EXPRESSION) {
                VectorVariable *vector_variable = reinterpret_cast<VectorVariable *>(var);
                result_str  = assembly_table.get_assembly_translation(VECTOR_NODE, vector_variable);
            }

            else if(variable_type == TEMP_ID_EXPRESSION) {
                result_str = assembly_table.get_id_to_name_mapping(var->id);
            }
            else{
                assert(0); // Can not happen
            }
            return result_str;
        }


    public:

        virtual void visit(Declaration *decl) {
            // Visit the sub expression to get initial values ready
            if (decl->initial_val != nullptr) {
                decl->initial_val->visit(*this);
            }

            std::string decl_instructions = "";
            if (decl->get_is_const())
                decl_instructions = assembly_table.get_assembly_translation(CONST_DECLARATION_NODE, decl);
            else
                decl_instructions = assembly_table.get_assembly_translation(DECLARATION_NODE, decl);
            if (decl_instructions == "") // Predefined or error out
                return;

            push_back_instruction(decl_instructions); // Collect the declarations
        }

        virtual void visit(AssignStatement *assign_stmt) {

            assign_stmt->variable->visit(*this);
            assign_stmt->expression->visit(*this);

            std::string assembly_variable_str = get_assembly_id_str(assign_stmt->variable);
            std::string assign_stmt_instructions = assembly_table.get_assembly_translation(ASSIGNMENT_NODE, assembly_variable_str, \
                                                                                            assign_stmt->expression->get_result_register_name());

            push_back_instruction(assign_stmt_instructions);
        }

        virtual void visit(IfStatement *if_statement) {
            assert(if_statement->expression != nullptr);
            if_statement->expression->visit(*this);
            if_statement->expression->visit(expr_visitor);

            bool expression_val = false;
            int expression_instance_type = expr_visitor.get_expression_instance_type();
            // We evaluate the expression and if it is true, let's skip the else statement
            if (expression_instance_type == BOOL_EXPRESSION) {
                BoolLiteralExpression *ble =  reinterpret_cast<BoolLiteralExpression *>(if_statement->expression);
                expression_val = ble->bool_literal;

                // We do deadcode elimination here, and ignore either if stataement, or else statement;
                if (expression_val){
                    if_statement->statement->visit(*this);
                }
                else {
                    if (if_statement->else_statement)
                        if_statement->else_statement->visit(*this);
                }
            }
            else{
                if_statement->statement->visit(*this);
                if (if_statement->else_statement)
                    if_statement->else_statement->visit(*this);
            }
        }

        virtual void visit(BinaryExpression *be) {

            be->left_expression->visit(*this);
            be->right_expression->visit(*this);

            // Get the left expression and right expression register name (it can be in memory or just register)
            std::string left_result_name = be->left_expression->get_result_register_name();
            std::string right_result_name = be->right_expression->get_result_register_name();
            assert(left_result_name != "");
            assert(right_result_name != "");

            int operator_type = be->operator_type;


            std::string binary_result_instruction = assembly_table.get_assembly_translation(BINARY_EXPRESSION_NODE, be, operator_type, \
                                                                                            left_result_name, right_result_name);
            push_back_instruction(binary_result_instruction);
        }

        virtual void visit(VariableExpression *ve) {
            ve->id_node->visit(*this);

            // Set the result register name so upper layer can see it
            std::string result_str = get_assembly_id_str(ve->id_node);
            ve->set_result_register_name(result_str);
        }

        virtual void visit(ConstructorExpression *ce) {
            ce->constructor->visit(*this);

            std::string result_str =  assembly_table.get_assembly_translation(CONSTRUCTOR_NODE, ce->constructor);
            assert(result_str != "");
            // Atm, compiler only supports const declared constructors.. i.e vec4(1.0, 2.0, 3.0, 4.0)
            // where all input are literals
            ce->set_result_register_name(result_str);

        }

    public:
        void push_back_instruction(std::string instruction) {m_instruction_list.push_back(instruction);}

        void write_out_instructions() {
            for (std::string instruction : m_instruction_list){
                std::cout << instruction << std::endl;
            }
        }



};

int genCode(node *ast)
{
    codeGenVisitor code_visitor;
    ast->visit(code_visitor);
    code_visitor.push_back_instruction("END");
    code_visitor.write_out_instructions();

    return 1;
}
