#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <vector>
#include <unordered_map>
#include "semantic.h"

using namespace std;

class Interpreter {
private:
    vector<Tetrad> tetrads;
    unordered_map<string, int> memory;
    unordered_map<string, int> labels;

    bool isIntegerLiteral(const string& value);
    int getValue(const string& operand);
    void setValue(const string& name, int value);
    int divide(int left, int right, const string& op);
    int getLabelIndex(const string& labelName);
    void collectLabels();

public:
    Interpreter(vector<Tetrad>& tetrads);
    void run();
    void printMemory();
    void saveToFile(const string& filename);
};

#endif