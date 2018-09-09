/**
 * Simple test program for symachin.
 */

#include <iostream>
#include "symachin/Expression.h"
#include "symachin/SymachinException.h"
#include "interpreter/Lexer.h"
#include "interpreter/LexerException.h"
#include "interpreter/Parser.h"
#include "interpreter/ParserException.h"

using namespace std;
using namespace symachin;

void lexout(const string &fname) {
    Lexer lex;

    try {
        vector<token*> *tokens = lex.LexFile(fname);
        for (vector<token*>::iterator it = tokens->begin(); it != tokens->end(); it++) {
            token *tkn = *it;
            cout << tkn->ToString() << endl;
        }
    } catch (LexerException &ex) {
        cout << "ERROR: " << ex.whats() << endl;
    }
}

void parse(const string &fname, Parser &p) {
    try {
        p.ParseFile(fname);
    } catch (SymachinException &ex) {
        cout << "ERROR: " << ex.whats() << endl;
    }
}

int main(int argc, char *argv[]) {
    Parser p;

    int i;
    for (i = 1; i < argc; i++) {
        string filename(argv[i]);
        parse(filename, p);
    }

    return 0;
}
