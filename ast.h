#ifndef AST_H_
#define AST_H_ 1
#include <stdlib.h>
#include <string>
#include <vector>
#include <cassert>
#include <ostream>

/**************************************************************************
 *                              FORWARD DECLARATIONS                      *
 *                                                                        *
 *************************************************************************/
class PrintVisitor;
class NodeLocation;
class Scope;
class Declaration;
class Declarations;
class Type;
class Node;

/*----------------------------------Statement related--------------------*/
class Statement;
class Statements;
class AssignStatement;
class IfStatement;
class NestedScope;
class EmptyStatement;

/*--------------------------------Expression related---------------------*/
class Expression;
class ConstructorExpression;
class FloatLiteralExpression;
class IntLiteralExpression;
class BoolLiteralExpression;
class Constructor;
class UnaryExpression;
class BinaryExpression;
class VariableExpression;
class FunctionExpression;

class Function;
class IdentifierNode;
class VectorVariable;
class Arguments;

typedef Node node;

extern node *ast;

typedef enum
{
    UNKNOWN = 0,

    SCOPE_NODE = (1 << 0),

    DECLARATIONS_NODE = (1 << 15),
    DECLARATION_NODE = (1 << 16),
    TYPE_NODE = (1 << 2) | (1 << 8),

    STATEMENTS_NODE = (1 << 17),
    IF_STATEMENT_NODE = (1 << 1) | (1 << 11),
    ASSIGNMENT_NODE = (1 << 1) | (1 << 13),
    NESTED_SCOPE_NODE = (1 << 1) | (1 << 14),
    EMPTY_STATEMENT_NODE = (1 << 2) | (1 << 7),

    EXPRESSION_NODE = (1 << 2),
    UNARY_EXPRESSION_NODE = (1 << 2) | (1 << 3),
    BINARY_EXPRESSION_NODE = (1 << 2) | (1 << 4),
    VECTOR_NODE = (1 << 18),
    FUNCTION_NODE = (1 << 2) | (1 << 9),
    CONSTRUCTOR_NODE = (1 << 2) | (1 << 10),
    ARGUMENTS_NODE = (1 << 2) | (1 << 5),
    IDENTIFIER_NODE = (1 << 2) | (1 << 6),

    CONST_DECLARATION_NODE = (1 << 2) | (1 << 19),

} NodeKind;

typedef enum
{
    BOOL_EXPRESSION,
    INT_LITERAL,
    FLOAT_LITERAL,
    CONSTRUCTOR_EXPRESSION,
    VARIABLE,
    FUNCTION,
    TEMP_ID_EXPRESSION,
    TEMP_VECTOR_EXPRESSION,
} ExpressionType;

typedef enum
{
    MUL_INSTURCTION,
    MOV_INSTRUCTION,
    TEMP_INSTRUCTION,
    DP3_INSTRUCTION,
    RSQ_INSTRUCTION,
    LIT_INSTRUCTION,
    CONST_REGISTER,
} AssemblyInstructionType;

class Visitor
{
  public:
    virtual void visit(Scope *scope);
    virtual void visit(Declaration *decl);
    virtual void visit (Declarations *decls);
    virtual void visit(Type *type);

    virtual void visit(Statement *stmt);
    virtual void visit(Statements *stmts);
    virtual void visit(AssignStatement *as_stmt);
    virtual void visit(IfStatement *if_statement);
    virtual void visit(NestedScope *ns);
    virtual void visit(EmptyStatement *es);

    virtual void visit(ConstructorExpression *ce);
    virtual void visit(FloatLiteralExpression *fle);
    virtual void visit(BoolLiteralExpression *ble);
    virtual void visit(IntLiteralExpression *ile);
    virtual void visit(UnaryExpression *ue);
    virtual void visit(BinaryExpression *be);
    virtual void visit(VariableExpression *ve);
    virtual void visit(FunctionExpression *fe);

    virtual void visit(Function *f);
    virtual void visit(Constructor *c);
    virtual void visit(Arguments *args);
    virtual void visit(IdentifierNode *var);
    virtual void visit(VectorVariable *vec_var);
};

class PrintVisitor : public Visitor {
  public:
    virtual void visit(Scope *scope);
    virtual void visit(Declaration *decl);
    virtual void visit(Declarations *decls);
    virtual void visit(Type *type);

    virtual void visit(Statement *stmt);
    virtual void visit(Statements *stmts);
    virtual void visit(AssignStatement *as_stmt);
    virtual void visit(IfStatement *if_statement);
    virtual void visit(NestedScope *ns);
    virtual void visit(EmptyStatement *es);

