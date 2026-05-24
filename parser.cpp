#include "parser.h"
#include <iostream>

Parser::Parser(Lexer& lex, SemanticAnalyzer& sem)
    : lexer(&lex), semantic(&sem), analyzePos(-1) {
}

Parser::~Parser() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}

void Parser::getNextElement() {
    if (analyzePos + 1 < lexer->getSizeAnalyzedInfo()) {
        analyzePos++;
        currentElement = lexer->getElAnalyzedInfo(analyzePos);
    }
    else {
        currentElement = { 0, 0 };
    }
}

void Parser::getPrevElement() {
    if (analyzePos > 0) {
        analyzePos--;
        currentElement = lexer->getElAnalyzedInfo(analyzePos);
    }
}

string Parser::getElementValue() {
    switch (currentElement.first) {
    case 1: return lexer->getElReadWord(currentElement.second);
    case 2: return lexer->getElReadConst(currentElement.second);
    case 3: return lexer->getElReadKeywordType(currentElement.second);
    case 4: return lexer->getElReadSpecialSymbol(currentElement.second);
    default: return "";
    }
}

string Parser::getElementValueAt(int pos) {
    if (pos < 0 || pos >= lexer->getSizeAnalyzedInfo()) return "";
    auto elem = lexer->getElAnalyzedInfo(pos);
    switch (elem.first) {
    case 1: return lexer->getElReadWord(elem.second);
    case 2: return lexer->getElReadConst(elem.second);
    case 3: return lexer->getElReadKeywordType(elem.second);
    case 4: return lexer->getElReadSpecialSymbol(elem.second);
    default: return "";
    }
}

bool Parser::checkElement(int table, int line) {
    if (currentElement.first == table) {
        if (line == -1 || currentElement.second == line) {
            return true;
        }
    }
    return false;
}

void Parser::syntaxError(const string& msg) {
    cerr << "Синтаксическая ошибка: " << msg << ", найдено: "
        << getElementValue() << endl;
    exit(1);
}

void Parser::enterScope() {
    semantic->enterScope("block");
}

void Parser::exitScope() {
    semantic->exitScope();
}

void Parser::program() {
    getNextElement();
    while (currentElement.first != 0) {
        if (checkElement(3, 1)) {
            declaration();
        }
        else {
            statement();
        }
        if (currentElement.first == 0) break;
    }
}

void Parser::declaration() {
    getNextElement();

    if (!checkElement(1)) {
        syntaxError("Ожидается идентификатор");
    }
    string varName = getElementValue();
    getNextElement();

    bool initialized = false;
    string initValue = "";

    if (checkElement(4, 6)) {
        getNextElement();
        initValue = expression();
        initialized = true;
    }

    if (!checkElement(4, 20)) {
        syntaxError("Ожидается ';'");
    }

    semantic->declareVariable(varName, initialized);
    if (initialized) {
        semantic->genAssignment(varName, initValue);
    }

    getNextElement();
}

void Parser::statement() {
    if (checkElement(3, 2)) {
        whileStmt();
    }
    else if (checkElement(3, 4)) {
        writeStmt();
    }
    else if (checkElement(3, 3)) {
        readStmt();
    }
    else if (checkElement(4, 18)) {
        compoundStmt();
    }
    else if (checkElement(1)) {
        assignment();
    }
    else {
        syntaxError("Неожиданная конструкция");
    }
}

string Parser::assignment() {
    if (!checkElement(1)) {
        syntaxError("Ожидается идентификатор");
    }
    string varName = getElementValue();
    getNextElement();

    if (!checkElement(4, 6)) {
        syntaxError("Ожидается '='");
    }
    getNextElement();

    string exprResult = expression();

    if (!checkElement(4, 20)) {
        syntaxError("Ожидается ';'");
    }

    semantic->genAssignment(varName, exprResult);
    getNextElement();

    return varName;
}

void Parser::whileStmt() {
    getNextElement();

    if (!checkElement(4, 16)) {
        syntaxError("Ожидается '('");
    }
    getNextElement();

    string startLabel = semantic->newLabel();
    string endLabel = semantic->newLabel();

    semantic->genLabel(startLabel);

    string condResult = expression();

    if (!checkElement(4, 17)) {
        syntaxError("Ожидается ')'");
    }
    getNextElement();

    semantic->genJumpFalse(condResult, endLabel);

    compoundStmt();

    semantic->genJump(startLabel);
    semantic->genLabel(endLabel);
}

void Parser::writeStmt() {
    getNextElement();

    if (!checkElement(4, 16)) {
        syntaxError("Ожидается '('");
    }
    getNextElement();

    string exprResult = expression();

    if (!checkElement(4, 17)) {
        syntaxError("Ожидается ')'");
    }
    getNextElement();

    if (!checkElement(4, 20)) {
        syntaxError("Ожидается ';'");
    }

    semantic->genWrite(exprResult);
    getNextElement();
}

void Parser::readStmt() {
    getNextElement();

    if (!checkElement(4, 16)) {
        syntaxError("Ожидается '('");
    }
    getNextElement();

    if (!checkElement(1)) {
        syntaxError("Ожидается идентификатор");
    }
    string varName = getElementValue();

    if (!semantic->isDeclared(varName)) {
        syntaxError("Переменная не объявлена: " + varName);
    }

    getNextElement();

    if (!checkElement(4, 17)) {
        syntaxError("Ожидается ')'");
    }
    getNextElement();

    if (!checkElement(4, 20)) {
        syntaxError("Ожидается ';'");
    }

    semantic->genRead(varName);
    getNextElement();
}

