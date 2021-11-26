#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <locale.h>


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
        	cout<<"aa"<<endl;
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

void conteo2(vector<Token> tokens){
	
	int  WHITESPACEnum = 0, KEYWORDnum = 0, IDENTIFIERnum = 0, INTERGER_LITERALnum = 0,  DOUBLE_LITERALnum = 0;
	
	int STRING_LITERALnum = 0, OPERATORnum = 0, ESCAPE_SEQnum = 0, POTENTIAL_DOUBLEnum = 0;
    
   
	for (Token t : tokens) {
	
		
		switch(t.mType){
			
			case KEYWORD:
				KEYWORDnum++;
				break;
			case IDENTIFIER:
				IDENTIFIERnum++;
				break;
			case INTERGER_LITERAL:
				INTERGER_LITERALnum++;
				break;
			case DOUBLE_LITERAL:
				DOUBLE_LITERALnum++;
				break;
			case STRING_LITERAL:
				STRING_LITERALnum++;
				break;
			case OPERATOR:
				OPERATORnum++;
				break;
				
			
		}
		
		
	 }
	 
	 cout<<"Cantidad de 'KeyWord' --->"<<KEYWORDnum<<endl;
	 cout<<"Cantidad de 'Identifiers' --->"<<IDENTIFIERnum<<endl;
	 cout<<"Cantidad de 'Integer_Literal' --->"<<INTERGER_LITERALnum<<endl;
	 cout<<"Cantidad de 'Double_Literal' --->"<<DOUBLE_LITERALnum<<endl;
	 cout<<"Cantidad de 'String_Literal' --->"<<STRING_LITERALnum<<endl;
	 cout<<"Cantidad de 'Operator' --->"<<OPERATORnum<<endl;
	
}

void conteo(vector<Token> tokens){
	
	int  WHITESPACEnum = 0, KEYWORDnum = 0, IDENTIFIERnum = 0, INTERGER_LITERALnum = 0,  DOUBLE_LITERALnum = 0;
	
	int STRING_LITERALnum = 0, OPERATORnum = 0, ESCAPE_SEQnum = 0, POTENTIAL_DOUBLEnum = 0;
    
   
	for (Token t : tokens) {
	
		
		switch(t.mType){
			
			case KEYWORD:
				KEYWORDnum++;
				break;
			case IDENTIFIER:
				IDENTIFIERnum++;
				break;
			case INTERGER_LITERAL:
				INTERGER_LITERALnum++;
				break;
			case DOUBLE_LITERAL:
				DOUBLE_LITERALnum++;
				break;
			case STRING_LITERAL:
				STRING_LITERALnum++;
				break;
			case OPERATOR:
				OPERATORnum++;
				break;		
		}	
	 }
	 cout<<"++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	 cout<<"+         TIPO                     CANTIDAD  +"<<endl;
	 cout<<"+        KeyWord                       "<<KEYWORDnum<<"     +"<<endl;
	 cout<<"+        Identifiers                   "<<IDENTIFIERnum<<"    +"<<endl;
	 cout<<"+        Integer_Literal               "<<INTERGER_LITERALnum<<"     +"<<endl;
	 cout<<"+        Double_Literal                "<<DOUBLE_LITERALnum<<"     +"<<endl;
	 cout<<"+        String_Literal                "<<STRING_LITERALnum<<"     +"<<endl;
	 cout<<"+        Operator                      "<<OPERATORnum<<"    +"<<endl;
	 cout<<"++++++++++++++++++++++++++++++++++++++++++++++"<<endl;	
}

