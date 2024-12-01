/*
 * File: statement.cpp
 * -------------------
 * This file implements the constructor and destructor for
 * the Statement class itself.  Your implementation must do
 * the same for the subclasses you define for each of the
 * BASIC statements.
 */

#include "statement.hpp"
#include "control.cpp"


/* Implementation of the Statement class */

int stringToInt(std::string str);

Statement::Statement() = default;

Statement::~Statement() = default;

/* Implementation of the RemStmt class */

RemStmt::RemStmt(std::string comment) {}
void RemStmt::execute(EvalState &state, Program &program) {
    nextControl(state, program);
}

/* Implementation of the LetStmt class */
LetStmt::LetStmt(std::string var, Expression *exp) : var(var), exp(exp) {}
LetStmt::~LetStmt() {
    delete exp;
}
void LetStmt::execute(EvalState &state, Program &program) {
    state.setValue(var, exp->eval(state));
    nextControl(state, program);
}

/* Implementation of the PrintStmt class */
PrintStmt::PrintStmt(Expression *exp) : exp(exp) {}
PrintStmt::~PrintStmt() {
    delete exp;
}
void PrintStmt::execute(EvalState &state, Program &program) {
    std::cout << exp->eval(state);
    nextControl(state, program);
}

/* Implementation of the InputStmt class */
InputStmt::InputStmt(std::string var) : var(var) {}
void InputStmt::execute(EvalState &state, Program &program) {
    std::string input;
    std::cin >> input; // 类型？
    state.setValue(var, stringToInt(input));
    nextControl(state, program);
}

/* Implementation of the EndStmt class */
EndStmt::EndStmt() {}
void EndStmt::execute(EvalState &state, Program &program) {
}

/* Implementation of the GotoStmt class */
GotoStmt::GotoStmt(int lineNumber) : lineNumber(lineNumber) {}
void GotoStmt::execute(EvalState &state, Program &program) {
    program.getParsedStatement(lineNumber)->execute(state, program);
    nextControl(state, program, lineNumber);
}

/* Implementation of the IfStmt class */
IfStmt::IfStmt(Expression *exp, char cmp, Expression *exp2, int lineNumber) : exp(exp), lineNumber(lineNumber), cmp(cmp), exp2(exp2) {}

IfStmt::~IfStmt() {
    delete exp;
    delete exp2;
}

void IfStmt::execute(EvalState &state, Program &program) {
    if (cmp == '=') {
        if (exp->eval(state) == exp2->eval(state)) {
            nextControl(state, program, lineNumber);
            return;
        }
        else {
            nextControl(state, program);
            return;
        }
    }
    if (cmp == '<') {
        if (exp->eval(state) < exp2->eval(state)) {
            nextControl(state, program, lineNumber);
            return;
        }
        else {
            nextControl(state, program);
            return;
        }
    }
    if (cmp == '>') {
        if (exp->eval(state) > exp2->eval(state)) {
            nextControl(state, program, lineNumber);
            return;
        }
        else {
            nextControl(state, program);
            return;
        }
    }
}

//todo
