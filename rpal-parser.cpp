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
//const string PUNCTUATION_TOKEN = "PUNCTUATION";
const string UNDEFINED_TOKEN = "UNDEFINED";

const char operatorArray[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '!', '^',
                              '_', '[', ']', '{', '}', '"', '`', '?', '\0'};

const char stringAllowedCharArray[] = {'(', ')', ';', ',', ' ', '\0'};

const char stringAllowedEscapeCharArray[] = {'t', 'n', '\\', '\'', '\0'};

const char eolCharArray[] = {'\r', '\n', '\0'};

const char punctuationArray[] = {'(', ')', ';', ',', '\0'};

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

bool isPunctuation(char c) {
    if (std::find(std::begin(punctuationArray), std::end(punctuationArray), c) != std::end(punctuationArray))
        return true;
    else
        return false;
}

void readPunctuationChar(ifstream &file) {
    checkIfEOF(file);
    //nextTokenType = PUNCTUATION_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    if (isPunctuation(peek)) {
        file.get(x);
        NT += x;
    }
}

bool isStringAllowedChar(char c) {
    if ((std::find(std::begin(stringAllowedCharArray), std::end(stringAllowedCharArray), c) !=
         std::end(stringAllowedCharArray)) || isnumber(c) || isalpha(c) || isOperator(c))
        return true;
    else
        return false;
}

bool isEscapeCharInString(ifstream &file, char &peek) {
    char x; //get the next character in stream in this
    //peek and store the next character in stream in this
    if (peek == '\\') {
        file.get(x);
        //NT += x; //No need to add the escape backslash to the string token
        peek = file.peek();
        if (std::find(std::begin(stringAllowedEscapeCharArray), std::end(stringAllowedEscapeCharArray), peek) !=
            std::end(stringAllowedEscapeCharArray)) {
            file.get(x);
            NT += x;
            peek = file.peek();
            return true;
        }
        else {
            cout << "\n\nERROR! Expected an escape character, but " << peek << " happened! DIE!\n\n";
            throw std::exception();
        }
    }
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
        //NT += x; //No need to add the quotes to the string token
        peek = file.peek();
    } else {
        cout << "\n\nERROR! Expected start of string, but " << peek << " happened! DIE!\n\n";
        throw std::exception();
    }
    while (isStringAllowedChar(peek) || (isEscapeCharInString(file, peek) && isStringAllowedChar(peek))) {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
    if (peek != '\'') { //check for the single quote to close the string
        file.get(x);
        //NT += x; //No need to add the quotes to the string token
    } else {
        cout << "\n\nERROR! Expected close of string, but " << peek << " happened! DIE!\n\n";
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

void resolveIfCommentOrOperator(ifstream &file) {
    char x;
    file.get(x); //Move past the first '/'
    char peek = file.peek();
    if (peek == '/') {
        //This means it's a comment line, so keep reading/updating file stream pointer without "tokenizing" (adding to NT) until an eol.
        while (!(std::find(std::begin(eolCharArray), std::end(eolCharArray), peek) !=
                 std::end(eolCharArray))) {
            file.get(x); //move past the whitespaces until an eol
            peek = file.peek();
        }
        file.get(x); //Move past the EOL
    } else {
        // this means it's an operator sequence
        NT += '/'; //Add the first '/' that we moved past to the operator token
        readOperatorToken(file);
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
    } else if (peek == '/') {
        resolveIfCommentOrOperator(file);
    } else if (isOperator(peek)) {
        readOperatorToken(file);
    } else if (peek == '\'') { //Start of a string
        readStringToken(file);
    } else if (iswspace(peek)) { //ignore whiteSpace chars (space, horTab, newLine, carrReturn, verTab)
        char x;
        file.get(x); //further the file pointer and ignore the whitespace character (no need to tokenize it)
        //NT += x;
    } else if (isPunctuation(peek)) {
        readPunctuationChar(file);
    } else {
        file.get(); //TODO: Is this required?
        checkIfEOF(file);
        cout << "\n\nERROR! Found unexpected char, '" << peek << "' happened! DIE!\n\n";
        throw std::exception();
    }

}

void read(ifstream &file, string token) {
    if (token.compare(NT) != 0 && token.compare(nextTokenType) != 0) {
        cout << "\n\nError! Expected " << token << " but found " << NT << "\n\n";
        throw std::exception();
    }
    scan(file);
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

