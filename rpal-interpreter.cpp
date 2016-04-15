#include <iostream>
#include <fstream>
#include <stack>
#include <list>

using namespace std;

/**
 * Author: Mickey Vellukunnel
 * UFID: 9413-9616
 *
 * University of Florida (Go Gators!)
 *
 * COP 5556 Spring 2016
 *
 * Project 2: RPal Interpreter (Built upon Project 1: RPal Parser)
 *
 */

string NT; //NextToken
const string IDENTIFIER_TOKEN = "IDENTIFIER";
const string INTEGER_TOKEN = "INTEGER";
const string STRING_TOKEN = "STRING";
const string OPERATOR_TOKEN = "OPERATOR";
const string KEYWORD_TOKEN = "KEYWORD";
const string UNDEFINED_TOKEN = "UNDEFINED";
const string PUNCTUATION_TOKEN = "PUNCTUATION";

const string FCN_FORM_LABEL = "function_form";

const string GAMMA_STD_LABEL = "std:Gamma";
const string LAMBDA_STD_LABEL = "std:Lambda";

const char *operatorArray = "+-*<>&.@/:=~|$!#!^_[]{}\"`?";

const char *stringAllowedCharArray = "();, ";

const char *stringAllowedEscapeCharArray = "tn\\\'";

const char *eolCharArray = "\r\n";

const char *punctuationArray = "();,";

string nextTokenType = UNDEFINED_TOKEN;

struct Node { // For the first child next sibling binary tree representation of nary trees.
    string label;
    struct Node *firstKid;
    struct Node *nextSibling;
};

stack<Node *> trees; //Stack of trees used to manipulate the AST generation.

/*
 * Check if the end of file has been reached.
 */
bool checkIfEOF(ifstream &file) {
    if (!file.good() || file.peek() == EOF) {
        return true;
    }
    return false;
}

/*
 * Helper function to build a new tree node, and add it to the
 * stack of trees, if necessary by popping existing trees from
 * stack and adding it as children to the new node.
 */

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

/**
 * Read an identifier/keyword token into NT.
 */
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

/**
 * Read an integer token into NT.
 */
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

/**
 * Check if the char is an operator.
 */
bool isOperator(char c) {
    if (strchr(operatorArray, c))
        return true;
    else
        return false;
}

/**
 * Check if the char is a punctuation.
 */
bool isPunctuation(char c) {
    if (strchr(punctuationArray, c))
        return true;
    else
        return false;
}

/**
 * Read a punctuation token into NT.
 */
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

/**
 * Check if the char is allowed in a string.
 */
bool isStringAllowedChar(char c) {
    if (strchr(stringAllowedCharArray, c) || isdigit(c) || isalpha(c) || isOperator(c))
        return true;
    else
        return false;
}

/**
 * Check if the char is an escape character.
 */
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

/**
 * Read a string token into NT.
 */
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

/**
 * Read an operator token into NT.
 */
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

/*
 * Check and resolve if the next token sequence is of a comment ("//") ,
 * or if it's an operator ("/").
 */
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

/*
 * The procedure for the Vl non-terminal.
 */
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

/*
 * The procedure for the Vb non-terminal.
 */
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

/*
 * The procedure for the Db non-terminal.
 */
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
            buildTree(FCN_FORM_LABEL, n + 1); //n + 'E'
        }
    }
}

/*
 * The procedure for the Dr non-terminal.
 */
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

/*
 * The procedure for the Da non-terminal.
 */
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
        buildTree("and", n);
    }
}

/*
 * The procedure for the D non-terminal.
 */
void D(ifstream &file) {
    //cout << "\nD!";
    Da(file);
    if (NT.compare("within") == 0) {
        readToken(file, "within");
        D(file);
        buildTree("within", 2);
    }
}

/*
 * The procedure for the Rn non-terminal.
 */
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

/*
 * The procedure for the R non-terminal.
 */
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

/*
 * The procedure for the Ap non-terminal.
 */
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

/*
 * The procedure for the Af non-terminal.
 */
void Af(ifstream &file) {
    //cout << "\nAf!";
    Ap(file);
    if (NT.compare("**") == 0) {
        readToken(file, "**");
        Af(file);
        buildTree("**", 2);
    }
}

/*
 * The procedure for the At non-terminal.
 */
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

