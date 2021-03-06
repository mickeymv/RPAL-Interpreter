# RPAL-Interpreter
The objective of this second project is to implement the sub-tree (partial) standardizer, and the 13-rule CSE machine. The specifications are in the class notes, and have been discussed in detail in class. You can simply extend your previous project. Make sure your previous project works on all the test cases!

Regarding grading and submission

Put in a BUGS file all comments of known errors (if any) of your project. It is MUCH better for you to list the limitations of your software than to let us discover them. The more time it takes for us to find what's wrong with your program, the larger the portion of credit you lose. In short, you must make it easy for us to examine your program.
Your output should match the corresponding output for each one of the test programs.
Grading strategy: To grade your project, we will run your program on each of the test programs, and we will compare your output with the correct one. Full credit will be given a perfect match in every case.
Executable Name: p2. Include a make file in your submission.
You must provide a makefile to build this project. We will compile your program by invoking: make

we will then test p2 by invoking it as follows:

p2 testprog1

p2 testprog2

..

Notice that the input is NOT standard input (stdin), but instead a command-line parameter.

Examples:

p2 -l /cis/homes/manuel/systems/pal/tests.rpal/trees

p2 -ast /cis/homes/manuel/systems/pal/tests.rpal/Treepicture

p2 (without switches) should produce the result of the program.
Required switches: -ast This switch prints the abstract syntax tree, and nothing else. No headers or footers. The AST must match exactly, character for character, the AST produced by rpal.

Optional switches: -l This produces a listing of the input.

It must be possible to run p2 from ANY directory: ~user/cop5556/p2 testfile
It must run correctly on storm.cise.ufl.edu. You can ssh into storm and test your project there. If you have questions about storm or your cise account, refer to this website: http://www.cise.ufl.edu/help/
Note: you cannot require any changes to the test files for your program to run. The files will be used as is. In the past, students have tried to specify, things like "no blank lines" or "every token must be separated by spaces". This represents a change to the rpal specifications and so is not acceptable. If your program does not correctly parse the given test files as is, it doesn't work.
TESTING YOUR PROJECT
This file, "difftest.pl" will be used to test your project. 

The input will be something like: 
./difftest.pl -1 "./rpal -ast FILE" -2 "./p2 -ast FILE" -t ~/rpal/tests/
