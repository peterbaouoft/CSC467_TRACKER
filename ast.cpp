#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0


Node* ast_allocate(NodeKind type, ...){

    va_list args;

    Node *ret_node = NULL;
    switch(type) {
        case SCOPE_NODE: {
            Scope* scope = new Scope();
            scope->d = va_arg(args, Declarations*);

            ret_node = scope;
            break;
        }

        case DECLARATIONS_NODE:{
            Declarations *declarations = new Declarations();
            declarations->declarations = va_arg(args, Declarations*);
            declarations->declaration =  va_arg(args, Declaration*);

            ret_node = declarations;
            break;
        }

        case DECLARATION_NODE: {
            Declaration *declaration = new Declaration();
            declaration->i = va_arg(args, Identifier*);
            declaration->t = va_arg(args, Type*);
            // char* val = va_arg(args, char*);
            // if (val != NULL)
                // declaration->initial_val =  static_cast<string>(val);

            ret_node = declaration;
            break;
        }
        case IDENT_NODE: {
            string val = static_cast<string>(va_arg(args, char*));
            ret_node = new Identifier(val);
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
            string type = type_list[type_index][dimen_index];

            ret_node = new Type(type);
            break;
        }
        default:
            ;
    }


    return NULL;

}


void ast_print(Node* root){


}
