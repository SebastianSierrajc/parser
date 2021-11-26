#pragma once

#include <string>
#include <vector>

namespace simpleparser {

using namespace std;

enum TokenType {
    WHITESPACE,
    IDENTIFIER,
    INTERGER_LITERAL,
    DOUBLE_LITERAL,
    STRING_LITERAL,
    OPERATOR,
    ESCAPE_SEQ,
    POTENTIAL_DOUBLE
};

class Token {
   public:
    enum TokenType mType { WHITESPACE };
    string mText;
    size_t mStartOffset;
    size_t mEndOffset{0};
    size_t mLineNumber{0};
};

class Tokenizer {
   public:
    vector<Token> parse(const string &inProgram);

   private:
    void endToken(Token &token, vector<Token> &tokens);
    void escapeSeq(char c, Token &token);
};

}  // namespace simpleparser