void Parser::compoundStmt() {
    if (!checkElement(4, 18)) {
        syntaxError("Ожидается '{'");
    }

    enterScope();
    getNextElement();

    while (currentElement.first != 0 && !checkElement(4, 19)) {
        if (checkElement(3, 1)) {
            declaration();
        }
        else {
            statement();
        }
        if (checkElement(4, 19)) break;
    }

    if (!checkElement(4, 19)) {
        syntaxError("Ожидается '}'");
    }

    exitScope();
    getNextElement();
}

string Parser::expression() {
    string left = comparison();

    while (true) {
        if (checkElement(4, 13)) {   // &&
            getNextElement();
            string right = comparison();
            string temp = semantic->newTempVar();
            semantic->genBinaryOp("&&", left, right, temp);
            left = temp;
        }
        else if (checkElement(4, 14)) {   // ||
            getNextElement();
            string right = comparison();
            string temp = semantic->newTempVar();
            semantic->genBinaryOp("||", left, right, temp);
            left = temp;
        }
        else {
            break;
        }
    }

    return left;
}

string Parser::comparison() {
    string left = additive();

    if (checkElement(4, 9)) {   // <
        getNextElement();
        string right = additive();
        string temp = semantic->newTempVar();
        semantic->genComparison("<", left, right, temp);
        left = temp;
    }
    else if (checkElement(4, 10)) {   // >
        getNextElement();
        string right = additive();
        string temp = semantic->newTempVar();
        semantic->genComparison(">", left, right, temp);
        left = temp;
    }
    else if (checkElement(4, 11)) {   // <=
        getNextElement();
        string right = additive();
        string temp = semantic->newTempVar();
        semantic->genComparison("<=", left, right, temp);
        left = temp;
    }
    else if (checkElement(4, 12)) {   // >=
        getNextElement();
        string right = additive();
        string temp = semantic->newTempVar();
        semantic->genComparison(">=", left, right, temp);
        left = temp;
    }
    else if (checkElement(4, 7)) {    // ==
        getNextElement();
        string right = additive();
        string temp = semantic->newTempVar();
        semantic->genComparison("==", left, right, temp);
        left = temp;
    }
    else if (checkElement(4, 8)) {    // !=
        getNextElement();
        string right = additive();
        string temp = semantic->newTempVar();
        semantic->genComparison("!=", left, right, temp);
        left = temp;
    }

    return left;
}

string Parser::additive() {
    string left = multiplicative();

    while (true) {
        if (checkElement(4, 1)) {   // +
            getNextElement();
            string right = multiplicative();
            string temp = semantic->newTempVar();
            semantic->genBinaryOp("+", left, right, temp);
            left = temp;
        }
        else if (checkElement(4, 2)) {   // -
            getNextElement();
            string right = multiplicative();
            string temp = semantic->newTempVar();
            semantic->genBinaryOp("-", left, right, temp);
            left = temp;
        }
        else {
            break;
        }
    }

    return left;
}

string Parser::multiplicative() {
    string left = unary();

    while (true) {
        if (checkElement(4, 3)) {   // *
            getNextElement();
            string right = unary();
            string temp = semantic->newTempVar();
            semantic->genBinaryOp("*", left, right, temp);
            left = temp;
        }
        else if (checkElement(4, 4)) {   // /
            getNextElement();
            string right = unary();
            string temp = semantic->newTempVar();
            semantic->genBinaryOp("/", left, right, temp);
            left = temp;
        }
        else if (checkElement(4, 5)) {   // %
            getNextElement();
            string right = unary();
            string temp = semantic->newTempVar();
            semantic->genBinaryOp("%", left, right, temp);
            left = temp;
        }
        else {
            break;
        }
    }

    return left;
}

string Parser::unary() {
    if (checkElement(4, 15)) {   // !
        getNextElement();
        string operand = unary();
        string temp = semantic->newTempVar();
        semantic->genBinaryOp("!", operand, "", temp);
        return temp;
    }
    return primary();
}

string Parser::primary() {
    if (checkElement(1)) {
        string name = getElementValue();

        if (!semantic->isDeclared(name)) {
            syntaxError("Переменная не объявлена: " + name);
        }

        getNextElement();
        return name;
    }
    else if (checkElement(2)) {
        string constVal = getElementValue();
        getNextElement();
        return constVal;
    }
    else if (checkElement(3, 3)) {
        getNextElement();
        if (!checkElement(4, 16)) {
            syntaxError("Ожидается '('");
        }
        getNextElement();
        if (!checkElement(4, 17)) {
            syntaxError("Ожидается ')'");
        }
        getNextElement();

        string temp = semantic->newTempVar();
        semantic->genRead(temp);
        return temp;
    }
    else if (checkElement(4, 16)) {
        getNextElement();
        string expr = expression();
        if (!checkElement(4, 17)) {
            syntaxError("Ожидается ')'");
        }
        getNextElement();
        return expr;
    }

    syntaxError("Ожидается идентификатор, константа, read() или выражение в скобках");
    return "";
}

bool Parser::run() {
    try {
        program();
        return true;
    }
    catch (...) {
        return false;
    }
}

void Parser::saveToFile(const string& filename) {
    outputFile.open(filename);
    if (!outputFile.is_open()) return;

    outputFile << "Синтаксический анализатор" << endl;
    outputFile << "Синтаксических ошибок не обнаружено" << endl;
    outputFile << "Метод: рекурсивный спуск" << endl;

    outputFile.close();
}
