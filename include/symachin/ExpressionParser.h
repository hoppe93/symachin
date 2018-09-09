#ifndef _SYMACHIN_EXPRESSION_PARSER_H
#define _SYMACHIN_EXPRESSION_PARSER_H

#include <string>
#include <vector>

namespace symachin {
    enum intp_ttype {
        SYMACHIN_TTYPE_OPERATOR,
        SYMACHIN_TTYPE_LPAR,
        SYMACHIN_TTYPE_RPAR,
        SYMACHIN_TTYPE_SYMBOL
    };
    struct intp_token {
        enum intp_ttype type;
        int precedence;
        std::string text;
    };

    class ExpressionParser {
        private:
            std::string *buffer;
            int bufindx=-1;

            std::vector<struct intp_token*> operator_stack;
            std::vector<struct intp_token*> symbol_stack;
            std::vector<struct intp_token*> output_queue;

            // Internal functions
            bool eoe() const;
            bool is_ignoreable(char) const;
            int is_operator(char) const;
            enum intp_ttype is_parenthesis(char) const;
            struct intp_token *next();
        public:
            ExpressionParser();

            ExpressionPtr Parse(const std::string&);
            ExpressionPtr Parse(const std::string*);

            ExpressionPtr ParsePostfix();
            void PreProcessBuffer();
            void ToPostfix();
    };
}

#endif/*_SYMACHIN_EXPRESSION_PARSER_H*/