/*
 * The procedure for the A non-terminal.
 */
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

/*
 * The procedure for the Bp non-terminal.
 */
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

/*
 * The procedure for the Bs non-terminal.
 */
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

/*
 * The procedure for the Bt non-terminal.
 */
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

/*
 * The procedure for the B non-terminal.
 */
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

/*
 * The procedure for the Tc non-terminal.
 */
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

/*
 * The procedure for the Ta non-terminal.
 */
void Ta(ifstream &file) {
    //cout << "\nTa!";
    Tc(file);
    while (NT.compare("aug") == 0) { //left recursion
        readToken(file, "aug");
        Tc(file);
        buildTree("aug", 2);
    }
}

/*
 * The procedure for the T non-terminal.
 */
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

/*
 * The procedure for the Ew non-terminal.
 */
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

/*
 * The procedure for the E non-terminal.
 */
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

/*
 * Read the next token sequence into NT.
 */
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

/*
 * Print the nodes of the tree in a pre-order fashion.
 */
void recursivelyPrintTree(Node *node, string indentDots) {
    cout << indentDots + node->label << "\n";
    if (node->firstKid != NULL) {
        recursivelyPrintTree(node->firstKid, indentDots + "(-.#.-)");
    }
    if (node->nextSibling != NULL) {
        recursivelyPrintTree(node->nextSibling, indentDots);
    }
}

void convertFunctionForm(Node *functionFormNode) {
    //cout<<"\nInside function form conversion!\nFunction form is:\n";
    //recursivelyPrintTree(functionFormNode, "");
    Node *fcnLambdaRightChildNodeHeader = new Node; //the "lambda" right child node header of the final standardized sub-tree
    fcnLambdaRightChildNodeHeader->label = LAMBDA_STD_LABEL;
    fcnLambdaRightChildNodeHeader->nextSibling = NULL;

    list<Node *> fcnVariableList;
    functionFormNode->label = "=";  //the "=" header node of the final standardized sub-tree

    Node *temp = functionFormNode->firstKid;    //the fcn label left child node of the final standardized sub-tree
    while (temp->nextSibling->nextSibling !=
           NULL) { //temp->nextSibling->nextSibling == NULL implies temp->nextSibling is the "Expression" part of the fcnForm
        temp = temp->nextSibling;
        fcnVariableList.push_back(temp);
    }
    //temp = temp->nextSibling; //this would be the expression part of the fcn form //the final expression node which is the rightMost child of the right sub-tree

    functionFormNode->firstKid->nextSibling = fcnLambdaRightChildNodeHeader;
    fcnLambdaRightChildNodeHeader->nextSibling = NULL;
    fcnLambdaRightChildNodeHeader->firstKid = fcnVariableList.front();
    Node *lambdaTemp = fcnLambdaRightChildNodeHeader;
    fcnVariableList.pop_front();
    while (fcnVariableList.size() > 0) {
        Node *newLambdaRightNode = new Node;
        lambdaTemp->firstKid->nextSibling = newLambdaRightNode;
        newLambdaRightNode->nextSibling = NULL;
        lambdaTemp = newLambdaRightNode;
        lambdaTemp->firstKid = fcnVariableList.front();
        fcnVariableList.pop_front();
    }
    //lambdaTemp->firstKid->nextSibling = temp;
    //cout<<"\nInside function form conversion!\nThe standardized Function form is:\n";
    recursivelyPrintTree(functionFormNode, "");
}

/* DO not optimize the unary and binary operators (optimizations for the CISE machine)
void convertUop(Node *uopNode) {
    //cout<<"\nInside uop conversion!\nuop ast form before standardizing is:\n";
    //recursivelyPrintTree(uopNode, "");
    Node *uopLeftChild = new Node;
    uopLeftChild->label = uopNode->label;
    uopLeftChild->nextSibling = uopNode->firstKid;
    uopNode->label = GAMMA_STD_LABEL;
    uopNode->firstKid = uopLeftChild;
    //cout<<"\nThe standardized uop is:\n";
    //recursivelyPrintTree(uopNode, "");
}

void convertOperator(Node *binaryOperatorNode) {
    //cout<<"\nInside Operator conversion!\nOp ast form before standardizing is:\n";
    //recursivelyPrintTree(opNode, "");
    Node *leftGammaChild = new Node;
    Node *leftLeftOperatorChild = new Node;
    leftLeftOperatorChild->label = binaryOperatorNode->label;
    leftLeftOperatorChild->firstKid = NULL;
    leftLeftOperatorChild->nextSibling = binaryOperatorNode->firstKid; //E1
    leftGammaChild->label = GAMMA_STD_LABEL;
    leftGammaChild->firstKid = leftLeftOperatorChild;
    leftGammaChild->nextSibling = binaryOperatorNode->firstKid->nextSibling; //E2
    binaryOperatorNode->firstKid->nextSibling = NULL;
    binaryOperatorNode->firstKid = leftGammaChild;
    binaryOperatorNode->label = GAMMA_STD_LABEL;
    //cout<<"\nThe standardized operator is:\n";
    //recursivelyPrintTree(opNode, "");
}
 */

