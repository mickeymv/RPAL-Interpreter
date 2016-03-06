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
const string KEYWORD_TOKEN = "KEYWORD";
const string UNDEFINED_TOKEN = "UNDEFINED";

const char operatorArray[] = {'+', '-', '*', '<', '>', '&', '.', '@', '/', ':', '=', '~', '|', '$', '!', '#', '!', '^',
                              '_', '[', ']', '{', '}', '"', '`', '?', '\0'};

const char stringAllowedCharArray[] = {'(', ')', ';', ',', ' ', '\0'};

const char stringAllowedEscapeCharArray[] = {'t', 'n', '\\', '\'', '\0'};

const char eolCharArray[] = {'\r', '\n', '\0'};

const char punctuationArray[] = {'(', ')', ';', ',', '\0'};

string nextTokenType = UNDEFINED_TOKEN;

bool checkIfEOF(ifstream &file) {
    if (!file.good() || file.peek() == EOF) {
        return true;
    }
    return false;
}


void readIdentifierToken(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without complete parsing! Will die now!!\n\n";
        exit(0);
    }
    nextTokenType = IDENTIFIER_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    while (isalpha(peek) || isnumber(peek) || peek == '_') {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
    if (NT.compare("rec") == 0 || NT.compare("where") == 0 || NT.compare("in") == 0 || NT.compare("and") == 0 ||
        NT.compare("let") == 0 || NT.compare("fn") == 0 || NT.compare("or") == 0 || NT.compare("not") == 0 ||
        NT.compare("gr") == 0 || NT.compare("ge") == 0 || NT.compare("ls") == 0 || NT.compare("le") == 0 ||
        NT.compare("eq") == 0 || NT.compare("ne") == 0 || NT.compare("within") == 0) {
        nextTokenType = KEYWORD_TOKEN;
    }
}

void readIntegerToken(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without complete parsing! Will die now!!\n\n";
        exit(0);
    }
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
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without complete parsing! Will die now!!\n\n";
        exit(0);
    }
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
        NT += x; //Add the escape backslash to the string token (as per the reference implementation)
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
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without complete parsing! Will die now!!\n\n";
        exit(0);
    }
    nextTokenType = STRING_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this

    if (peek == '\'') { //check for the single quote to start the string
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
    if (peek == '\'') { //check for the single quote to close the string
        file.get(x);
        //NT += x; //No need to add the quotes to the string token
    } else {
        cout << "\n\nERROR! Expected close of string, but " << peek << " happened! DIE!\n\n";
        throw std::exception();
    }
}

void readOperatorToken(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without complete parsing! Will die now!!\n\n";
        exit(0);
    }
    nextTokenType = OPERATOR_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    while (isOperator(peek)) {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
}

void scan(ifstream &file);

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
        scan(file); //call scan to get the next token
    } else {
        // this means it's an operator sequence
        NT += '/'; //Add the first '/' that we moved past to the operator token
        readOperatorToken(file);
    }
}

void E(ifstream &file);

void D(ifstream &file);

void readToken(ifstream &file, string token);

void Vl(ifstream &file) {
    readToken(file, IDENTIFIER_TOKEN);
    if (NT.compare(",") == 0) {
        readToken(file, ",");
        Vl(file);
    }
}

void Vb(ifstream &file) {
    if (NT.compare("(") == 0) {
        readToken(file, "(");
        if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0) {
            Vl(file);
        }
        readToken(file, ")");
    } else if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0) {
        readToken(file, IDENTIFIER_TOKEN);
    }
}

void Db(ifstream &file) {
    if (NT.compare("(") == 0) {
        readToken(file, "(");
        D(file);
        readToken(file, ")");
    } else if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0) {
        readToken(file, IDENTIFIER_TOKEN);
        if (NT.compare(",") == 0) {
            readToken(file, ",");
            Vl(file);
            readToken(file, "=");
            E(file);
        } else {
            while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || NT.compare("(") == 0) {
                Vb(file);
            }
            readToken(file, "=");
            E(file);
        }
    }
}

void Dr(ifstream &file) {
    if (NT.compare("rec") == 0) {
        readToken(file, "rec");
    }
    Db(file);
}

void Da(ifstream &file) {
    Dr(file);
    while (NT.compare("and") == 0) {
        readToken(file, "and");
        Dr(file);
    }
}

void D(ifstream &file) {
    Da(file);
    if (NT.compare("within") == 0) {
        readToken(file, "within");
        D(file);
    }
}

void Rn(ifstream &file) {
    if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || nextTokenType.compare(INTEGER_TOKEN) == 0 ||
        nextTokenType.compare(STRING_TOKEN) == 0) {
        readToken(file, nextTokenType);
    } else if (NT.compare("true") == 0 || NT.compare("false") == 0 ||
               NT.compare("nil") == 0 || NT.compare("dummy") == 0) {
        readToken(file, NT);
    } else if (NT.compare("(") == 0) {
        readToken(file, "(");
        E(file);
        readToken(file, ")");
    }
}

void R(ifstream &file) {
    Rn(file);
    while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || nextTokenType.compare(INTEGER_TOKEN) == 0 ||
           nextTokenType.compare(STRING_TOKEN) == 0 || NT.compare("true") == 0 || NT.compare("false") == 0 ||
           NT.compare("nil") == 0 || NT.compare("dummy") == 0 || NT.compare("(") == 0) {
        Rn(file);
    }
}

