#ifndef TETRAD_H
#define TETRAD_H

#include <string>
#include <vector>

enum class TetradOp {
    ADD, SUB, MUL, DIV, MOD,
    ASSIGN,
    LESS,
    AND, OR, NOT,
    READ, WRITE,
    LABEL, JMP, JF,
    NOP
};

struct Tetrad {
    int index;             
    TetradOp op;           
    std::string arg1;       
    std::string arg2;       
    std::string result;    

    Tetrad(int idx, TetradOp op, const std::string& a1 = "",
        const std::string& a2 = "", const std::string& res = "")
        : index(idx), op(op), arg1(a1), arg2(a2), result(res) {
    }

    std::string opToString() const;
    std::string toString() const;
};

using TetradList = std::vector<Tetrad>;

#endif 