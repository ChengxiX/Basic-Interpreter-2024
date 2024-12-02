#include "Utils/tokenScanner.hpp"

class myTokenScanner : public TokenScanner {
public:
    TokenType getTokenType(std::string token) const {
        if (token == "") return TokenType(EOF);
        char ch = token[0];
        if (isspace(ch)) return SEPARATOR;
        if (ch == '"' || (ch == '\'' && token.length() > 1)) return STRING;
        bool flag = true;
        for (int i = 0; i < token.length(); i++) {
            if (!isdigit(token[i])) flag = false;
        }
        if (flag) return NUMBER;
        if (isWordCharacter(ch)) return WORD;
        return OPERATOR;
    }
};