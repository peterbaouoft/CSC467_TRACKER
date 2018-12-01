#include "ast.h"
#include "common.h"
#include <sstream>
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <unordered_map>


class ARBAssemblyTable
{
    private:

        std::stringstream buffer;
        std::unordered_map<std::string, std::string> m_name_map;   
    
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

        void insert_register_name_into_map(const std::string varaible_name){
            if (get_id_to_name_mapping(variable_name) == variable_name)
                assert(0); // We don't handle if and else statements in our compiler yet
            
            m_name_map.emplace(variable_name, "__" + variable_name + "__");
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
                break;
            }
            case VECTOR_NODE:
            {
                VectorVariable *vec_var = va_arg(args, VectorVariable*);
                
                buffer << get_id_to_name_mapping(vec_var->id); 
                buffer << "." << get_index_to_characater_mapping(vec_var->vector_index);
            }

            }
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
    public:
        /* Empty visiting statements, as we only need to retrieve the current level type */
        
        virtual void visit(Scope *scope) {
            std::string scope_str = assembly_table.get_assembly_translation(SCOPE_NODE);
            push_back_instruction(scope_str);
            scope->declarations->visit(*this);
            scope->statements->visit(*this);
        } 
        // virtual void visit(Declaration *decl) {}
        // virtual void visit (Declarations *decls) {}

        // virtual void visit(Statement *stmt) {}
        // virtual void visit(Statements *stmts) {}
        // virtual void visit(AssignStatement *as_stmt) {}
        // virtual void visit(IfStatement *if_statement) {}
        // virtual void visit(NestedScope *ns) {}
        // virtual void visit(EmptyStatement *es) {}

        // virtual void visit(ConstructorExpression *ce) {}
        // virtual void visit(FloatLiteralExpression *fle) {}
        // virtual void visit(BoolLiteralExpression *ble) { }
        // virtual void visit(IntLiteralExpression *ile) {}
        // virtual void visit(UnaryExpression *ue) {}
        // virtual void visit(BinaryExpression *be) {}
        // virtual void visit(VariableExpression *ve) {}
        // virtual void visit(FunctionExpression *fe) {s}


        // virtual void visit(Function *f) {}
        // virtual void visit(Constructor *c) {}
        virtual void visit(VectorVariable *vv) {
            std::string vector_str = assembly_table.get_assembly_translation(VECTOR_NODE, vv);
            push_back_instruction(test_string); 
        }

    public:
        void push_back_instruction(std::string instruction) {m_instruction_list.push_back(instruction);}

};

int genCode(node *ast)
{
    codeGenVisitor code_visitor;
    ast->visit(code_visitor);

}