void convertInfixOperator(Node *infixOperatorNode) {
    //cout<<"\nInside Infix Operator conversion!\nInfix ast form before standardizing is:\n";
    //recursivelyPrintTree(opNode, "");
    Node *leftGammaChild = new Node;
    Node *leftLeftOperatorChild = new Node;
    leftLeftOperatorChild->label = infixOperatorNode->firstKid->nextSibling->label;
    leftLeftOperatorChild->nextSibling = infixOperatorNode->firstKid; //E1
    leftGammaChild->label = GAMMA_STD_LABEL;
    leftGammaChild->firstKid = leftLeftOperatorChild;
    leftGammaChild->nextSibling = infixOperatorNode->firstKid->nextSibling->nextSibling; //E2
    infixOperatorNode->firstKid->nextSibling = NULL;
    infixOperatorNode->firstKid = leftGammaChild;
    infixOperatorNode->label = GAMMA_STD_LABEL;
    //cout<<"\nThe standardized Infix operator is:\n";
    //recursivelyPrintTree(opNode, "");
}

void convertLambdaExpression(Node *lambdaNode) {
    //cout<<"\nInside lambda expression conversion!\nlambda expression ast form before standardizing is:\n";
    //recursivelyPrintTree(lambdaNode, "");
    lambdaNode->label = LAMBDA_STD_LABEL;

    list<Node *> fcnVariableList;

    Node *temp = lambdaNode->firstKid;    //the top left child node of the final standardized lambda sub-tree
    while (temp->nextSibling->nextSibling !=
           NULL) { //temp->nextSibling->nextSibling == NULL implies temp->nextSibling is the "Expression" part of the fcnForm
        temp = temp->nextSibling;
        fcnVariableList.push_back(temp);
    }
    temp = temp->nextSibling; //this would be the expression part of the fcn form //the final expression node which is the rightMost child of the right sub-tree

    Node *lambdaTemp = lambdaNode;
    while (fcnVariableList.size() > 0) {
        Node *newLambdaRightNode = new Node;
        lambdaTemp->firstKid->nextSibling = newLambdaRightNode;
        newLambdaRightNode->nextSibling = NULL;
        newLambdaRightNode->label = LAMBDA_STD_LABEL;
        lambdaTemp = newLambdaRightNode;
        lambdaTemp->firstKid = fcnVariableList.front();
        fcnVariableList.pop_front();
    }
    lambdaTemp->firstKid->nextSibling = temp; // E
    //cout<<"\nThe standardized lambda expression is:\n";
    //recursivelyPrintTree(lambdaNode, "");
}