void Ap(ifstream &file) {
    R(file);
    while (NT.compare("@") ==
           0) { //TODO: this might cause an error. How would '@' be parsed if it's a prefix to a fn name?
        readToken(file, "@");
        readToken(file, IDENTIFIER_TOKEN);
        R(file);
    }
}

void Af(ifstream &file) {
    Ap(file);
    if (NT.compare("**") == 0) {
        readToken(file, "**");
        Af(file);
    }
}

void At(ifstream &file) {
    Af(file);
    while (NT.compare("*") == 0 || NT.compare("/") == 0) {
        readToken(file, NT);
        Af(file);
    }
}

void A(ifstream &file) {
    if (NT.compare("+") == 0) {
        readToken(file, "+");
    } else if (NT.compare("-") == 0) {
        readToken(file, "-");
    }
    At(file);
    while (NT.compare("+") == 0 || NT.compare("-") == 0) {
        readToken(file, NT);
        At(file);
    }
}

void Bp(ifstream &file) {
    A(file);
    if (NT.compare("gr") == 0 || NT.compare(">") == 0) {
        readToken(file, NT);
        A(file);
    } else if (NT.compare("ge") == 0 || NT.compare(">=") == 0) {
        readToken(file, NT);
        A(file);
    } else if (NT.compare("ls") == 0 || NT.compare("<") == 0) {
        readToken(file, NT);
        A(file);
    } else if (NT.compare("le") == 0 || NT.compare("<=") == 0) {
        readToken(file, NT);
        A(file);
    } else if (NT.compare("eq") == 0) {
        readToken(file, "eq");
        A(file);
    } else if (NT.compare("ne") == 0) {
        readToken(file, "ne");
        A(file);
    }
}

void Bs(ifstream &file) {
    if (NT.compare("not") == 0) {
        readToken(file, "not");
    }
    Bp(file);
}

void Bt(ifstream &file) {
    Bs(file);
    while (NT.compare("&") == 0) {
        readToken(file, "&");
        Bs(file);
    }
}

void B(ifstream &file) {
    Bt(file);
    while (NT.compare("or") == 0) {
        readToken(file, "or");
        Bt(file);
    }
}

void Tc(ifstream &file) {
    B(file);
    if (NT.compare("->") ==
        0) {  //TODO: possible place of an error when '->' is expected and evaluated but it was actually a '-'? Is that possible?
        readToken(file, "->");
        Tc(file);
        readToken(file, "|");
        Tc(file);
    }
}

void Ta(ifstream &file) {
    Tc(file);
    while (NT.compare("aug") == 0) { //left recursion
        readToken(file, "aug");
        Tc(file);
    }
}

void T(ifstream &file) {
    Ta(file);
    while (NT.compare(",") == 0) { //combo of left recursion AND common prefix
        readToken(file, ",");
        Ta(file);
    }
}

void Ew(ifstream &file) {
    T(file);
    if (NT.compare("where") == 0) { //common prefix
        readToken(file, "where");
        Dr(file);
    }
}

void E(ifstream &file) {
    int N = 0;
    if (NT.compare("let") == 0) {
        readToken(file, "let");
        D(file);
        readToken(file, "in");
        E(file);
    } else if (NT.compare("fn") == 0) { //TODO:  || nextTokenType.compare(IDENTIFIER_TOKEN) == 0 // Is this required?
        //TODO: It should be NT.compare(fn) alone ... How to do this properly? How to resolve "fn" with a custom user defined function name?
        readToken(file, "fn");
        do {
            Vb(file);
            N++;
        } while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || NT.compare("(") == 0);
        readToken(file, ".");
        E(file);
    } else {
        Ew(file);
    }

}

void scan(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached !\n\n";
        return;
    }
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
        scan(file); //call scan to get the next token
    } else if (isPunctuation(peek)) {
        readPunctuationChar(file);
    }
//    } else {
//        file.get(); //TODO: Is this required?
//        if (checkIfEOF(file)) {
//            cout << "\n\nEOF reached !!\n\n";
//            exit(0);
//        }
//        cout << "\n\nERROR! Found unexpected char, '" << peek << "' happened! DIE!\n\n";
//        throw std::exception();
//    }

}

void readToken(ifstream &file, string token) {
    //if (token.compare("fn") == 0 && nextTokenType.compare(IDENTIFIER_TOKEN) ==  //TODO: Is this required?
    //                                0); //TODO: because the function name could be any identifier? Or is it?
    //else
    if (token.compare(NT) != 0 && token.compare(nextTokenType) != 0) {
        cout << "\n\nError! Expected '" << token << "' , but found '" << NT << "' !\n\n";
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
                scan(the_file); //Prepare the first token by placing it within 'NT'
                E(the_file);    //Call the first non-terminal procedure to start parsing
                //cout << " " << NT;
                if (checkIfEOF(the_file)) {
                    cout << "\n\nEOF successfully reached after complete parsing! Will exit now!!\n\n";
                    exit(1);
                } else {
                    cout << "\n\nSomething went wrong! EOF not reached but parsing complete! Will exit now!!\n\n";
                    exit(0);
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


