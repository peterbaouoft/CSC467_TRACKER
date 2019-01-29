# Run or Test the program
1: You can run some existing tests from the semantic analysis like the following:
```
make
./compiler467 -Da semantic_test/semantic_checking_test/assignment.c
```
2: You can run the code generation via files in Demos:
```
make
./compiler467 Demos/Demo1/shader.frag
``` 

3: Or you can write your own test files like the ones in Demos. 
The specifications of the shading language can be found at
http://www.dsrg.utoronto.ca/csc467/lab/MiniGLSLSpec.pdf

# Lab 3 Roadmap
1: Semantic Analysis (all of the listed items and its rules)
2: Symbol table construction 
3: The leftover bits from AST (mainly expression node, and remaining visitor functions
for implementation, you can refer to the earlier implementations):
4: Error reporting

# Lab 2 Roadmap
Khoi:
Peter:

Current testing framework (regression tests mainly):
Step 1: Create test case in parser_test/basic_test folder (the naming convention is test_$type.txt)
Step 2: run the command "compiler467 -Tp $file_name"
Step 3: Create a new file with naming convention (test_$type_output.txt), copying the output from step 2 in it
Step 4: cd into the parser_test directory, and run python test.py

The remaining need to be tested:
1: type
2: unary and binary expression
3: operator precedence
4: function name

NOTE: do keep in mind tho, we still need to use our logic to see if the parser output makes sense,
this requires a bit of time
# Lab 1 Roadmap

1: Identifiers, Keywords, Integers, Loops, Comments
    Khoi        Peter

2: yylex understanding (peter) and testing understanding

# Meeting
Tuesday   4-5
Friday    12-1
