#include "program.hpp"
#include "statement.hpp"
#include "evalstate.hpp"

void nextControl(EvalState &state, Program &program, int lineNumber=-1) {
    if (state.singleStep) {
        return;
    }
    if (lineNumber != -1) {
        state.currentLineNumber = program.getNextLineNumber(lineNumber-1);
    }
    else {
        state.currentLineNumber = program.getNextLineNumber(state.currentLineNumber);
    }
    program.getParsedStatement(state.currentLineNumber)->execute(state, program);
}