void archivo(vector<Token> tokens){
	
	int  WHITESPACEnum = 0, KEYWORDnum = 0, IDENTIFIERnum = 0, INTERGER_LITERALnum = 0,  DOUBLE_LITERALnum = 0;
	
	int STRING_LITERALnum = 0, OPERATORnum = 0, ESCAPE_SEQnum = 0, POTENTIAL_DOUBLEnum = 0;
    
   
	for (Token t : tokens) {
	
		
		switch(t.mType){
			
			case KEYWORD:
				KEYWORDnum++;
				break;
			case IDENTIFIER:
				IDENTIFIERnum++;
				break;
			case INTERGER_LITERAL:
				INTERGER_LITERALnum++;
				break;
			case DOUBLE_LITERAL:
				DOUBLE_LITERALnum++;
				break;
			case STRING_LITERAL:
				STRING_LITERALnum++;
				break;
			case OPERATOR:
				OPERATORnum++;
				break;		
		}	
	 }
	 ofstream file;
	 file.open("OutPut.txt");
	 file<<"++++++++++++++++++++++++++++++++++++++++++++++\n"<<endl;
	 file<<"+         TIPO                     CANTIDAD  +\n"<<endl;
	 file<<"+        KeyWord                       "<<KEYWORDnum<<"     +\n"<<endl;
	 file<<"+        Identifiers                   "<<IDENTIFIERnum<<"    +\n"<<endl;
	 file<<"+        Integer_Literal               "<<INTERGER_LITERALnum<<"     +\n"<<endl;
	 file<<"+        Double_Literal                "<<DOUBLE_LITERALnum<<"     +\n"<<endl;
	 file<<"+        String_Literal                "<<STRING_LITERALnum<<"     +\n"<<endl;
	 file<<"+        Operator                      "<<OPERATORnum<<"    +\n"<<endl;
	 file<<"++++++++++++++++++++++++++++++++++++++++++++++\n"<<endl;
	 
	  file<< "Token \t CODE \n";
			    for (Token t : tokens) {
			        file<< t.mText << "\t" << t.mType << "\n";
			    }
	 
	 file<<"Cantidad de 'KeyWord' --->"<<KEYWORDnum<<"\n"<<endl;
	 file<<"Cantidad de 'Identifiers' --->"<<IDENTIFIERnum<<"\n"<<endl;
	 file<<"Cantidad de 'Integer_Literal' --->"<<INTERGER_LITERALnum<<"\n"<<endl;
	 file<<"Cantidad de 'Double_Literal' --->"<<DOUBLE_LITERALnum<<"\n"<<endl;
	 file<<"Cantidad de 'String_Literal' --->"<<STRING_LITERALnum<<"\n"<<endl;
	 file<<"Cantidad de 'Operator' --->"<<OPERATORnum<<"\n"<<endl;
	 
	 
	 file.close();	
}

int main(int argc, char *argv[]) {
	
	setlocale(LC_CTYPE, "Spanish");
	int op;
	string path, buf;
	vector<Token> tokens;
	bool seguir = true;
	
	do{
		system("cls");
			
		cout<<"Menú principal"<<endl;
		cout<<"1. Leer archivo"<<endl;
		cout<<"2. Mostrar datos clasificados"<<endl;
		cout<<"3. Mostrar tabla de contenido"<<endl;
		cout<<"4. Guardar Datos En un Archivo .txt"<<endl;
		cout<<"5. Salir"<<endl;
		cout<<"Ingrese la opción"<<endl;
		cin>>op;
		
		switch(op) {
			case 1:
				system("cls");
				path = argv[1];
    			buf = readFile(path);
    			tokens = parse(buf);
    			cout<<"> Archivo "<<path<<" leído correctamente\n"<<endl;
    			printFile(buf);
				system("pause");
				break;
				
			case 2:
				system("cls");
				cout << "Token \t CODE \n";
			    for (Token t : tokens) {
			        cout << t.mText << "\t" << t.mType << "\n";
			    }
			    conteo2(tokens);
				system("pause");
				break;
				
			case 3:
				system("cls");
				conteo(tokens);
				system("pause");
				break;
				
			case 4:
				system("cls");
				archivo(tokens);
				cout<<"Archivo Creado Satisfactoriamente"<<endl;
				system("pause");
				break;
				
				
			case 5:
				system("cls");
				seguir = false;
				cout<<"El programa ha finalizado"<<endl;
				cout<<"\nDesarrollado por:"<<endl;
				cout<<" - Juan Sierra\n - Juan Bueno\n - Carlos Plaza\n - Santiago Gutiérrez\n - Diego Ochoa\n - Daniel Hilarión"<<endl;
				break;
				
				
			default:
				system("cls");
				cout<<"Revise que lo ingresado sea uno de los números de las opciones del menú\n"<<endl;
				system("pause");
				break;
		}
	}while (seguir);
	
    return 0;
}
