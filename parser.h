#ifndef PARSER_H
#define PARSER_H

#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include "lexer.h"
#include "semantic.h"

using namespace std;

class Parser {
private:
    Lexer* lexer;
    SemanticAnalyzer* semantic;
    ofstream outputFile;
    pair<int, int> currentElement;
    int analyzePos;

    void getNextElement();
    void getPrevElement();
    string getElementValue();
    string getElementValueAt(int pos);
    bool checkElement(int table, int line = -1);
    void syntaxError(const string& msg);
    void enterScope();
    void exitScope();
    void readStmt();

    void program();
    void declaration();
    void statement();
    string assignment();
    void whileStmt();
    void writeStmt();
    void compoundStmt();

    string expression();
    string comparison();
    string additive();
    string multiplicative();
    string unary();
    string primary();

public:
    Parser(Lexer& lex, SemanticAnalyzer& sem);
    ~Parser();
    bool run();
    void saveToFile(const string& filename);
};

#endif
