BONUS PORTIONS:

1: Report the line numeber on which the semantic error occurred
2: Report the column of the line, with the line number, on which the error occurred
3: Provide a contextual message that gives extra meaning to the error to help the programmer debug the error


======================================================================================================
1 & 2: Through bison tutorial online, you can define a YY_USER_ACTION to update the row and column 
of your program. Then, the location (which is YYLTYPE `*`), will be passed into the ast_allocate,

For instance:
$$ = ast_allocate(DECLARATION_NODE, $1, $2, NULL, false, &@$);

For the above one, this is one of the allocation for one of the declarations. The last argument &@$, 
is to pass the address of the LHS. This contains information of YYLTYPE, which contains
a: first line b: last line c: first column d: last column of the correspondent rule.


Afterwards, the information for each rule is passed into the class information in ast.h. In error
handling, those information will be accessed



3: One of the contextual message that we have is:
1: Return the declaration of the variable when an error happened. This is done during the construction of
a symbol table, when we reach an id without a declaration, if it is not an error, a declaration (source) will
be put into the id class. As a result, we use that information to output where is the declaration 
