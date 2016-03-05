#include <iostream>
#include <fstream>

/**
 * Author: Mickey Vellukunnel
 * UFID: 9413-9616
 */

using namespace std;

string NT; //NextToken
const string IDENTIFIER_TOKEN = "IDENTIFIER";
const string INTEGER_TOKEN = "INTEGER";
const string STRING_TOKEN = "STRING";
const string OPERATOR_TOKEN = "OPERATOR";
const string UNDEFINED_TOKEN = "UNDEFINED";

const char operatorArray[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '!', '^',
                              '_', '[', ']', '{', '}', '"', '`', '?', '\0'};

const char stringAllowedCharArray[] = {'(', ')', ';', ',', ' ', '\0'};

string nextTokenType = UNDEFINED_TOKEN;

void checkIfEOF(ifstream &file) {
    if (!file.good()) {
        cout << "\n\nEOF reached!\n\n";
        exit(1);
    }
}


void readIdentifierToken(ifstream &file) {
    checkIfEOF(file);
    nextTokenType = IDENTIFIER_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    while (isalpha(peek) || isnumber(peek) || peek == '_') {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
}

void readIntegerToken(ifstream &file) {
    checkIfEOF(file);
    nextTokenType = INTEGER_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    while (isnumber(peek)) {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
}

bool isOperator(char c) {
    if (std::find(std::begin(operatorArray), std::end(operatorArray), c) != std::end(operatorArray))
        return true;
    else
        return false;
}

bool isStringAllowedChar(char c) {
    if ((std::find(std::begin(stringAllowedCharArray), std::end(stringAllowedCharArray), c) !=
         std::end(stringAllowedCharArray)) || isnumber(c) || isalpha(c) || isOperator(c))
        return true;
    else
        return false;
}

void readStringToken(ifstream &file) {
    checkIfEOF(file);
    nextTokenType = STRING_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this

    if (peek != '\'') { //check for the single quote to start the string
        file.get(x);
        NT += x;
        peek = file.peek();
    } else {
        cout << "\n\nERROR! Expected start of string, " << peek << " happened! DIE!\n\n";
        throw std::exception();
    }
    while (isStringAllowedChar(peek)) {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
    if (peek != '\'') { //check for the single quote to close the string
        file.get(x);
        NT += x;
    } else {
        cout << "\n\nERROR! Expected close of string, " << peek << " happened! DIE!\n\n";
        throw std::exception();
    }
}

void readOperatorToken(ifstream &file) {
    checkIfEOF(file);
    nextTokenType = OPERATOR_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    while (isOperator(peek)) {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
}

void scan(ifstream &file) {
    checkIfEOF(file);
    nextTokenType = UNDEFINED_TOKEN;

    char peek = file.peek(); //peek and store the next character in stream in this

    NT.clear(); //clear NT to get the next token in file

    if (isalpha(peek)) {
        readIdentifierToken(file);
    } else if (isnumber(peek)) {
        readIntegerToken(file);
    } else if (isOperator(peek)) {
        readOperatorToken(file);
    }

}

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
                string nextToken;
                while (1) {
                    scan(the_file);
                    cout << " " << NT;
                }
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

void read(ifstream &file, string token) {
    if (token.compare(NT) != 0 && token.compare(nextTokenType) != 0) {
        cout << "\n\nError! Expected " << token << " but found " << NT << "\n\n";
        throw std::exception();
    }
    scan(file);
}

