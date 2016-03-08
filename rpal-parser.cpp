#include <iostream>
#include <fstream>
#include <stack>

#include <cstdlib>
#include <cstring>

using namespace std;

/**
 * Author: Mickey Vellukunnel
 * UFID: 9413-9616
 */

string NT; //NextToken
const string IDENTIFIER_TOKEN = "IDENTIFIER";
const string INTEGER_TOKEN = "INTEGER";
const string STRING_TOKEN = "STRING";
const string OPERATOR_TOKEN = "OPERATOR";
const string KEYWORD_TOKEN = "KEYWORD";
const string UNDEFINED_TOKEN = "UNDEFINED";
const string PUNCTUATION_TOKEN = "PUNCTUATION";

const char *operatorArray = "+-*<>&.@/:=~|$!#!^_[]{}\"`?";

const char *stringAllowedCharArray = "();, ";

const char *stringAllowedEscapeCharArray = "tn\\\'";

const char *eolCharArray = "\r\n";

const char *punctuationArray = "();,";

string nextTokenType = UNDEFINED_TOKEN;

struct Node {
    string label;
    struct Node *firstKid;
    struct Node *nextSibling;
};

stack<Node *> trees;


bool checkIfEOF(ifstream &file) {
    if (!file.good() || file.peek() == EOF) {
        return true;
    }
    return false;
}

void buildTree(string nodeLabel, int noOfTreesToPopAndMakeChildren) {
    //cout << "\n# Going to build the node: '" << nodeLabel << "' in tree!";
    Node *treeNode = new Node;
    treeNode->label = nodeLabel;
    treeNode->nextSibling = NULL;
    Node *treeNodePtr = NULL; //which will point to the first child of children (if any) of the newly added node
    if (noOfTreesToPopAndMakeChildren > 0 && trees.empty()) {
        cout << "\n\nERROR! Something went wrong in AST generation! Program will die now!\n\n";
        exit(0);
    }
    while (noOfTreesToPopAndMakeChildren > 0 && !trees.empty()) {
        if (treeNodePtr != NULL) {
            //cout << "\n# Node '" << trees.top()->label << "' to be the child of tree: '" << nodeLabel <<
            //"', and left-sibling of '" << treeNodePtr->label;
            trees.top()->nextSibling = treeNodePtr;
            treeNodePtr = trees.top();
        } else {
            treeNodePtr = trees.top();
            //cout << "\n# Node '" << treeNodePtr->label << "' to be the child of tree: '" << nodeLabel << "'";
        }
        trees.pop();
        noOfTreesToPopAndMakeChildren--;
    }
    treeNode->firstKid = treeNodePtr;
    //cout << "\n# Adding to tree the Node: '" << nodeLabel << "'";
    trees.push(treeNode);
    return;
}

void readIdentifierToken(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without correct parsing through grammar! Will die now!!\n\n";
        exit(0);
    }
    nextTokenType = IDENTIFIER_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    while (isalpha(peek) || isdigit(peek) || peek == '_') {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
    if (NT.compare("rec") == 0 || NT.compare("where") == 0 || NT.compare("in") == 0 || NT.compare("and") == 0 ||
        NT.compare("let") == 0 || NT.compare("fn") == 0 || NT.compare("or") == 0 || NT.compare("not") == 0 ||
        NT.compare("gr") == 0 || NT.compare("ge") == 0 || NT.compare("ls") == 0 || NT.compare("le") == 0 ||
        NT.compare("eq") == 0 || NT.compare("ne") == 0 || NT.compare("within") == 0 || NT.compare("true") == 0 ||
        NT.compare("false") == 0 || NT.compare("nil") == 0 || NT.compare("dummy") == 0 || NT.compare("aug") == 0) {
        nextTokenType = KEYWORD_TOKEN;
        //cout << "\nKeyword: " << NT << "\n";
    }
    else {
        //cout << "\nID: " << NT << "\n";
    }
}

void readIntegerToken(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without correct parsing through grammar! Will die now!!\n\n";
        exit(0);
    }
    nextTokenType = INTEGER_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    while (isdigit(peek)) {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
    //cout << "\nINT: " << NT << "\n";
}

