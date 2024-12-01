/*
 * File: Basic.cpp
 * ---------------
 * This file is the starter project for the BASIC interpreter.
 */

#include <cctype>
#include <iostream>
#include <memory>
#include <string>
#include "exp.hpp"
#include "parser.hpp"
#include "program.hpp"
#include "statement.hpp"
#include "Utils/error.hpp"
#include "Utils/tokenScanner.hpp"
#include "Utils/strlib.hpp"
#pragma GCC optimize(2)


/* Function prototypes */

void processLine(std::string line, Program &program, EvalState &state);
Statement* parseClause(std::string line, bool singleLine=false);
bool validVarName(const std::string &);

/* Main program */

int main() {
    EvalState state;
    Program program;
    //cout << "Stub implementation of BASIC" << endl;
    while (true) {
        try {
            std::string input;
            getline(std::cin, input);
            if (input.empty())
                return 0;
            TokenScanner scanner(input);
            std::string f = scanner.nextToken();
            if (scanner.getTokenType(f) == NUMBER) {
                if (!scanner.hasMoreTokens()) {
                    program.removeSourceLine(stringToInteger(f));
                    continue;
                }
                program.addSourceLine(stringToInteger(f), input);
                continue;
            }
            if (! (scanner.getTokenType(f)==WORD)) {
                throw ErrorException("SYNTAX ERROR");
            }
            if (f == "RUN") {
                if (program.clauses.empty()) {
                    continue;
                }
                for (auto it = program.clauses.begin(); it != program.clauses.end(); it++) {
                    program.setParsedStatement(it->first, parseClause(it->second.source));
                }
                int line = program.getFirstLineNumber();
                Statement *stmt = program.getParsedStatement(line);
                stmt->execute(state, program);
                continue;
            }
            if (f == "LIST") {
                for (auto &it : program.clauses) {
                    std::cout << it.second.source << std::endl;
                }
                continue;
            }
            if (f == "CLEAR") {
                program.clear();
                state.Clear();
                continue;
            }
            if (f == "QUIT") {
                return 0;
            }
            if (f == "HELP") {
                std::cout << "RUN: run the program" << '\n';
                std::cout << "LIST: list the program" << '\n';
                std::cout << "CLEAR: clear the program" << '\n';
                std::cout << "QUIT: quit the program" << '\n';
                std::cout << "HELP: print this help message" << std::endl;
                continue;
            }
            processLine(input, program, state);
        } catch (ErrorException &ex) {
            std::cout << ex.getMessage() << std::endl;
        }
    }
    return 0;
}

/*
 * Function: processLine
 * Usage: processLine(line, program, state);
 * -----------------------------------------
 * Processes a single line entered by the user.  In this version of
 * implementation, the program reads a line, parses it as an expression,
 * and then prints the result.  In your implementation, you will
 * need to replace this method with one that can respond correctly
 * when the user enters a program line (which begins with a number)
 * or one of the BASIC commands, such as LIST or RUN.
 */

void processLine(std::string line, Program &program, EvalState &state) {
    auto s = parseClause(line, true);
    s->execute(state, program);
    delete s;
}

