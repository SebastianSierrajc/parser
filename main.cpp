#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

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
};

class Token {
   public:
    enum TokenType mType { WHITESPACE };
    string mText;
    size_t mStartOffset;
    size_t mEndOffset{0};
    size_t mLineNumber{0};
};

void endToken(Token &token, vector<Token> &tokens) {
    if (token.mType != WHITESPACE) {
        tokens.push_back(token);
    } else if (token.mType == POTENTIAL_DOUBLE) {
        if (token.mText.compare(".") == 0) {
            token.mType = OPERATOR;
        } else {
            token.mType = DOUBLE_LITERAL;
        }
        tokens.push_back(token);
    } else if (token.mType == IDENTIFIER) {
        if (keywords.find(token.mText) != keywords.end()) {
            token.mType = KEYWORD;
        }
    }

    token.mType = WHITESPACE;
    token.mText.erase();
}

void escapeSeq(char c, Token &token) {
    switch (c) {
        case 'n':
            token.mText.push_back('\n');
            break;
        case 'r':
            token.mText.push_back('\r');
            break;
        case 't':
            token.mText.push_back('\t');
            break;
        case '\\':
            token.mText.push_back('\\');
            break;

        default:
            throw runtime_error(string("Secuencia de escape desconocida: \\") +
                                string(1, c) +
                                string("en el string de la linea ") +
                                to_string(token.mLineNumber + 1) + ".");
    }

    token.mType = STRING_LITERAL;
}

vector<Token> parse(const string &inProgram) {
    vector<Token> tokens;

    Token currentToken;
    currentToken.mLineNumber = 1;

    for (char currCh : inProgram) {
        if (currentToken.mType == ESCAPE_SEQ) {
            cout << currentToken.mText << " " << currCh << "\n";
            escapeSeq(currCh, currentToken);
            continue;
        }

        switch (currCh) {
            case '1' ... '9':  // literales numericos enteros
                if (currentToken.mType == WHITESPACE) {
                    currentToken.mType = INTERGER_LITERAL;
                } else if (currentToken.mType == POTENTIAL_DOUBLE) {
                    currentToken.mType = DOUBLE_LITERAL;
                }
                currentToken.mText.push_back(currCh);
                break;
            case '.':  // literales numricos flotantes
                if (currentToken.mType == WHITESPACE) {
                    currentToken.mType = POTENTIAL_DOUBLE;
                    currentToken.mText.push_back(currCh);
                } else if (currentToken.mType == INTERGER_LITERAL) {
                    currentToken.mType = DOUBLE_LITERAL;
                    currentToken.mText.push_back(currCh);
                } else if (currentToken.mType == STRING_LITERAL) {
                    currentToken.mText.push_back(currCh);
                } else {
                    endToken(currentToken, tokens);
                    currentToken.mType = OPERATOR;
                    currentToken.mText.push_back(currCh);
                    endToken(currentToken, tokens);
                }

                break;
            case '"':  // literales string
                if (currentToken.mType != STRING_LITERAL) {
                    endToken(currentToken, tokens);
                    currentToken.mType = STRING_LITERAL;
                    currentToken.mText.push_back(currCh);
                } else if (currentToken.mType == STRING_LITERAL) {
                    currentToken.mText.push_back(currCh);
                    endToken(currentToken, tokens);
                }
                break;

            case '!':
            case '#' ... '-':  // puntuadores
            case '/':
            case ':' ... '?':  // operadores
            case '[':
            case ']':
            case '{' ... '~':
                if (currentToken.mType != STRING_LITERAL) {
                    endToken(currentToken, tokens);
                    currentToken.mType = OPERATOR;
                    currentToken.mText.push_back(currCh);
                    endToken(currentToken, tokens);
                } else {
                    currentToken.mText.push_back(currCh);
                }
                break;
            case ' ':  // espacios en blanco
            case '\t':
                if (currentToken.mType == STRING_LITERAL) {
                    currentToken.mText.push_back(currCh);
                } else {
                    endToken(currentToken, tokens);
                }
                break;
            case '\r':  // saltos de linea
            case '\n':
                currentToken.mLineNumber++;
                break;

            case '\\':
                if (currentToken.mType == STRING_LITERAL) {
                    currentToken.mType = ESCAPE_SEQ;
                } else {
                    endToken(currentToken, tokens);
                    currentToken.mType = OPERATOR;
                    currentToken.mText.push_back(currCh);
                    endToken(currentToken, tokens);
                }
                break;
            default:
                if (currentToken.mType == WHITESPACE ||
                    currentToken.mType == INTERGER_LITERAL ||
                    currentToken.mType == DOUBLE_LITERAL) {
                    endToken(currentToken, tokens);
                    currentToken.mType = IDENTIFIER;
                    currentToken.mText.push_back(currCh);
                } else {
                    currentToken.mText.push_back(currCh);
                }
                break;
        }
    }

    endToken(currentToken, tokens);

    return tokens;
}

string readFile(string path) {
    ifstream file(path);
    stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void printFile(string ss) { cout << ss; }

int main(int argc, char *argv[]) {
    string path = argv[1];
    string buf;
    vector<Token> tokens;
    buf = readFile(path);
    tokens = parse(buf);

    printFile(buf);

    cout << "Token \t CODE \n";
    for (Token t : tokens) {
        cout << t.mText << "\t " << t.mType << "\n";
    }

    return 0;
}