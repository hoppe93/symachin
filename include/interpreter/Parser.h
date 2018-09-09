#ifndef _PARSER_H
#define _PARSER_H

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "symachin/Expression.h"
#include "symachin/Operators/Replace.h"

#include "interpreter/Lexer.h"
#include "interpreter/ParserException.h"

class Parser {
    private:
        std::unordered_map<std::string, symachin::ExpressionPtr> expressions;
        std::unordered_map<std::string, symachin::ReplacePtr> rules;

        std::vector<token*> *tokenlist;
        unsigned long tknindex;

        std::string currentlabel;

        token endtkn = { "", "", 0, 0, token::ENDOFSTREAM };

        // Internal routines
        token *advance();
        ttype expect(ttype);
        ttype expect_expression(ttype t=token::ENDSTATEMENT);
        token *gtkn() const;
        ttype peek() const;
        void require_label() const;

        // Commands
        void apply_to(const std::string&, const std::string&, symachin::ExpressionPtr&);
        void assert(symachin::ExpressionPtr&, symachin::ExpressionPtr&);
        void assign(const std::string&, symachin::ExpressionPtr&);
        void define(const std::string&, symachin::ReplacePtr&);
        //void group_by(symachin::ExpressionPtr&, std::vector<symachin::FactorPtr>&, std::vector<std::string>&, const std::string&);
        void group_by(symachin::ExpressionPtr&, std::vector<symachin::ExpressionPtr>&, std::vector<std::string>&, const std::string&);
        void print();
        void printn(symachin::ExpressionPtr&);
        void replace_in(const std::string&, const std::string&, const std::string&, symachin::ExpressionPtr);

        template<typename ... Args>
        ttype expect(ttype t, Args... args) {
            if (tknindex >= tokenlist->size())
                Error("Unexpected end of token stream.");
            token *tkn = tokenlist->at(tknindex);

            if (tkn->type == t) {
                tknindex++;
                return tkn->type;
            } else
                return expect(args...);
        };
    public:
        void LoadTokens(std::vector<token*>*);

        void Parse(std::string&);
        void Parse(std::vector<token*>*);
        void ParseFile(const std::string&);

        /**
         * Report an error in the parser.
         */
        template<typename ... Args>
        void Error(const std::string& msg, Args&& ... args) const {
            token *tkn;
            if (tknindex > 1 && tknindex < tokenlist->size())
                tkn = tokenlist->at(tknindex-1);
            else if (tknindex == 0)
                tkn = tokenlist->front();
            else
                tkn = tokenlist->back();

            throw ParserException(
                "%s: line %d:%d: "+msg,
                tkn->filename.c_str(), tkn->line, tkn->charpos,
                std::forward<Args>(args) ...
            );
        };
};

#endif/*_PARSER_H*/