bool isOperator(char c) {
    if (strchr(operatorArray, c))
        return true;
    else
        return false;
}

bool isPunctuation(char c) {
    if (strchr(punctuationArray, c))
        return true;
    else
        return false;
}

void readPunctuationChar(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without correct parsing through grammar! Will die now!!\n\n";
        exit(0);
    }
    nextTokenType = PUNCTUATION_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this
    if (isPunctuation(peek)) {
        file.get(x);
        NT += x;
    }
    //cout << "\nPunc: " << NT << "\n";
}

bool isStringAllowedChar(char c) {
    if (strchr(stringAllowedCharArray, c) || isdigit(c) || isalpha(c) || isOperator(c))
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
        if (strchr(stringAllowedEscapeCharArray, peek)) {
            file.get(x);
            NT += x;
            peek = file.peek();
            return true;
        }
        else {
            cout << "\n\nERROR! Expected an escape character, but " << peek << " happened! Parser will DIE now!\n\n";
            throw exception();
        }
    }
    else
        return false;
}

void readStringToken(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without correct parsing through grammar! Will die now!!\n\n";
        exit(0);
    }
    nextTokenType = STRING_TOKEN;
    char x; //get the next character in stream in this
    char peek = file.peek(); //peek and store the next character in stream in this

    if (peek == '\'') { //check for the single quote to start the string
        file.get(x);
        NT += x; //Add quotes to the token to separate the string from non-string literals with same value
        peek = file.peek();
    } else {
        cout << "\n\nERROR! Expected start of string, but " << peek << " happened! Parser will DIE now!\n\n";
        throw exception();
    }
    while (isStringAllowedChar(peek) || (isEscapeCharInString(file, peek) && isStringAllowedChar(peek))) {
        file.get(x);
        NT += x;
        peek = file.peek();
    }
    if (peek == '\'') { //check for the single quote to close the string
        file.get(x);
        NT += x; //Add quotes to the token to separate the string from non-string literals with same value
    } else {
        cout << "\n\nERROR! Expected close of string, but " << peek << " happened! Parser will DIE now!\n\n";
        throw exception();
    }
    //cout << "\nString: " << NT << "\n";
}

void readOperatorToken(ifstream &file) {
    if (checkIfEOF(file)) {
        cout << "\n\nEOF reached unexpectedly without correct parsing through grammar! Will die now!!\n\n";
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
    //cout << "\nOp: " << NT << "\n";
}

void scan(ifstream &file);

void resolveIfCommentOrOperator(ifstream &file) {
    char x;
    file.get(x); //Move past the first '/'
    char peek = file.peek();
    if (peek == '/') {
        //This means it's a comment line, so keep reading/updating file stream pointer without "tokenizing" (adding to NT) until an eol.
        while (!strchr(eolCharArray, peek)) {
            file.get(x); //move past the whitespaces until an eol
            peek = file.peek();
        }
        file.get(x); //Move past the EOL
//        cout << "\nComment ignored";
//        cout << "\nGoing to scan!";
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

int Vl(ifstream &file, int identifiersReadBefore, bool isRecursiveCall) {
    //cout << "\nVl!";
    buildTree("<ID:" + NT + ">", 0);
    readToken(file, IDENTIFIER_TOKEN);
    if (NT.compare(",") == 0) {
        readToken(file, ",");
        identifiersReadBefore += 1;
        identifiersReadBefore = Vl(file, identifiersReadBefore, true);
    }
    int identifiersInVList = identifiersReadBefore + 1;
    if (!isRecursiveCall && identifiersInVList > 1) {
        //cout << "\nBefore calling buildTree in Vl\n";
        //cout << "\nidentifiersInVList= " << identifiersInVList << ", and trees are of number: " << trees.size();
        buildTree(",", identifiersInVList);
    }
    return identifiersReadBefore;
}

void Vb(ifstream &file) {
    //cout << "\nVb!";
    if (NT.compare("(") == 0) {
        readToken(file, "(");
        bool isVl = false;
        if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0) {
            Vl(file, 0, false);
            isVl = true;
        }
        readToken(file, ")");
        if (!isVl) {
            //cout << "\nBefore calling buildTree in Vb\n";
            buildTree("()", 0);
        }
    } else if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0) {
        buildTree("<ID:" + NT + ">", 0);
        readToken(file, IDENTIFIER_TOKEN);
    }
}

void Db(ifstream &file) {
    //cout << "\nDb!";
    if (NT.compare("(") == 0) {
        readToken(file, "(");
        D(file);
        readToken(file, ")");
    } else if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0) {
        buildTree("<ID:" + NT + ">", 0);
        readToken(file, IDENTIFIER_TOKEN);
        if (NT.compare(",") == 0) {
            readToken(file, ",");
            Vl(file, 1, false);
            readToken(file, "=");
            E(file);
            //cout << "\nBefore calling buildTree in Db\n";
            buildTree("=", 2);
        } else if (NT.compare("=") == 0) {
            readToken(file, "=");
            E(file);
            //cout << "\nBefore calling buildTree in Db1\n";
            buildTree("=", 2);
        } else {
            int n = 1;
            while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || NT.compare("(") == 0) {
                Vb(file);
                n++;
            }
            readToken(file, "=");
            E(file);
            //cout << "\nBefore calling buildTree in Db2\n";
            buildTree("function_form", n + 1); //n + 'E'
        }
    }
}