Statement* parseClause(std::string line, bool singleLine) {
    TokenScanner scanner(line);
    scanner.ignoreWhitespace();
    scanner.scanNumbers();
    if (!singleLine) {
        std::string line = scanner.nextToken();
        if (scanner.getTokenType(line) != NUMBER) {
            throw ErrorException("SYNTAX ERROR");
        }
    }
    std::string token = scanner.nextToken();
    if (scanner.getTokenType(token) != WORD) {
        throw ErrorException("SYNTAX ERROR");
    }
    if (token == "REM") {
        if (singleLine) {
            throw ErrorException("SYNTAX ERROR");
        }
        return new RemStmt(line);
    }
    if (token == "LET") {
        std::string var = scanner.nextToken();
        if ((scanner.getTokenType(var) != WORD && scanner.getTokenType(var) != NUMBER) || !validVarName(var)) {
            throw ErrorException("SYNTAX ERROR");
        }
        std::string op = scanner.nextToken();
        if (op != "=") {
            throw ErrorException("SYNTAX ERROR");
        }
        Expression *exp = nullptr;
        try {
            exp = parseExp(scanner);
            return new LetStmt(var, exp);
        }
        catch (ErrorException &ex) {
            if (exp != nullptr) {
                delete exp;
            }
            throw ErrorException("SYNTAX ERROR");
        }
        
    }
    if (token == "PRINT") {
        Expression *exp = nullptr;
        try {
            exp = parseExp(scanner);
            return new PrintStmt(exp);
        }
        catch (ErrorException &ex) {
            if (exp != nullptr) {
                delete exp;
            }
            throw ErrorException("SYNTAX ERROR");
        }
    }
    if (token == "INPUT") {
        std::string var = scanner.nextToken();
        if ((scanner.getTokenType(var) != WORD && scanner.getTokenType(var) != NUMBER) || !validVarName(var)) {
            throw ErrorException("SYNTAX ERROR");
        }
        if (scanner.hasMoreTokens()) {
            throw ErrorException("SYNTAX ERROR");
        }
        return new InputStmt(var);
    }
    if (token == "GOTO") {
        if (singleLine) {
            throw ErrorException("SYNTAX ERROR");
        }
        std::string line = scanner.nextToken();
        if (scanner.getTokenType(line) != NUMBER) {
            throw ErrorException("SYNTAX ERROR");
        }
        if (scanner.hasMoreTokens()) {
            throw ErrorException("SYNTAX ERROR");
        }
        return new GotoStmt(stringToInteger(line));
    }
    if (token == "IF") {
        if (singleLine) {
            throw ErrorException("SYNTAX ERROR");
        }
        int i = line.find("IF");
        int equal = line.find("=");
        if (equal == std::string::npos) {
            equal = line.find("<");
        }
        if (equal == std::string::npos) {
            equal = line.find(">");
        }
        if (equal == std::string::npos) {
            throw ErrorException("SYNTAX ERROR");
        }
        int then_ = line.find("THEN", equal);
        if (then_ == std::string::npos) {
            throw ErrorException("SYNTAX ERROR");
        }
        std::string left = line.substr(i+2, equal-i-2);
        std::string right = line.substr(equal+1, then_-equal-1);
        std::string then = line.substr(then_+4);
        auto l = TokenScanner(left);
        auto r = TokenScanner(right);
        auto t = TokenScanner(then);
        l.ignoreWhitespace();
        l.scanNumbers();
        r.ignoreWhitespace();
        r.scanNumbers();
        t.ignoreWhitespace();
        t.scanNumbers();
        Expression *exp1 = nullptr;
        Expression *exp2 = nullptr;
        try {
            exp1 = parseExp(l);
            exp2 = parseExp(r);
        }
        catch (ErrorException &ex) {
            if (exp1 != nullptr) {
                delete exp1;
            }
            if (exp2 != nullptr) {
                delete exp2;
            }
            throw ErrorException("SYNTAX ERROR");
        }
        std::string lineNum = t.nextToken();
        if (t.getTokenType(lineNum) != NUMBER) {
            throw ErrorException("SYNTAX ERROR");
        }
        if (t.hasMoreTokens()) {
            throw ErrorException("SYNTAX ERROR");
        }
        return new IfStmt(exp1, line[equal], exp2, stringToInteger(lineNum));
    }
    if (token == "END") {
        if (singleLine) {
            throw ErrorException("SYNTAX ERROR");
        }
        if (scanner.hasMoreTokens()) {
            throw ErrorException("SYNTAX ERROR");
        }
        return new EndStmt();
    }
    throw ErrorException("SYNTAX ERROR");
}

bool validVarName(const std::string &n) {
    if (n != "LET" && n != "PRINT" && n != "INPUT" && n != "IF" && n != "THEN" && n != "GOTO" && n != "REM" && n != "END" && n != "RUN" && n != "LIST" && n != "CLEAR" && n != "QUIT" && n != "HELP") {
        return true;
    }
    return false;
}