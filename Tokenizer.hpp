#pragma once

#include <set>
#include <string>
#include <vector>

namespace simpleparser {

using namespace std;

const set<string> keywords = {  // Tabla de simbolos palabras reservadas
    "asm",         "auto",       "break",
    "case",        "catch",      "char",
    "class",       "const",      "continue",
    "default",     "delete",     "do",
    "double",      "else",       "enum",
    "extern",      "float",      "for",
    "friend",      "goto",       "if",
    "inline",      "int",        "long",
    "new",         "operator",   "private",
    "protected",   "public",     "register",
    "return",      "short",      "signed",
    "sizeof",      "static",     "struct",
    "switch",      "template",   "this",
    "thorw",       "try",        "typedef",
    "union",       "unsigned",   "virtual",
    "void",        "volatile",   "while",
    "bool",        "const_cast", "dynamic_cast",
    "explicit",    "export",     "false",
    "mutable",     "namespace",  "reinterpret_cast",
    "static_cast", "true",       "typeid",
    "typename",    "using",      "wchar_t"};

enum TokenType {
    WHITESPACE,
    KEYWORD,
    IDENTIFIER,
    INTERGER_LITERAL,
    DOUBLE_LITERAL,
    STRING_LITERAL,
    OPERATOR,
    ESCAPE_SEQ,
    POTENTIAL_DOUBLE,
    POTENTIAL_COMMENT,
    COMMENT
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