void convertAndExpression(Node *andHeaderNode) {
    andHeaderNode->label = "=";
    Node *tempEqualsChildHeaderNode = andHeaderNode->firstKid;
    list<Node *> variableNodesList;
    list<Node *> expressionNodesList;
    while (tempEqualsChildHeaderNode != NULL) {
        variableNodesList.push_back(tempEqualsChildHeaderNode->firstKid);
        expressionNodesList.push_back(tempEqualsChildHeaderNode->firstKid->nextSibling);
        tempEqualsChildHeaderNode = tempEqualsChildHeaderNode->nextSibling;
    }

    Node *commaHeaderNode = new Node;
    Node *tauHeaderNode = new Node;

    commaHeaderNode->label = ",";
    tauHeaderNode->label = "tau";
    tauHeaderNode->nextSibling = NULL;
    commaHeaderNode->nextSibling = tauHeaderNode;

    andHeaderNode->firstKid = commaHeaderNode;

    Node *commaVariableTempNode, *tauExpressionTempNode;

    commaVariableTempNode = variableNodesList.front();
    variableNodesList.pop_front();
    tauExpressionTempNode = expressionNodesList.front();
    expressionNodesList.pop_front();

    commaHeaderNode->firstKid = commaVariableTempNode;

    tauHeaderNode->firstKid = tauExpressionTempNode;

    while (!variableNodesList.empty()) {
        commaVariableTempNode->nextSibling = variableNodesList.front();
        variableNodesList.pop_front();
        tauExpressionTempNode->nextSibling = expressionNodesList.front();
        expressionNodesList.pop_front();
        commaVariableTempNode = commaVariableTempNode->nextSibling;
        tauExpressionTempNode = tauExpressionTempNode->nextSibling;
    }

    commaVariableTempNode->nextSibling = NULL;
    tauExpressionTempNode->nextSibling = NULL;

}

void convertLetExpression(Node *letNode) {
    letNode->label = GAMMA_STD_LABEL;

    letNode->firstKid->label = LAMBDA_STD_LABEL;

    Node *pNode = letNode->firstKid->nextSibling;
    Node *eNode = letNode->firstKid->firstKid->nextSibling;

    //switch the p and e nodes

    letNode->firstKid->nextSibling = eNode;
    letNode->firstKid->firstKid->nextSibling = pNode;


}

void convertRecExpression(Node *recNode) {
    recNode = recNode->firstKid;

    Node *rightGammaChild = new Node;
    rightGammaChild->label = GAMMA_STD_LABEL;
    Node *rightRightLambdaChild = new Node;
    rightRightLambdaChild->label = LAMBDA_STD_LABEL;

    Node *leftChildYNode = new Node;
    leftChildYNode->label = "Y";

    rightGammaChild->firstKid = leftChildYNode;
    leftChildYNode->nextSibling = rightRightLambdaChild;

    Node *functionNameNode = new Node;
    functionNameNode->label = recNode->firstKid->label;

    rightRightLambdaChild->firstKid = functionNameNode;
    functionNameNode->nextSibling = recNode->firstKid->nextSibling; //E

    recNode->firstKid->nextSibling = rightGammaChild;

}

void convertWhereExpression(Node *whereNode) {
    whereNode->label = GAMMA_STD_LABEL;


    Node *pNode = whereNode->firstKid;
    Node *leftChildLambdaNode = pNode->nextSibling;
    leftChildLambdaNode->label = LAMBDA_STD_LABEL;
    Node *eNode = leftChildLambdaNode->firstKid->nextSibling;

    whereNode->firstKid = leftChildLambdaNode;

    //switch the p and e nodes

    leftChildLambdaNode->nextSibling = eNode;
    leftChildLambdaNode->firstKid->nextSibling = pNode;

    pNode->nextSibling = NULL;
}

void convertWithinExpression(Node *withinNode) {
    withinNode->label = "=";

    Node *withinOne = withinNode->firstKid;
    Node *withinTwo = withinOne->nextSibling;

    Node *rightGammaChild = new Node;
    Node *rightLeftLambdaChild = new Node;
    rightGammaChild->label = GAMMA_STD_LABEL;
    rightLeftLambdaChild->label = LAMBDA_STD_LABEL;

    rightGammaChild->firstKid = rightLeftLambdaChild;
    rightLeftLambdaChild->nextSibling = withinOne->firstKid->nextSibling; //E1
    rightLeftLambdaChild->firstKid = withinOne->firstKid; //X1
    rightLeftLambdaChild->firstKid->nextSibling = withinTwo->firstKid->nextSibling; //E2

    withinNode->firstKid = withinTwo->firstKid; //X2
    withinNode->firstKid->nextSibling = rightGammaChild;


}

/*
 * Standardize the nodes of the tree in a pre-order fashion.
 */