void Dr(ifstream &file) {
    //cout << "\nDr!";
    int isRec = false;
    if (NT.compare("rec") == 0) {
        //cout << "\n Going to consume \"REC!\"";
        readToken(file, "rec");
        isRec = true;
    }
    Db(file);
    if (isRec) {
        //cout << "\nBefore calling buildTree in Dr\n";
        buildTree("rec", 1);
    }
}

void Da(ifstream &file) {
    //cout << "\nDa!";
    Dr(file);
    int n = 1;
    while (NT.compare("and") == 0) {
        readToken(file, "and");
        Dr(file);
        n++;
    }
    if (n > 1) {
        //cout << "\nBefore calling buildTree in Da\n";
        buildTree("and", n); //TODO: might break. does '+' imply we need to put all children under a single node?
    }
}

void D(ifstream &file) {
    //cout << "\nD!";
    Da(file);
    if (NT.compare("within") == 0) {
        readToken(file, "within");
        D(file);
        buildTree("within", 2);
    }
}

void Rn(ifstream &file) {
    //cout << "\nRn!";
    if (nextTokenType.compare(IDENTIFIER_TOKEN) == 0) {
        //cout << "\n\nbuildTreeNode ID:" + NT + "\n\n";
        buildTree("<ID:" + NT + ">", 0);
        readToken(file, IDENTIFIER_TOKEN);
    } else if (nextTokenType.compare(STRING_TOKEN) == 0) {
        //cout << "\n\nbuildTreeNode STR:" + NT + "\n\n";
        buildTree("<STR:" + NT + ">", 0);
        readToken(file, STRING_TOKEN);
    } else if (nextTokenType.compare(INTEGER_TOKEN) == 0) {
        //cout << "\n\nbuildTreeNode INT:" + NT + "\n\n";
        buildTree("<INT:" + NT + ">", 0);
        readToken(file, INTEGER_TOKEN);
    } else if (NT.compare("true") == 0 || NT.compare("false") == 0 ||
               NT.compare("nil") == 0 || NT.compare("dummy") == 0) {
        buildTree("<" + NT + ">", 0);
        readToken(file, NT);
    } else if (NT.compare("(") == 0) {
        readToken(file, "(");
        E(file);
        readToken(file, ")");
    }
}

void R(ifstream &file) {
    //cout << "\nR!";
    Rn(file);
    while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || nextTokenType.compare(INTEGER_TOKEN) == 0 ||
           nextTokenType.compare(STRING_TOKEN) == 0 || NT.compare("true") == 0 || NT.compare("false") == 0 ||
           NT.compare("nil") == 0 || NT.compare("dummy") == 0 || NT.compare("(") == 0) {
        Rn(file);
        buildTree("gamma", 2);
    }
}

