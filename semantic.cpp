#include "semantic.h"
#include <iostream>
#include <cctype>

SemanticAnalyzer::SemanticAnalyzer()
    : tempCounter(0), labelCounter(0), lastTemp(""), currentScope(nullptr) {
    enterScope("global");
}

SemanticAnalyzer::~SemanticAnalyzer() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
    for (auto scope : allScopes) {
        delete scope;
    }
}

string SemanticAnalyzer::generateTemp() {
    return "t" + to_string(tempCounter++);
}

string SemanticAnalyzer::generateLabel() {
    return "L" + to_string(labelCounter++);
}

void SemanticAnalyzer::addTetrad(const string& op, const string& arg1,
    const string& arg2, const string& result) {
    tetrads.emplace_back(tetrads.size(), op, arg1, arg2, result);
}

void SemanticAnalyzer::enterScope(const string& name) {
    int level = currentScope ? currentScope->level + 1 : 0;
    Scope* newScope = new Scope(name, level, currentScope);
    currentScope = newScope;
    allScopes.push_back(newScope);
}

void SemanticAnalyzer::exitScope() {
    if (currentScope && currentScope->parent) {
        currentScope = currentScope->parent;
    }
}

Scope* SemanticAnalyzer::findVariable(const string& varName) {
    Scope* scope = currentScope;
    while (scope) {
        if (scope->variables.find(varName) != scope->variables.end()) {
            return scope;
        }
        scope = scope->parent;
    }
    return nullptr;
}

void SemanticAnalyzer::checkVariableDeclared(const string& varName) {
    if (!findVariable(varName)) {
        cerr << "Семантическая ошибка: переменная '" + varName + "' не объявлена" << endl;
        exit(1);
    }
}

void SemanticAnalyzer::checkVariableInitialized(const string& varName) {
    Scope* scope = findVariable(varName);
    if (!scope) {
        cerr << "Семантическая ошибка: переменная '" + varName + "' не объявлена" << endl;
        exit(1);
    }
    auto it = scope->variables.find(varName);
    if (it != scope->variables.end() && !it->second.initialized) {
        cerr << "Семантическая ошибка: переменная '" + varName + "' не инициализирована" << endl;
        exit(1);
    }
}

bool SemanticAnalyzer::declareVariable(const string& name, bool initialized) {
    if (!currentScope) return false;

    if (currentScope->variables.find(name) != currentScope->variables.end()) {
        cerr << "Семантическая ошибка: переменная '" + name + "' уже объявлена" << endl;
        return false;
    }

    VarInfo info;
    info.name = name;
    info.type = "int";
    info.declared = true;
    info.initialized = initialized;

    currentScope->variables[name] = info;
    return true;
}

bool SemanticAnalyzer::isDeclared(const string& name) {
    return findVariable(name) != nullptr;
}

bool SemanticAnalyzer::isInitialized(const string& name) {
    Scope* scope = findVariable(name);
    if (!scope) return false;
    auto it = scope->variables.find(name);
    return it != scope->variables.end() && it->second.initialized;
}

void SemanticAnalyzer::setInitialized(const string& name) {
    Scope* scope = findVariable(name);
    if (scope) {
        scope->variables[name].initialized = true;
    }
}

string SemanticAnalyzer::newTempVar() {
    return generateTemp();
}

string SemanticAnalyzer::newLabel() {
    return generateLabel();
}

void SemanticAnalyzer::genAssignment(const string& var, const string& expr) {
    addTetrad("=", expr, "", var);
    setInitialized(var);
}

void SemanticAnalyzer::genBinaryOp(const string& op, const string& left,
    const string& right, const string& result) {
    // Поддерживаемые бинарные операции: +, -, *, /, %, &&, ||, !
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
        op == "&&" || op == "||" || op == "!") {
        addTetrad(op, left, right, result);
    }
    else {
        cerr << "Семантическая ошибка: неизвестная операция '" + op + "'" << endl;
        exit(1);
    }
}

void SemanticAnalyzer::genComparison(const string& op, const string& left,
    const string& right, const string& result) {
    // Поддерживаемые операции сравнения: <, >, <=, >=, ==, !=
    if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
        addTetrad(op, left, right, result);
    }
    else {
        cerr << "Семантическая ошибка: неизвестная операция сравнения '" + op + "'" << endl;
        exit(1);
    }
}

void SemanticAnalyzer::genRead(const string& var) {
    addTetrad("READ", "", "", var);
    setInitialized(var);
}

void SemanticAnalyzer::genWrite(const string& expr) {
    addTetrad("WRITE", expr, "", "");
}

void SemanticAnalyzer::genLabel(const string& label) {
    addTetrad("LABEL", label, "", "");
}

void SemanticAnalyzer::genJump(const string& label) {
    addTetrad("JMP", "", "", label);
}

void SemanticAnalyzer::genJumpFalse(const string& cond, const string& label) {
    addTetrad("JF", cond, "", label);
}

void SemanticAnalyzer::printTetrads() {
    cout << "\n Тетрады" << endl;
    cout << " N | Операция | Арг1 | Арг2 | Результат" << endl;
    cout << "----------------------------------------" << endl;

    for (const auto& t : tetrads) {
        cout << " " << t.index << " | " << t.op;

        cout << " | ";
        if (!t.arg1.empty()) cout << t.arg1;
        else cout << " ";

        cout << " | ";
        if (!t.arg2.empty()) cout << t.arg2;
        else cout << " ";

        cout << " | ";
        if (!t.result.empty()) cout << t.result;
        else cout << " ";

        cout << endl;
    }
}

void SemanticAnalyzer::printSymbolTable() {
    cout << "\n Таблица символов" << endl;
    cout << "Область | Уровень | Имя | Тип | Инициализирована" << endl;
    cout << "----------------------------------------" << endl;
    for (auto scope : allScopes) {
        for (auto& entry : scope->variables) {
            VarInfo& var = entry.second;
            cout << scope->name << " | " << scope->level << " | "
                << var.name << " | " << var.type << " | "
                << (var.initialized ? "Да" : "Нет") << endl;
        }
    }
}

void SemanticAnalyzer::saveToFile(const string& filename) {
    outputFile.open(filename);
    if (!outputFile.is_open()) return;

    outputFile << "Сематический анализатор" << endl;
    outputFile << "Семантических ошибок не обнаружено" << endl << endl;

    outputFile << "Таблица символов:" << endl;
    outputFile << "Область | Уровень | Имя | Тип | Инициализирована" << endl;
    for (auto scope : allScopes) {
        for (auto& entry : scope->variables) {
            VarInfo& var = entry.second;
            outputFile << scope->name << " | " << scope->level << " | "
                << var.name << " | " << var.type << " | "
                << (var.initialized ? "Да" : "Нет") << endl;
        }
    }

    outputFile << endl << "Тетрады:" << endl;
    outputFile << "Номер | Операция | Арг1 | Арг2 | Результат" << endl;
    for (const auto& t : tetrads) {
        outputFile << t.index << " | " << t.op << " | "
            << t.arg1 << " | " << t.arg2 << " | " << t.result << endl;
    }

    outputFile.close();
}
