#include <iostream>
#include <fstream>

/**
 * Author: Mickey Vellukunnel
 * UFID: 9413-9616
 */

using namespace std;

int main(int argc, char *argv[]) {

    if (argc == 3) {
        if (argv[2][0] == '-') {
            cout << "\n\nUsages:\n" << argv[0] << " <filename>\n";
            cout << argv[0] << " -ast <filename>\n";
            cout << argv[0] << " -l <filename>\n\n";
        }
        else {
            // We assume argv[2] is a filename to open
            ifstream the_file(argv[2]);
            // Always check to see if file opening succeeded
            if (!the_file.is_open())
                cout << "\n\nCould not open file: '" << argv[2] << "'" << "\n\n";
            else {
                if (strcmp(argv[1], "-l") == 0) { //Print input? (//Listing of input?)
                    cout << "\n\n";
                    char x;
                    // the_file.get ( x ) returns false if the end of the file
                    //  is reached or an error occurs
                    while (the_file.get(x))
                        cout << x;

                    // the_file is closed implicitly here
                    cout << "\n\n";
                } else if (strcmp(argv[1], "-ast") == 0) {
                    //Call ast generator
                    cout << "\n\nAST Tree should print!\n\n";
                }
            }
        }
    }


    else if (argc == 2) {
        if (argv[1][0] == '-') {
            cout << "\n\nUsages:\n" << argv[0] << " <filename>\n";
            cout << argv[0] << " -ast <filename>\n";
            cout << argv[0] << " -l <filename>\n\n";
        }
        else {
            // We assume argv[2] is a filename to open
            ifstream the_file(argv[1]);
            // Always check to see if file opening succeeded
            if (!the_file.is_open())
                cout << "\n\nCould not open file: '" << argv[1] << "'" << "\n\n";
            else {
                //Call parser
                cout << "\n\nShould lexically analyze by recursively descending!!\n\n";
            }
        }
    }
    else if (argc != 2) {
        /*
         * p1 (without switches) should produce no output whatsoever.
            Required switches: -ast This switch prints the abstract syntax tree, and nothing else. No headers or footers.
         The AST must match exactly, character for character, the AST produced by rpal.

            Optional switches: -l This produces a listing of the input.
         */
        cout << "\n\nUsages:\n" << argv[0] << " <filename>\n";
        cout << argv[0] << " -ast <filename>\n";
        cout << argv[0] << " -l <filename>\n\n";
    }

}