void Ap(ifstream &file) {
    //cout << "\nAp!";
    R(file);
    while (NT.compare("@") ==
           0) {
        readToken(file, "@");
        buildTree("<ID:" + NT + ">", 0); //the operator which is being inFixed, for example 'Conc'.
        readToken(file, IDENTIFIER_TOKEN);
        R(file);
        buildTree("@", 3);
    }
}

void Af(ifstream &file) {
    //cout << "\nAf!";
    Ap(file);
    if (NT.compare("**") == 0) {
        readToken(file, "**");
        Af(file);
        buildTree("**", 2);
    }
}

void At(ifstream &file) {
    //cout << "\nAt!";
    Af(file);
    while (NT.compare("*") == 0 || NT.compare("/") == 0) {
        string token = NT; //saving token since call to read will update NT with the next token.
        readToken(file, NT);
        Af(file);
        buildTree(token, 2);
    }
}

void A(ifstream &file) {
    //cout << "\nA!";
    if (NT.compare("+") == 0) {
        readToken(file, "+");
        At(file);
    } else if (NT.compare("-") == 0) {
        readToken(file, "-");
        At(file);
        buildTree("neg", 1);
    } else {
        At(file);
    }
    while (NT.compare("+") == 0 || NT.compare("-") == 0) {
        string token = NT; //saving token since call to read will update NT with the next token.
        readToken(file, NT);
        At(file);
        buildTree(token, 2);
    }
}

void Bp(ifstream &file) {
    //cout << "\nBp!";
    A(file);
    if (NT.compare("gr") == 0 || NT.compare(">") == 0) {
        readToken(file, NT);
        A(file);
        buildTree("gr", 2);
    } else if (NT.compare("ge") == 0 || NT.compare(">=") == 0) {
        readToken(file, NT);
        A(file);
        buildTree("ge", 2);
    } else if (NT.compare("ls") == 0 || NT.compare("<") == 0) {
        readToken(file, NT);
        A(file);
        buildTree("ls", 2);
    } else if (NT.compare("le") == 0 || NT.compare("<=") == 0) {
        readToken(file, NT);
        A(file);
        buildTree("le", 2);
    } else if (NT.compare("eq") == 0) {
        readToken(file, "eq");
        A(file);
        buildTree("eq", 2);
    } else if (NT.compare("ne") == 0) {
        readToken(file, "ne");
        A(file);
        buildTree("ne", 2);
    }
}

void Bs(ifstream &file) {
    //cout << "\nBs!";
    bool isNeg = false;
    if (NT.compare("not") == 0) {
        readToken(file, "not");
        isNeg = true;
    }
    Bp(file);
    if (isNeg) {
        buildTree("not", 1);
    }
}

void Bt(ifstream &file) {
    //cout << "\nBt!";
    Bs(file);
    int n = 1;
    while (NT.compare("&") == 0) {
        readToken(file, "&");
        Bs(file);
        n++;
    }
    if (n > 1) {
        buildTree("&", n);
    }
}

void B(ifstream &file) {
    //cout << "\nB!";
    Bt(file);
    int n = 1;
    while (NT.compare("or") == 0) {
        readToken(file, "or");
        Bt(file);
        n++;
    }
    if (n > 1) {
        buildTree("or", n);
    }
}

void Tc(ifstream &file) {
    //cout << "\nTc!";
    B(file);
    if (NT.compare("->") ==
        0) {
        readToken(file, "->");
        Tc(file);
        readToken(file, "|");
        Tc(file);
        buildTree("->", 3);
    }
}

void Ta(ifstream &file) {
    //cout << "\nTa!";
    Tc(file);
    while (NT.compare("aug") == 0) { //left recursion
        readToken(file, "aug");
        Tc(file);
        buildTree("aug", 2);
    }
}

