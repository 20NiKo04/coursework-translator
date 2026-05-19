#include "lexer.h"
#include <iomanip>
#include <iostream>

void Lexer::getNextSymbol() {
    if (file.get(currentSymbol)) {
        currentPos++;
        currentSymbol = tolower(currentSymbol);
    }
    else {
        currentSymbol = '\0';
    }
}

string Lexer::peekNextChar() {
    char next = file.peek();
    return string(1, tolower(next));
}

void Lexer::addAnalyzedInfo(int tableNum, int str) {
    analyzedInfo.push_back(make_pair(tableNum, str));
}

void Lexer::addReadWordTable(string value) {
    static int position = 1;
    for (const auto& element : readWord) {
        if (element.second == value) {
            addAnalyzedInfo(1, element.first);
            return;
        }
    }
    readWord.insert({ position, value });
    addAnalyzedInfo(1, position);
    position++;
}

void Lexer::addReadConstTable(string value) {
    static int position = 1;
    for (const auto& element : readConst) {
        if (element.second == value) {
            addAnalyzedInfo(2, element.first);
            return;
        }
    }
    readConst.insert({ position, value });
    addAnalyzedInfo(2, position);
    position++;
}

void Lexer::addReadKeywordTypeTable(int position, string value) {
    for (const auto& element : readKeywordType) {
        if (element.second == value) {
            addAnalyzedInfo(3, element.first);
            return;
        }
    }
    readKeywordType.insert({ position, value });
    addAnalyzedInfo(3, position);
}

void Lexer::addReadSpecialSymbolTable(int position, string value) {
    for (const auto& element : readSpecialSymbol) {
        if (element.second == value) {
            addAnalyzedInfo(4, element.first);
            return;
        }
    }
    readSpecialSymbol.insert({ position, value });
    addAnalyzedInfo(4, position);
}

void Lexer::addReadErrorSymbolTable(int str, int pos, char value) {
    readErrorSymbol.insert({ {str, pos}, value });
}

bool Lexer::isKeyword(string value) {
    for (const auto& element : keywordType) {
        if (element.second == value) {
            addReadKeywordTypeTable(element.first, value);
            return true;
        }
    }
    return false;
}

bool Lexer::isSpecialSymbol(string value) {
    for (const auto& element : specialSymbol) {
        if (element.second == value) {
            addReadSpecialSymbolTable(element.first, value);
            return true;
        }
    }
    return false;
}

Lexer::Lexer(string filename) {
    file.open(filename);
    if (!file.is_open()) {
        cout << "Файл не найден: " << filename << endl;
        exit(0);
    }
    getNextSymbol();
}

void Lexer::getNextToken() {
    while (currentSymbol != '\0') {
        string result = "";

        if (isspace(currentSymbol) != 0 && currentSymbol != '\n') {
            getNextSymbol();
            continue;
        }

        if (currentSymbol == '\n') {
            currentStr++;
            currentPos = 0;
            getNextSymbol();
            continue;
        }

        if (isdigit(currentSymbol) != 0) {
            while (isdigit(currentSymbol) != 0) {
                result += currentSymbol;
                getNextSymbol();
            }
            addReadConstTable(result);
            continue;
        }

        if (isalpha(currentSymbol) != 0 || currentSymbol == '_') {
            while (isalnum(currentSymbol) != 0 || currentSymbol == '_') {
                result += currentSymbol;
                getNextSymbol();
            }
            if (isKeyword(result)) {
                continue;
            }
            addReadWordTable(result);
            continue;
        }

        string nextChar = peekNextChar();
        string twoChar = string(1, currentSymbol) + nextChar;

        if (twoChar == "==" || twoChar == "!=" || twoChar == "<=" ||
            twoChar == ">=" || twoChar == "&&" || twoChar == "||") {
            if (isSpecialSymbol(twoChar)) {
                getNextSymbol(); 
                getNextSymbol();
                continue;
            }
        }

        string singleChar = string(1, currentSymbol);
        if (isSpecialSymbol(singleChar)) {
            getNextSymbol();
            continue;
        }

        addReadErrorSymbolTable(currentStr, currentPos, currentSymbol);
        getNextSymbol();
    }
}

