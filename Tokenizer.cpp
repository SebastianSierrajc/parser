#include "Tokenizer.hpp"

#include <stdexcept>

using namespace std;

namespace simpleparser {

using namespace std;

vector<Token> Tokenizer::parse(const string &inProgram) {
    vector<Token> tokens;

    Token currentToken;
    currentToken.mLineNumber = 1;

    for (char currCh : inProgram) {
        if (currentToken.mType == ESCAPE_SEQ) {
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
                } else if (currentToken.mType == STRING_LITERAL) {
                    currentToken.mText.push_back(currCh);
                }
                break;
        }
    }

    endToken(currentToken, tokens);

    return tokens;
}

void Tokenizer::endToken(Token &token, vector<Token> &tokens) {
    if (token.mType != WHITESPACE) {
        tokens.push_back(token);
    } else if (token.mType == POTENTIAL_DOUBLE) {
        if (token.mText.compare(".") == 0) {
            token.mType = OPERATOR;
        } else {
            token.mType = DOUBLE_LITERAL;
        }
        tokens.push_back(token);
    }

    token.mType = WHITESPACE;
    token.mText.erase();
}

void Tokenizer::escapeSeq(char c, Token &token) {
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

}  // namespace simpleparser