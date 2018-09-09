#ifndef _SYMACHIN_OPERATOR_H
#define _SYMACHIN_OPERATOR_H

namespace symachin {
    class Operator {
        private:
        public:
            virtual ExpressionPtr Operate(const Expression&) const = 0;
    };
}

#endif/*_SYMACHIN_OPERATOR_H*/
