/*
 * File: program.cpp
 * -----------------
 * This file is a stub implementation of the program.h interface
 * in which none of the methods do anything beyond returning a
 * value of the correct type.  Your job is to fill in the bodies
 * of each of these methods with an implementation that satisfies
 * the performance guarantees specified in the assignment.
 */

#include "program.hpp"



Program::Program() = default;

Program::~Program() {
    for (auto it = clauses.begin(); it != clauses.end(); it++) {
        delete it->second.stmt;
    }
}

void Program::clear() {
    // Replace this stub with your own code
    //todo
    for (auto it = clauses.begin(); it != clauses.end(); it++) {
        delete it->second.stmt;
    }
    clauses.clear();
}

void Program::addSourceLine(int lineNumber, const std::string &line) {
    // Replace this stub with your own code
    //todo
    if (clauses.find(lineNumber) != clauses.end()) {
        removeSourceLine(lineNumber);
    }
    clauses[lineNumber] = Clause{lineNumber, line, nullptr};
}

void Program::removeSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    delete clauses[lineNumber].stmt;
    clauses.erase(lineNumber);
}

std::string Program::getSourceLine(int lineNumber) {
    // Replace this stub with your own code
    //todo
    return clauses[lineNumber].source;
}

void Program::setParsedStatement(int lineNumber, Statement *stmt) {
    // Replace this stub with your own code
    //todo
    clauses[lineNumber].stmt = stmt;
    stmt->lineNum = lineNumber;
}

//void Program::removeSourceLine(int lineNumber) {

Statement *Program::getParsedStatement(int lineNumber) {
    // Replace this stub with your own code
    //todo
    auto it = clauses.find(lineNumber);
    if (it == clauses.end()) {
        throw ErrorException("LINE NUMBER ERROR");
    }
    return it->second.stmt;
}

int Program::getFirstLineNumber() {
    // Replace this stub with your own code
    //todo
    auto it = clauses.begin();
    return it->first;
}

int Program::getNextLineNumber(int lineNumber) {
    // lineNumber是现在的行号，返回下一行的行号
    // todo
    auto it = clauses.find(lineNumber);
    it++;
    if (it==clauses.end()) {
        return -1;
    }
    return it->first;
}

//more func to add
//todo


