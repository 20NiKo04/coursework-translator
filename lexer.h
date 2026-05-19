#ifndef LEXER_H
#define LEXER_H

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <iostream>

using namespace std;

class Lexer {
private:
    ifstream file;
    char currentSymbol;
    int currentStr = 1;
    int currentPos = 0;

    unordered_map<int, string> keywordType = {
        {1, "int"}, {2, "while"}, {3, "read"}, {4, "write"}
    };

    unordered_map<int, string> specialSymbol = {
        {1, "+"}, {2, "-"}, {3, "*"}, {4, "/"}, {5, "%"},
        {6, "="}, {7, "=="}, {8, "!="}, {9, "<"}, {10, ">"},
        {11, "<="}, {12, ">="}, {13, "&&"}, {14, "||"}, {15, "!"},
        {16, "("}, {17, ")"}, {18, "{"}, {19, "}"}, {20, ";"}, {21, ","}
    };

    map<int, string> readWord;      
    map<int, string> readConst;      
    map<int, string> readKeywordType; 
    map<int, string> readSpecialSymbol; 
    map<pair<int, int>, char> readErrorSymbol; 

    vector<pair<int, int>> analyzedInfo;

    void getNextSymbol();
    void addAnalyzedInfo(int tableNum, int str);
    void addReadWordTable(string value);
    void addReadConstTable(string value);
    void addReadKeywordTypeTable(int position, string value);
    void addReadSpecialSymbolTable(int position, string value);
    void addReadErrorSymbolTable(int str, int pos, char value);
    bool isKeyword(string value);
    bool isSpecialSymbol(string value);
    string peekNextChar();

public:
    Lexer(string filename);
    void getNextToken();

    void printReadWord();
    void printReadConst();
    void printReadKeywordType();
    void printReadSpecialSymbol();
    void printReadErrorSymbol();
    void printAnalyzedInfo();
    void printOutputToFile(string filename);

    pair<int, int> getElAnalyzedInfo(int pos);
    string getElReadWord(int pos);
    string getElReadConst(int pos);
    string getElReadKeywordType(int pos);
    string getElReadSpecialSymbol(int pos);
    int getSizeAnalyzedInfo();
    bool isWord(string currentElement);
};

#endif