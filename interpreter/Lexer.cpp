/**
 * Implementation of the 'Lexer' class.
 */

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "interpreter/Lexer.h"
#include "interpreter/LexerException.h"

using namespace std;

/**
 * Get the next character from the buffer.
 */
char Lexer::gchar() {
    char c;
    if (bufcount >= buflen)
        return 0;

    c = buffer[bufcount++];

    switch (c) {
        case '#':   // Comment
            while (
                bufcount < buflen &&
                (c=buffer[bufcount++])!='\n'
                //(c=buffer[bufcount++])!=0 &&
                //c != '\n'
            ) continue;

            if (c == 0)
                break;
        case '\n':
            line++;
            charpos = 0;
        case '\t':
        case '\r':
            c = ' ';
            break;
        case '/':   // Multiline comment
            if (bufcount < buflen &&
                buffer[bufcount] == '*') {
                bufcount++;
                while (bufcount < buflen) {
                    if ((c=buffer[bufcount++]) == '*' &&
                        bufcount < buflen &&
                        (c=buffer[bufcount]) == '/') {
                        bufcount++;
                        break;
                    } else if (c == '\n') {
                        line++;
                        charpos = 0;
                    }

                    charpos++;
                }

                return gchar();
            }
        default: break;
    }

    charpos++;
    return c;
}

/**
 * Get the current character in the buffer.
 */
char Lexer::cchar() {
    if (bufcount == 0)
        return gchar();
    else {
        char c = buffer[bufcount-1];
        switch (c) {
            case '\n':
            case '\t':
            case '\r':
                return ' ';
            default:
                return c;
        }
    }
}

/**
 * Peek at the next character in the
 * stream, without modifying the lexer state.
 * (NOTE: Does _not_ ignore whitespace, comments etc.)
 */
char Lexer::peek() const {
    if (bufcount >= buflen)
        return 0;
    else
        return buffer[bufcount];
}

/**
 * Check if the given character is an operator.
 */
ttype Lexer::is_operator(const char c) const {
    switch (c) {
        case '+': return token::PLUS;
        case '-': return token::MINUS;
        case '*': return token::MULTIPLICATION;
        case '=': return token::EQUALS;
        case '(': return token::LPAR;
        case ')': return token::RPAR;
        case ':': return token::ASSIGN;
        case ';': return token::ENDSTATEMENT;
        default:  return token::UNKNOWN;
    }
}

/**
 * Return the next token from the buffer.
 */
