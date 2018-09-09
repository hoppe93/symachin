#ifndef _SYMACHIN_EXPRESSION_H
#define _SYMACHIN_EXPRESSION_H

#include <memory>
#include <vector>
#include "symachin/Factor.h"
#include "symachin/Term.h"

namespace symachin {
    class Expression;
    typedef std::shared_ptr<Expression> ExpressionPtr;
    typedef std::shared_ptr<std::vector<ExpressionPtr>> vectorExpressionPtr;

    class Expression {
        private:
            vectorTermPtr terms=nullptr;
        public:
            Expression();
            Expression(Term&);
            Expression(vectorTermPtr);
            Expression(const std::string&);
            ~Expression();

            vectorTermPtr GetTerms() const { return terms; }
            vectorTermPtr MoveTerms();
            bool HasTerm(const Term&) const;
            bool IsEqual(Expression&) const;
            bool IsZero() const;
            void Negate();
            unsigned int NumberOfTerms() const { return terms->size(); }

            void Add(Factor&);
            void Add(Term&);
            void Add(vectorTermPtr);
            void Add(Expression&);

            void Subtract(Factor&);
            void Subtract(Term&);
            void Subtract(vectorTermPtr);
            void Subtract(Expression&);

            void Multiply(Factor&);
            void Multiply(Term&);
            void Multiply(std::vector<TermPtr>&);
            void Multiply(Expression&);
            void Multiply(const std::string&);

            static void Add(vectorTermPtr, const Term&);
            static vectorTermPtr Multiply(const std::vector<TermPtr>&, const std::vector<TermPtr>&);
            static vectorTermPtr Multiply(const Term&, const std::vector<TermPtr>&);
            static vectorTermPtr Multiply(const std::vector<TermPtr>&, const Term&);
            static ExpressionPtr Multiply(const std::vector<TermPtr>&, const Expression&);
            static ExpressionPtr Multiply(const Expression&, const std::vector<TermPtr>&);
            static ExpressionPtr Multiply(const Expression&, const Expression&);

            static vectorTermPtr Parse(const std::string&);

            vectorExpressionPtr GroupBy(const std::vector<TermPtr>&) const;
            std::string ToString();

            long CountMaxUsage();
    };
}

#endif/*_SYMACHIN_EXPRESSION_H*/
