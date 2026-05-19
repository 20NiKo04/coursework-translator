#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "interpreter.h"
#include <clocale>
#include <iostream>

using namespace std;

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    string filename = "input.txt";

    cout << "Лексический анализ" << endl;
    Lexer lexer(filename);
    lexer.getNextToken();
    lexer.printOutputToFile("lexer_output.txt");
    cout << "Результат лексического анализа сохранен в файл lexer_output.txt" << endl << endl;

    cout << "Синтаксический и Сематический анализ" << endl;
    SemanticAnalyzer semantic;
    Parser parser(lexer, semantic);

    if (parser.run()) {
        cout << "Синтаксических и семантических ошибок не обнаружено" << endl;
        parser.saveToFile("parser_output.txt");
        semantic.saveToFile("semantic_output.txt");
        cout << "Результат синтаксического анализа сохранен в файл parser_output.txt" << endl;
        cout << "Результат семантического анализа (таблица символов и тетрады) сохранен в файл semantic_output.txt" << endl << endl;

        Interpreter interpreter(semantic.getTetrads());
        interpreter.run();
        interpreter.saveToFile("interpreter_output.txt");
        cout << "Результат интерпретации сохранен в файл interpreter_output.txt" << endl;

    }
    else {
        cout << "Обнаружены ошибки" << endl;
    }

    cout << "\nНажмите Enter для выхода...";
    cin.get();
    cin.get();

    return 0;
}