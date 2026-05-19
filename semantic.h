#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

struct Tetrad {
    int index;
    string op;
    string arg1;
    string arg2;
    string result;

    Tetrad(int idx, string o, string a1 = "", string a2 = "", string r = "")
        : index(idx), op(o), arg1(a1), arg2(a2), result(r) {
    }
};

struct VarInfo {
    string name;
    string type;
    bool declared;
    bool initialized;
    VarInfo() : name(""), type("int"), declared(false), initialized(false) {}
    VarInfo(string n) : name(n), type("int"), declared(true), initialized(false) {}
};

struct Scope {
    string name;
    int level;
    Scope* parent;
    unordered_map<string, VarInfo> variables;
    Scope(string n, int l, Scope* p = nullptr) : name(n), level(l), parent(p) {}
};

class SemanticAnalyzer {
private:
    vector<Tetrad> tetrads;
    int tempCounter;
    int labelCounter;
    string lastTemp;

    Scope* currentScope;
    vector<Scope*> allScopes;
    ofstream outputFile;

    string generateTemp();
    string generateLabel();
    void addTetrad(const string& op, const string& arg1 = "",
        const string& arg2 = "", const string& result = "");

    void checkVariableDeclared(const string& varName);
    void checkVariableInitialized(const string& varName);
    Scope* findVariable(const string& varName);

public:
    SemanticAnalyzer();
    ~SemanticAnalyzer();

    void enterScope(const string& name = "block");
    void exitScope();

    bool declareVariable(const string& name, bool initialized = false);
    bool isDeclared(const string& name);
    bool isInitialized(const string& name);
    void setInitialized(const string& name);

    string newTempVar();
    string newLabel();
    void setLastTemp(const string& temp) { lastTemp = temp; }
    string getLastTemp() const { return lastTemp; }

    void genAssignment(const string& var, const string& expr);
    void genBinaryOp(const string& op, const string& left,
        const string& right, const string& result);
    void genComparison(const string& left, const string& right,
        const string& result);
    void genRead(const string& var);
    void genWrite(const string& expr);
    void genLabel(const string& label);
    void genJump(const string& label);
    void genJumpFalse(const string& cond, const string& label);

    void printTetrads();
    void printSymbolTable();
    void saveToFile(const string& filename);

    vector<Tetrad>& getTetrads() { return tetrads; }
};

#endif