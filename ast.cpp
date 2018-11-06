#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0


void* ast = NULL;

void* ast_allocate(NodeKind type, ...){

    va_list args;

    void *ret_node = NULL;
    va_start(args, type);
    switch(type) {
        case SCOPE_NODE: {
            Scope* scope = new Scope();
            scope->d = reinterpret_cast<Declarations*>(va_arg(args, void*));

            ret_node = reinterpret_cast<void*>(scope);
            break;
        }

        case DECLARATIONS_NODE:{
            Declarations *declarations = new Declarations();
            declarations->declarations = reinterpret_cast<Declarations*>(va_arg(args, void*));
            declarations->declaration =  reinterpret_cast<Declaration*>(va_arg(args, void*));

            ret_node = reinterpret_cast<void*>(declarations);
            break;
        }

        case DECLARATION_NODE: {
            Declaration *declaration = new Declaration();
            declaration->i = reinterpret_cast<Identifier*>(va_arg(args, void*));
            declaration->t = reinterpret_cast<Type*>(va_arg(args, void*));

            ret_node = reinterpret_cast<void*>(declaration);
            break;
        }
        case IDENT_NODE: {
            string val = static_cast<string>(va_arg(args, char*));
            ret_node = reinterpret_cast<void*>(new Identifier(val));
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
            ret_node = reinterpret_cast<void*>(new Type(type));
            break;
        }
        default:
            ;

    }

    va_end(args);
    return ret_node;

}


void ast_print(void* root){
    Node *casted_class = reinterpret_cast<Node*>(root);
    Visitor visitor;
    casted_class->visit(visitor);
}

