#ifndef _SYMACHIN_TERM_H
#define _SYMACHIN_TERM_H

#include <memory>
#include <vector>
#include "symachin/Factor.h"

namespace symachin {
    class Term;
    typedef std::shared_ptr<Term> TermPtr;
    typedef std::shared_ptr<std::vector<TermPtr>> vectorTermPtr;

    class Term {
        private:
            vectorFactorPtr factors=nullptr;
        public:
            Term(const std::string&, enum sign sgn=SYMACHIN_SIGN_POS);
            Term(const Factor&);
            Term(vectorFactorPtr);
            Term(const Term&);
            ~Term();

            bool ContainsTerm(const Term&) const;
            vectorFactorPtr GetFactors() const { return factors; }
            vectorFactorPtr GetNonNumericFactors() const;
            int GetNumericFactorValue(bool withsign=true) const;
            enum sign GetSign() const;
            bool HasFactor(const Factor&) const;
            bool HasNumericFactor() const;
            bool IsZero() const;

            void Multiply(const Factor&);
            void Multiply(const Term&);

            void RemoveFactor(const Factor&);
            void RemoveTerm(const Term&);
            void ReplaceFactors(vectorFactorPtr);

            void Negate();
            int NumberOfFactors() const;

            bool IsProportional(const Term& t) const { return IsEqual(t, false); }
            bool IsProportional(const TermPtr t) const { return IsProportional(*t); }
            bool IsEqual(const Term&, bool numericEquality=true) const;
            bool IsEqual(const TermPtr t, bool numericEquality=true) const { return IsEqual(*t, numericEquality); }

            std::string ToString() const;

            bool operator==(const Term &t) const { return IsEqual(t); }
    };
}

#endif/*_SYMACHIN_TERM_H*/
