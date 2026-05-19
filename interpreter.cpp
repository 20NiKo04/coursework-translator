#include "interpreter.h"
#include <cctype>
#include <iostream>
#include <fstream>

Interpreter::Interpreter(vector<Tetrad>& tetrads) {
    this->tetrads = tetrads;
    collectLabels();
}

bool Interpreter::isIntegerLiteral(const string& value) {
    if (value.empty()) return false;
    int pos = 0;
    if (value[0] == '-' || value[0] == '+') {
        if (value.size() == 1) return false;
        pos = 1;
    }
    for (int i = pos; i < (int)value.size(); i++) {
        if (!isdigit(value[i])) return false;
    }
    return true;
}

int Interpreter::getValue(const string& operand) {
    if (operand.empty()) return 0;
    if (isIntegerLiteral(operand)) return stoi(operand);
    auto it = memory.find(operand);
    if (it != memory.end()) return it->second;
    return 0;
}

void Interpreter::setValue(const string& name, int value) {
    memory[name] = value;
}

void Interpreter::collectLabels() {
    labels.clear();
    for (int i = 0; i < (int)tetrads.size(); i++) {
        if (tetrads[i].op == "LABEL") {
            labels[tetrads[i].arg1] = i;
        }
    }
}

int Interpreter::getLabelIndex(const string& labelName) {
    auto it = labels.find(labelName);
    if (it != labels.end()) return it->second;
    cerr << "Ошибка: метка " << labelName << " не найдена" << endl;
    exit(1);
}

void Interpreter::run() {
    int pc = 0;
    cout << "\nИнтерпретация" << endl;

    while (pc >= 0 && pc < (int)tetrads.size()) {
        Tetrad& q = tetrads[pc];
        string& op = q.op;

        if (op == "LABEL") {
            pc++;
        }
        else if (op == "JMP") {
            pc = getLabelIndex(q.result);
        }
        else if (op == "JF") {
            if (getValue(q.arg1) == 0) {
                pc = getLabelIndex(q.result);
            }
            else {
                pc++;
            }
        }
        else if (op == "READ") {
            string input;
            cout << "Введите значение для " << q.result << ": ";
            cin >> input;
            setValue(q.result, stoi(input));
            pc++;
        }
        else if (op == "WRITE") {
            cout << getValue(q.arg1) << endl;
            pc++;
        }
        else if (op == "=") {
            setValue(q.result, getValue(q.arg1));
            pc++;
        }
        else if (op == "+") {
            setValue(q.result, getValue(q.arg1) + getValue(q.arg2));
            pc++;
        }
        else if (op == "-") {
            setValue(q.result, getValue(q.arg1) - getValue(q.arg2));
            pc++;
        }
        else if (op == "*") {
            setValue(q.result, getValue(q.arg1) * getValue(q.arg2));
            pc++;
        }
        else if (op == "/") {
            int right = getValue(q.arg2);
            if (right == 0) {
                cerr << "Ошибка: деление на ноль" << endl;
                exit(1);
            }
            setValue(q.result, getValue(q.arg1) / right);
            pc++;
        }
        else if (op == "<") {
            setValue(q.result, (getValue(q.arg1) < getValue(q.arg2)) ? 1 : 0);
            pc++;
        }
        else {
            cerr << "Ошибка: неизвестная операция '" + op + "'" << endl;
            exit(1);
        }
    }

    cout << "\nВыполнение завершено" << endl;
}

void Interpreter::printMemory() {
    cout << "\n Память после выполнения" << endl;
    for (const auto& [key, value] : memory) {
        cout << key << " = " << value << endl;
    }
}

void Interpreter::saveToFile(const string& filename) {
    ofstream out(filename);
    if (!out.is_open()) return;
    out << " Интерпретатор" << endl;
    out << "Память после выполнения:" << endl;
    for (const auto& [key, value] : memory) {
        out << key << " = " << value << endl;
    }
    out.close();
}