void recursivelyStandardizeTree(Node *node) {
    if (node->firstKid != NULL) {
        recursivelyStandardizeTree(node->firstKid);
    }
    if (node->nextSibling != NULL) {
        recursivelyStandardizeTree(node->nextSibling);
    }
    if (node->label == "->") {
        //Do not standardize conditionals (optimizations for the CISE machine)
    } else if (node->label == "not" || node->label == "neg") { //convert unary operators to standardized form
        //Do not standardize unary operators (optimizations for the CISE machine) //convertUop(node);
    } else if (node->label == "aug" || node->label == "or" || node->label == "&" || node->label == "gr" ||
               node->label == "ge" || node->label == "ls" || node->label == "le" || node->label == "eq" ||
               node->label == "ne" || node->label == "+" || node->label == "-" || node->label == "*" ||
               node->label == "/" || node->label == "**") {
        //Do not standardize binary operators (optimizations for the CISE machine) //convertOperator(node);
    } else if (node->label == "tau") {
        //Do not standardize tau (optimizations for the CISE machine)
    } else if (node->label == "lambda") {    //convert lambda expression to standardized form
        if (node->firstKid->label == ",") { //lambda expression with a tuple of variables
            //Do not standardize lambda with a tuple of variables (optimizations for the CISE machine)
        } else {    //lambda expression with a list(?) of variable(s)
            convertLambdaExpression(node);
        }
    } else if (node->label == FCN_FORM_LABEL) {    //convert function_form to standardized form
        convertFunctionForm(node);
    } else if (node->label == "@") {    //convert infix operator to standardized form
        convertInfixOperator(node);
    } else if (node->label == "and") {
        convertAndExpression(node);
    } else if (node->label == "within") {
        convertWithinExpression(node);
    } else if (node->label == "rec") {
        convertRecExpression(node);
    } else if (node->label == "let") {
        convertLetExpression(node);
    } else if (node->label == "where") {
        convertWhereExpression(node);
    }
}

/*
 * Function to pop the (only) node
 * from the stack of trees and print the nodes of the tree
 * in a pre-order fashion.
 */
void printTree() {
    //cout << "\n\nGoing to print the tree now!\n\n";
    if (!trees.empty()) {
        //cout << "\n\nThis is supposed to be the only tree below!\n";
        Node *treeRootOfAST = trees.top();
        recursivelyPrintTree(treeRootOfAST, "");
    }
}

/*
 * Function to pop the (only) node
 * from the stack of trees and convert the nodes of the tree
 * to a standardized tree (tree whose internal nodes are only gammas or lambdas)
 * in a pre-order fashion.
 */
void convertASTToStandardizedTree() {
    cout << "\n\nGoing to standardize the tree now!\n\n";
    if (!trees.empty()) {
        //cout << "\n\nThis is supposed to be the only tree below!\n";
        Node *treeRootOfAST = trees.top();
        recursivelyStandardizeTree(treeRootOfAST);
    }
}

/*
 * Function to consume the next token from input with the next
 * expected token by the RPAL grammar and move ahead to read the
 * next token from input.
 */
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
                    /* Optional switches: -l This produces a listing of the input. */
                    cout << "\n\n";
                    char x;
                    // the_file.get ( x ) returns false if the end of the file
                    //  is reached or an error occurs
                    while (the_file.get(x))
                        cout << x;
                    // the_file is closed implicitly here
                    cout << "\n\n";
                } else if (strcmp(argv[1], "-ast") == 0) {
                    /*
                     * Required switches: -ast This switch prints the abstract syntax tree, and nothing else.
                     * No headers or footers.
                     * The AST must match exactly, character for character, the AST produced by rpal.
                     */
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
            /* p2 (without switches) should produce the result of the program. */
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
                    cout << "\nThe AST is:\n";
                    printTree(); //print the AST
                    convertASTToStandardizedTree();
                    cout << "\nThe standardized ST is:\n";
                    printTree(); //print the standardized tree
                } else {
                    cout << "\n\nERROR! EOF not reached but went through the complete grammar! Will exit now!!\n\n";
                    exit(0);
                }
            }
        }
    }
    else if (argc != 2) {
        /*
         * p2 (without switches) should produce the result of the program.
         * Required switches: -ast This switch prints the abstract syntax tree, and nothing else.
         * No headers or footers. The AST must match exactly, character for character, the AST produced by rpal.
         * Optional switches: -l This produces a listing of the input.
         */
        cout << "\n\nUsages:\n" << argv[0] << " <filename>\n";
        cout << argv[0] << " -ast <filename>\n";
        cout << argv[0] << " -l <filename>\n\n";
    }

}