    virtual void visit(ConstructorExpression *ce);
    virtual void visit(FloatLiteralExpression *fle);
    virtual void visit(BoolLiteralExpression *ble);
    virtual void visit(IntLiteralExpression *ile);
    virtual void visit(UnaryExpression *ue);
    virtual void visit(BinaryExpression *be);
    virtual void visit(VariableExpression *ve);
    virtual void visit(FunctionExpression *fe);

    virtual void visit(Function *f);
    virtual void visit(Constructor *c);
    virtual void visit(Arguments *args);
    virtual void visit(IdentifierNode *var);
    virtual void visit(VectorVariable *vec_var);
};

class NodeLocation
{
  private:
    int m_first_line = 0;
    int m_last_line  = 0;
    int m_first_col = 0;
    int m_last_col = 0;
  public:
    NodeLocation(int first_line, int last_line, int first_col, int last_col) :
      m_first_line(first_line), m_last_line(last_line), m_first_col(first_col), m_last_col(last_col) {}

    int get_first_line() const {return m_first_line;}
    int get_last_line() const {return m_last_line;}
    int get_first_col() const {return m_first_col;}
    int get_last_col() const {return m_last_col;}

};

inline std::ostream& operator<<(std::ostream &out, NodeLocation const& data) {
      out << "at line ";
      out << data.get_first_line() << ':';
      out << data.get_first_col();
      out << " to " << data.get_last_line() << ':';
      out << data.get_last_col();

      return out;
}

class Node
{
  private:
    NodeLocation *m_node_location_in_file = nullptr;
  public:
    virtual void visit(Visitor &vistor) = 0;
    virtual ~Node() {delete m_node_location_in_file;}

  public: /* The fields below are used to track the location for different nodes */
    NodeLocation *get_node_location () const {return m_node_location_in_file;}
    void set_node_location (NodeLocation *node_location) {m_node_location_in_file = node_location;}
};

class Type : public Node
{
  public:
    std::string type_name;

    Type(const std::string &type)
    {
        this->type_name = type;
    }
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    }
};

class Expression : public Node
{
  private:
    std::string type = "ANY_TYPE";
    bool m_is_const = false;
    std::string result_register_name = "";
  public:
    virtual std::string get_expression_type() const {return type;}
    virtual void set_expression_type(std::string type_str) {type = type_str;}
    virtual bool get_is_const() const { return m_is_const; }
    void set_is_const( bool is_const) { m_is_const = is_const;}

    virtual std::string get_result_register_name() const {return result_register_name;}
    virtual void set_result_register_name(std::string register_name) {result_register_name = register_name;}

  public:
    virtual ~Expression() {}
};

class Declaration : public Node
{
  private:
    bool is_const = false;
    bool is_read_only = false;
    bool is_write_only = false;
  public:
    Type *type = nullptr;
    std::string id;
    Expression *initial_val = nullptr;

  public:
    Declaration(Type *type, const std::string &id, Expression *init_val, bool is_const)
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

    bool get_is_const() const {return is_const;}
    void set_is_const(const bool &const_val) { is_const = const_val;}
    bool get_is_read_only() const {return is_read_only;}
    void set_is_read_only(const bool &read_only_val) { is_read_only = read_only_val;}
    bool get_is_write_only() const {return is_write_only;}
    void set_is_write_only(const bool &write_only_val) { is_write_only = write_only_val;}

  public: /* Place to put destructor function calls */
    ~Declaration() {
        delete type;
        delete initial_val;
    }
};

class Declarations : public Node
{
  public:
    std::vector<Declaration *> declaration_list;

  public:
    virtual void push_back_declaration(Declaration *decl) { declaration_list.push_back(decl); }
    virtual void push_front_declaration(Declaration *decl) { declaration_list.insert(declaration_list.begin(), decl);}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    }
  public:
    ~Declarations() {
        for (Declaration *decl : declaration_list)
            delete decl; /* We don't have nullptr in the list, so safe to loop all of them :) */
    }
};

class Statement : public Node
{
  public:
    virtual void visit(Visitor &visitor) = 0;

  public:
    virtual ~Statement() {}
};

class Statements : public Node
{
  private:
    std::vector<Statement *> statement_list;
  public:

    const std::vector<Statement *> &get_statement_list() const {return statement_list;}
    virtual void push_back_statement(Statement *stmt) { statement_list.push_back(stmt); }
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
  public:
    ~Statements() {
        for (Statement *statement : statement_list)
            delete statement;
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
  public:
    ~Scope() {
        delete declarations;
        delete statements;
    }
};

/**********************************************Identifier Classes*********************************************************/
class IdentifierNode : public Node
{
  private:
    Declaration *declaration = nullptr;

