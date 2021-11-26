#include <locale.h>

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
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
    POTENTIAL_COMMENT,
    COMMENT
};

class Token {
   public:
    enum TokenType mType { WHITESPACE };
    string mText;
    size_t mLineNumber{0};
};

void endToken(Token &token, vector<Token> &tokens) {
    if (token.mType != WHITESPACE || token.mType != COMMENT) {
        tokens.push_back(token);
    }

    if (token.mType == POTENTIAL_DOUBLE) {
        if (token.mText.compare(".") == 0) {
            token.mType = OPERATOR;
        } else {
            token.mType = DOUBLE_LITERAL;
        }
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
        } else if (currentToken.mType == POTENTIAL_COMMENT && currCh != '/') {
            currentToken.mType = OPERATOR;
            endToken(currentToken, tokens);
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

            case '/':  // comentarios
                if (currentToken.mType == STRING_LITERAL) {
                    currentToken.mText.push_back(currCh);
                } else if (currentToken.mType == POTENTIAL_COMMENT) {
                    currentToken.mType == COMMENT;
                    currentToken.mText.erase();
                } else {
                    currentToken.mType = POTENTIAL_COMMENT;
                    currentToken.mText.push_back(currCh);
                }
                break;

            case '!':
            case '#' ... '-':  // puntuadores
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
    if (file.is_open()) {
        stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    } else {
        return "er";
    }
}

void printFile(string ss) { cout << ss; }

int main(int argc, char *argv[]) {
    setlocale(LC_CTYPE, "Spanish");

    if (argv[1] == NULL) {
        cout << "Recuerda que el nombre del archivo lo debes pasar como "
                "par�metro"
             << endl;
        return -1;
    }

    int op;
    string path, buf;
    vector<Token> tokens;
    bool seguir = true;

    do {
        system("cls");

        cout << "Men� principal" << endl;
        cout << "1. Leer archivo" << endl;
        cout << "2. Mostrar datos clasificados" << endl;
        cout << "3. Mostrar tabla de contenido" << endl;
        cout << "4. Salir" << endl;
        cout << "Ingrese la opci�n" << endl;
        cin >> op;

        switch (op) {
            case 1:
                system("cls");
                path = argv[1];
                buf = readFile(path);
                if (buf != "er") {
                    tokens = parse(buf);
                    cout << "> Archivo " << path << " le�do correctamente\n"
                         << endl;
                    printFile(buf);
                    system("pause");
                } else {
                    cout << "No se ha encontrado el archivo " << path
                         << ", revise el argumento ingresado y vuelva a correr "
                            "el programa\n"
                         << endl;
                    system("pause");
                }

                break;

            case 2:
                system("cls");
                if (tokens.empty()) {
                    cout << "No se ha le�do ning�n archivo por el momento\n"
                         << endl;
                } else {
                    cout << "Token \t CODE \n";
                    for (Token t : tokens) {
                        cout << t.mText << "\t" << t.mType << "\n";
                    }
                }
                system("pause");
                break;

            case 3:
                system("cls");
                if (tokens.empty()) {
                    cout << "No se ha le�do ning�n archivo por el momento\n"
                         << endl;
                } else {
                    cout << "Tablita" << endl;
                }
                system("pause");
                break;

            case 4:
                system("cls");
                seguir = false;
                cout << "El programa ha finalizado" << endl;
                cout << "\nDesarrollado por:" << endl;
                cout << " - Juan Sierra\n - Juan Bueno\n - Carlos Plaza\n - "
                        "Santiago Guti�rrez\n - Diego Ochoa\n - Daniel Hilari�n"
                     << endl;
                break;

            default:
                system("cls");
                cout << "Revise que lo ingresado sea uno de los n�meros de las "
                        "opciones del men�\n"
                     << endl;
                system("pause");
                break;
        }
    } while (seguir);

    return 0;
}