token *Lexer::next() {
    char c = cchar();
    token *tkn = new token;

    tkn->filename = filename;
    tkn->charpos = charpos;
    tkn->line = line;

    // An 'accidental' null-character may occur
    // if we've switched back to the original buffer
    // after including a file. If so, reread the
    // buffer. If we've truly reached the end of
    // this buffer, we will get another null char.
    /*if (c == 0) {
        c = gchar();
        if (c == 0) {
            tkn->type = token::ENDOFSTREAM;
            tkn->text.clear();
            return tkn;
        }
    }*/

    do {
        if (c == ' ') {
            if (tkn->text.length() > 0)
                break;
            else continue;
        } else if (c == '"') {  // Beginning of string
            if (tkn->text.length() > 0)
                break;
            
            while (bufcount < buflen && buffer[bufcount] != '"') {
                tkn->text += buffer[bufcount++];
            }

            // Skip '"'
            bufcount++;
            gchar();

            if (tkn->text.length() > 0) {
                tkn->type = token::WORD;
                return tkn;
            }
        } else if ((tkn->type=is_operator(c))!=token::UNKNOWN) {
            if (tkn->text.length() > 0)
                break;
            else if (tkn->type == token::MINUS && peek()=='>') {
                gchar();
                tkn->type = token::RARROW;
                tkn->text = "->";
            } else tkn->text = c;

            c = gchar();
            return tkn;
        } else
            tkn->text += c;
    } while ((c=gchar())!=0);

    if (tkn->text.empty()) {    // End-of-stream
        delete tkn;
        return nullptr;
    } else {
        if (tkn->text[0] == '$') {
            tkn->type = token::REFERENCE;
            tkn->text.erase(0,1);
        } else if (tkn->text.front() == '[' && tkn->text.back() == ']') {
            if (tkn->text.length() == 2)
                Error("Label is empty.");
            tkn->type = token::LABEL;
            tkn->text = tkn->text.substr(1, tkn->text.length()-2);
        } else if (tkn->text == "apply") {
            tkn->type = token::APPLY;
        } else if (tkn->text == "assert") {
            tkn->type = token::ASSERT;
        } else if (tkn->text == "by") {
            tkn->type = token::BY;
        } else if (tkn->text == "define") {
            tkn->type = token::DEFINE;
        } else if (tkn->text == "end") {
            tkn->type = token::END;
        } else if (tkn->text == "eval") {
            tkn->type = token::EVAL;
        } else if (tkn->text == "group") {
            tkn->type = token::GROUP;
        } else if (tkn->text == "in") {
            tkn->type = token::IN;
        } else if (tkn->text == "include") {
            tkn->type = token::INCLUDE;
        } else if (tkn->text == "other") {
            tkn->type =token::OTHER;
        } else if (tkn->text == "print") {
            tkn->type = token::PRINT;
        } else if (tkn->text == "printf") {
            tkn->type = token::PRINTF;
        } else if (tkn->text == "printn") {
            tkn->type = token::PRINTN;
        } else if (tkn->text == "replace") {
            tkn->type = token::REPLACE;
        } else if (tkn->text == "to") {
            tkn->type = token::TO;
        } else if (tkn->text == "with") {
            tkn->type = token::WITH;
        } else {
            tkn->type = token::WORD;
        }

        return tkn;
    }
}

/**
 * Throw a LexerException.
 */
void Lexer::Error(const string &msg) {
    throw LexerException("%s: line %d:%d: %s", filename.c_str(), line, charpos, msg.c_str());
}

/**
 * Include the file with the given name.
 */
vector<token*> *Lexer::IncludeFile(const string &fname, vector<token*> *tokens) {
    string
        _buffer = buffer,
        _filename = filename;
    long
        _buflen = buflen,
        _bufcount = bufcount,
        _line = line,
        _charpos = charpos;

    tokens = LexFile(fname, tokens);

    charpos = _charpos;
    line = _line;
    bufcount = _bufcount;
    buflen = _buflen;
    filename = _filename;
    buffer = _buffer;

    return tokens;
}

/**
 * Conduct lexical analysis on the contents of
 * the file with the given name.
 */
vector<token*> *Lexer::LexFile(const string &filename, vector<token*> *tokens) {
    ifstream cfile(filename);
    stringstream bfr;

    if (!cfile.is_open())
        throw LexerException("Unable to open file: %s.", filename.c_str());
    
    bfr << cfile.rdbuf();
    cfile.close();

    string contents = bfr.str();
    return Lex(contents, filename, tokens);
}

/**
 * Conduct lexical analysis on the given string.
 *
 * contents: Source code to lex.
 * fname:    Name of file associated with the source code.
 */
vector<token*> *Lexer::Lex(string &contents, const string &fname, vector<token*> *tokens) {
    buffer = contents;
    buflen = buffer.length();
    bufcount = 0;
    line = 1;
    charpos = 1;

    filename = fname;

    token *tkn;
    if (tokens == nullptr)
        tokens = new vector<token*>();

    while ((tkn=next())!=nullptr) {
        if (tkn->type == token::INCLUDE) {
            tkn = next();
            if (tkn->type != token::WORD)
                Error("Expected name of file after INCLUDE.");

            IncludeFile(tkn->text, tokens);
        } else
            tokens->push_back(tkn);
    }

    return tokens;
}