void Lexer::printReadWord() {
    cout << setw(35) << "1. Таблица идентификаторов\n";
    cout << setw(18) << "Номер" << setw(24) << "Лексема\n";
    for (const auto& element : readWord) {
        cout << setw(10) << element.first << setw(30) << element.second << endl;
    }
    cout << endl;
}

void Lexer::printReadConst() {
    cout << setw(35) << "2. Таблица констант\n";
    cout << setw(18) << "Номер" << setw(15) << "Лексема\n";
    for (const auto& element : readConst) {
        cout << setw(10) << element.first << setw(20) << element.second << endl;
    }
    cout << endl;
}

void Lexer::printReadKeywordType() {
    cout << setw(35) << "3. Таблица ключевых слов\n";
    cout << setw(18) << "Номер" << setw(25) << "Лексема\n";
    for (const auto& element : readKeywordType) {
        cout << setw(10) << element.first << setw(30) << element.second << endl;
    }
    cout << endl;
}

void Lexer::printReadSpecialSymbol() {
    cout << setw(35) << "4. Таблица операторов\n";
    cout << setw(18) << "Номер" << setw(16) << "Лексема\n";
    for (const auto& element : readSpecialSymbol) {
        cout << setw(10) << element.first << setw(20) << element.second << endl;
    }
    cout << endl;
}

void Lexer::printReadErrorSymbol() {
    if (readErrorSymbol.empty()) return;
    cout << setw(35) << "Таблица ошибок\n";
    cout << setw(16) << "Строка" << setw(20) << "Позиция" << setw(18) << "Символ\n";
    for (const auto& element : readErrorSymbol) {
        cout << setw(10) << element.first.first << setw(20)
            << element.first.second << setw(20) << element.second << endl;
    }
    cout << endl;
}

void Lexer::printAnalyzedInfo() {
    cout << setw(30) << "Поток токенов\n";
    cout << setw(17) << "Таблица" << setw(20) << "Номер строки\n";
    for (const auto& element : analyzedInfo) {
        cout << setw(10) << element.first << setw(20) << element.second << endl;
    }
}

void Lexer::printOutputToFile(string filename) {
    ofstream out(filename);
    if (!out.is_open()) {
        cout << "Не удалось открыть файл: " << filename << endl;
        return;
    }
    streambuf* oldCout = cout.rdbuf(out.rdbuf());
    printReadWord();
    printReadConst();
    printReadKeywordType();
    printReadSpecialSymbol();
    printReadErrorSymbol();
    printAnalyzedInfo();
    cout.rdbuf(oldCout);
}

pair<int, int> Lexer::getElAnalyzedInfo(int pos) {
    return analyzedInfo[pos];
}

string Lexer::getElReadWord(int pos) {
    for (const auto& element : readWord) {
        if (element.first == pos) return element.second;
    }
    return "";
}

string Lexer::getElReadConst(int pos) {
    for (const auto& element : readConst) {
        if (element.first == pos) return element.second;
    }
    return "";
}

string Lexer::getElReadKeywordType(int pos) {
    for (const auto& element : readKeywordType) {
        if (element.first == pos) return element.second;
    }
    return "";
}

string Lexer::getElReadSpecialSymbol(int pos) {
    for (const auto& element : readSpecialSymbol) {
        if (element.first == pos) return element.second;
    }
    return "";
}

bool Lexer::isWord(string currentElement) {
    for (const auto& element : readWord) {
        if (element.second == currentElement) return true;
    }
    return false;
}

int Lexer::getSizeAnalyzedInfo() {
    return analyzedInfo.size();
}