void T(ifstream &file) {
    //cout << "\nT!";
    Ta(file);
    int n = 1;
    while (NT.compare(",") == 0) { //combo of left recursion AND common prefix
        n++;
        readToken(file, ",");
        Ta(file);
    }
    if (n != 1) {
        buildTree("tau", n);
    }
}

void Ew(ifstream &file) {
    //cout << "\nEw!";
    T(file);
    if (NT.compare("where") == 0) { //common prefix
        //cout << "\n Going to consume \"WHERE!\"";
        readToken(file, "where");
        Dr(file);
        buildTree("where", 2);
    }
}

void E(ifstream &file) {
    //cout << "\nE!";
    int N = 0;
    if (NT.compare("let") == 0) {
        readToken(file, "let");
        D(file);
        readToken(file, "in");
        E(file);
        buildTree("let", 2);
    } else if (NT.compare("fn") == 0) {
        readToken(file, "fn");
        do {
            Vb(file);
            N++;
        } while (nextTokenType.compare(IDENTIFIER_TOKEN) == 0 || NT.compare("(") == 0);
        readToken(file, ".");
        E(file);
        buildTree("lambda", N + 1); //number of 'Vb's plus the 'E'
    } else {
        Ew(file);
    }

}

void scan(ifstream &file) {
    if (checkIfEOF(file)) {
        //cout << "\n\nEOF reached !\n\n";
        return;
    }
    nextTokenType = UNDEFINED_TOKEN;

    char peek = file.peek(); //peek and store the next character in stream in this
    //cout << "\nIn scan, peek= '" << peek << "'";
    NT.clear(); //clear NT to get the next token in file

    if (isalpha(peek)) {
        readIdentifierToken(file);
    } else if (isdigit(peek)) {
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
        //cout << "\nGoing to scan!";
        scan(file); //call scan to get the next token
    } else if (isPunctuation(peek)) {
        readPunctuationChar(file);
    }
}

void recursivelyPrintTree(Node *node, string indentDots) {
    cout << indentDots + node->label << "\n";
    if (node->firstKid != NULL) {
        recursivelyPrintTree(node->firstKid, indentDots + ".");
    }
    if (node->nextSibling != NULL) {
        recursivelyPrintTree(node->nextSibling, indentDots);
    }
}

void printTree() {
    //cout << "\n\nGoing to print the tree now!\n\n";
    while (!trees.empty()) {
        //cout << "\n\nThis is supposed to be the only tree below!\n";
        Node *treeNode = trees.top();
        recursivelyPrintTree(treeNode, "");
        trees.pop();
    }
}

void readToken(ifstream &file, string token) {
    if (token.compare(NT) != 0 && token.compare(nextTokenType) != 0) {
        cout << "\n\nError! Expected '" << token << "' , but found '" << NT << "' !\n\n";
        throw exception();
    }
    //cout << "\ntoken '" << token << "' used! going to read next!";
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
                    //Call parser
                    //cout << "\n\nShould lexically analyze by recursively descending!!\n\n";
                    scan(the_file); //Prepare the first token by placing it within 'NT'
                    E(the_file);    //Call the first non-terminal procedure to start parsing
                    //cout << " " << NT;
                    if (checkIfEOF(the_file)) {
//                    cout << "\n\nEOF successfully reached after complete parsing! Will exit now!!\n\n";
//                    exit(1);
                        //cout << "\n\nAST Tree should print!\n\n";
                        printTree();
                    } else {
                        cout << "\n\nERROR! EOF not reached but went through the complete grammar! Will exit now!!\n\n";
                        exit(0);
                    }
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
                //cout << "\n\nShould lexically analyze by recursively descending!!\n\n";
                scan(the_file); //Prepare the first token by placing it within 'NT'
                E(the_file);    //Call the first non-terminal procedure to start parsing
                //cout << " " << NT;
                if (checkIfEOF(the_file)) {
//                    cout << "\n\nEOF successfully reached after complete parsing! Will exit now!!\n\n";
//                    exit(1);
                } else {
                    cout << "\n\nERROR! EOF not reached but went through the complete grammar! Will exit now!!\n\n";
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


