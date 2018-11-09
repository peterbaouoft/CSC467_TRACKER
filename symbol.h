#ifndef SYMBOL_H_
#define SYMBOL_H_ 1

#include <string.h>
#include "ast.h"
#include <forward_list>
#include <unordered_map>


class SymbolTablex {
    private:
        std::forward_list<std::unordered_map<std::string, Declaration*>> symbol_table;

    public:
        Declaration* create_symbol(Declaration* decl){
            // if (symbol_table.empty()) {
            //     symbol_table.emplace_front({{decl->id, decl}}); // Create empty unordered map with one pair
            //     return true;
            // }
            auto &current_map = symbol_table.front();
            if (current_map.count(decl->id) > 0){
                return current_map.find(decl->id)->second;
            }

            current_map.emplace(decl->id, decl);
            return NULL;
        }

        void enter_scope(){
            symbol_table.emplace_front();
        }
        void exit_scope(){
            symbol_table.pop_front();
        }

        Declaration *find_symbol(const std::string &id){
            for (const auto &current_map : symbol_table){
                auto fit = current_map.find(id);
                if (fit != current_map.end())
                    return fit->second;
            }
            return NULL;
        }
};

#endif
