#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Tokenizer.hpp"

using namespace std;
using namespace simpleparser;

string readFile(string path) {
    ifstream file(path);
    stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

void printFile(string ss) { cout << ss; }

int main(int argc, char *argv[]) {
    Tokenizer tokenizer;
    string path, buf;
    path = argv[1];
    buf = readFile(path);

    vector<Token> tokens = tokenizer.parse(buf);

    for (Token token : tokens) {
        token.debugPrint();
    }

    return 0;
}
