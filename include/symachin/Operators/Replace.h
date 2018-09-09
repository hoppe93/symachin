#ifndef _SYMACHIN_REPLACE_H
#define _SYMACHIN_REPLACE_H

#include <memory>
#include <string>
#include <vector>
#include "symachin/Expression.h"
#include "symachin/Factor.h"
#include "symachin/Operators/Operator.h"
#include "symachin/Operators/Replace.h"

namespace symachin {
    class Replace;
    typedef std::shared_ptr<Replace> ReplacePtr;

    struct replace_rule {
        FactorPtr from;
        vectorTermPtr to;
    };
    class Replace : public Operator {
        private:
            std::vector<struct replace_rule*> rules;
        public:
            Replace();
            ~Replace();

            void CreateRule(const Factor&, const vectorTermPtr);
            void CreateRule(const std::string&, const std::string&);
            virtual ExpressionPtr Operate(const Expression&) const;
    };
}

#endif/*_SYMACHIN_REPLACE_H*/