  public:
    virtual Type *get_id_type() const {return declaration ? declaration->type : NULL;}
    void set_declaration(Declaration *decl) {declaration = decl;}
    virtual Declaration *get_declaration() const {return declaration;}
    virtual void set_id_type(Type *type) {}

  public:
    std::string id;

    IdentifierNode(const std::string &identifier) : id(identifier) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
  public:
    virtual ~IdentifierNode () {} /* We don't delete declaration, because they are handled in the earlier stages, but have to say.. this might
                                   be a design flaw. It is a virtual destructor due to inheritance */
};

class VectorVariable : public IdentifierNode
{
  private:
    Type *type = nullptr;
  public:
    int vector_index;

    VectorVariable(std::string id_node, int v_index) :
        IdentifierNode(id_node), vector_index(v_index) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };

    Type *get_id_type() const{
        return type ?: IdentifierNode::get_id_type();
    }

    void set_id_type(Type *t) {
        type = t;
    }
  public:
    ~VectorVariable() { if (type != nullptr) delete type;}
};
/**********************************************END Identifier Classes*********************************************************/

/*=========================Beginning of STATEMENT class===================================*/
class AssignStatement : public Statement
{
  public:
    IdentifierNode *variable = nullptr;
    Expression *expression = nullptr;
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
   public:
    ~AssignStatement() {
        delete variable;
        delete expression;
    }
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
  public:
    ~IfStatement() {
        delete expression;
        delete statement;
        if (else_statement) delete else_statement;
    }
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
  public:
    ~NestedScope() {delete scope;}
};

class EmptyStatement: public Statement
{
  public:
    virtual void visit(Visitor &visitor) {
        visitor.visit(this);
    }
  public:
    ~EmptyStatement() {}
};

/*================END OF STATEMENT CLASS================*/

/*****************************************Consturctors and Functions Definitions**********************/
class Arguments : public Node
{
  private:
    std::vector<Expression *> m_expression_list;
  public:

    const std::vector<Expression *> &get_expression_list() const { return m_expression_list; }

    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };

    virtual void push_back_expression(Expression *expression) {m_expression_list.push_back(expression);}

  public:
    ~Arguments() {
        for (Expression *expression : m_expression_list)
            delete expression;
    }
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
  public:
    ~Constructor() {
        delete type;
        delete args;
    }
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
  public:
    ~ConstructorExpression() {delete constructor;}
};

class Function : public Node
{
  public:
    std::string function_name;
    Arguments *arguments;
    Function(std::string func_name, Arguments *args) : function_name(func_name), arguments(args) {}

    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
  public:
    ~Function() {delete arguments;}
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
  public:
    ~FunctionExpression() {delete function; }
};
/*****************************************END Consturctors and Functions Definitions**********************/

class VariableExpression : public Expression
{
  public:
    IdentifierNode *id_node;

    VariableExpression(IdentifierNode *id) : id_node(id) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };

  public:
    ~VariableExpression() {delete id_node;}
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
    virtual bool get_is_const () const {return true; }

    virtual std::string get_result_register_name() const {return std::to_string(int_literal);}
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

    virtual bool get_is_const() const {return true;}

    virtual std::string get_result_register_name() const {// Interesting, does this need a name to store it?
                                                          return std::to_string(bool_literal);}
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

    virtual bool get_is_const() const {return true;}
    virtual std::string get_result_register_name() const {return std::to_string(float_literal);}
};

class UnaryExpression : public Expression
{
  public:
    int operator_type;
    Expression *right_expression;

  public:

    UnaryExpression(int op, Expression *rhs_expression) : operator_type(op), right_expression(rhs_expression) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };

  public:
    ~UnaryExpression() {delete right_expression;}
};

class BinaryExpression : public UnaryExpression
{
  public:
    Expression *left_expression;

  public:

    BinaryExpression(int op, Expression *rhs_expression, Expression *lhs_expression) :
        UnaryExpression(op, rhs_expression), left_expression(lhs_expression) {}
    virtual void visit(Visitor &visitor)
    {
        visitor.visit(this);
    };
  public:
    ~BinaryExpression() {delete left_expression;}
};


/* Use of Expression visitor to get the type of the expression */
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

        virtual void visit(IdentifierNode *var) {set_expression_instance_type(TEMP_ID_EXPRESSION);}
        virtual void visit(VectorVariable *vec_var) {set_expression_instance_type(TEMP_VECTOR_EXPRESSION);}

};


node *ast_allocate(NodeKind type, ...);
void ast_print(node *ast_root);
void ast_free(node *ast_root);

int get_type_dimension (const std::string &type);


std::string get_base_type (const std::string type);


#endif /* AST_H_ */
