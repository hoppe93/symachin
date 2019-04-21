#ifndef _LEXER_H
#define _LEXER_H

#include <string>
#include <vector>

/* Token structure */
typedef struct lex_token {
    std::string text;
    std::string filename;
    long charpos, line;

    /* Token types */
    enum tok_type {
        UNKNOWN,            // Unknown token type
        ENDOFSTREAM,        // End-of-token-stream

        EXPRESSION,         // Expression (containing multiple words and operators)
        WORD,               // A single word
        LABEL,              // [???]
        REFERENCE,          // $???
        RARROW,             // ->
        PLUS,               // +
        MINUS,              // -
        MULTIPLICATION,     // *
        EQUALS,             // =
        LPAR,               // (
        RPAR,               // )
        ASSIGN,             // :
        ENDSTATEMENT,       // ;

        // Keywords
        APPLY,              // apply
        ASSERT,             // assert
        BY,                 // by
        DEFINE,             // define
        END,                // end
        GROUP,              // group
        IN,                 // in
        INCLUDE,            // include
        OTHER,              // other
        PRINT,              // print
        PRINTF,             // printf
        PRINTN,             // printn
        REPLACE,            // replace
        TO                  // to
    };
    tok_type type;

    struct lex_token *Copy() {
        struct lex_token *lt = new struct lex_token;

        lt->text = text;
        lt->filename = filename;
        lt->charpos = charpos;
        lt->line = line;
        lt->type = type;

        return lt;
    };
    std::string ToString() {
        return ToString(type) + "(" + text + ")";
    };
    static std::string ToString(tok_type t) {
        switch (t) {
            case EXPRESSION:     return "EXPRESSION";
            case WORD:           return "WORD";
            case LABEL:          return "LABEL";
            case REFERENCE:      return "REFERENCE";
            case RARROW:         return "RARROW";
            case PLUS:           return "PLUS";
            case MINUS:          return "MINUS";
            case MULTIPLICATION: return "MULTIPLICATION";
            case EQUALS:         return "EQUALS";
            case LPAR:           return "LPAR";
            case RPAR:           return "RPAR";
            case ASSIGN:         return "ASSIGN";
            case ENDSTATEMENT:   return "ENDSTATEMENT";

            // Keywords
            case APPLY:          return "APPLY";
            case ASSERT:         return "ASSERT";
            case BY:             return "BY";
            case DEFINE:         return "DEFINE";
            case END:            return "END";
            case GROUP:          return "GROUP";
            case IN:             return "IN";
            case INCLUDE:        return "INCLUDE";
            case OTHER:          return "OTHER";
            case PRINT:          return "PRINT";
            case PRINTF:         return "PRINTF";
            case PRINTN:         return "PRINTN";
            case REPLACE:        return "REPLACE";
            case TO:             return "TO";

            case ENDOFSTREAM:    return "ENDOFSTREAM";
            case UNKNOWN:
            default:             return "UNKNOWN";
        }
    };
} token;

typedef token::tok_type ttype;

/* Lexer class */
class Lexer {
    private:
        // Properties
        std::string buffer;
        long bufcount, buflen;
        std::string filename;
        long charpos, line;

        // Internal routines
        void back();
        char cchar();
        char gchar();
        ttype is_operator(const char) const;
        token *next();
        char peek() const;
    public:
        std::vector<token*> *Lex(std::string&, const std::string&, std::vector<token*> *tokens=nullptr);
        std::vector<token*> *LexFile(const std::string&, std::vector<token*> *tokens=nullptr);
        std::vector<token*> *IncludeFile(const std::string&, std::vector<token*> *tokens=nullptr);

        void Error(const std::string&);
};

#endif/*_LEXER_H*/
