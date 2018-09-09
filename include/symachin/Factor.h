#ifndef _SYMACHIN_FACTOR_H
#define _SYMACHIN_FACTOR_H

#include <memory>
#include <string>
#include <vector>

namespace symachin {
    class Factor;
    typedef std::shared_ptr<Factor> FactorPtr;
    typedef std::shared_ptr<std::vector<FactorPtr>> vectorFactorPtr;

    #define SIGN_NEGATE(s) (s==SYMACHIN_SIGN_POS?SYMACHIN_SIGN_NEG:SYMACHIN_SIGN_POS)
    enum sign {
        SYMACHIN_SIGN_POS,
        SYMACHIN_SIGN_NEG
    };

    class Factor {
        private:
            std::string name;
            enum sign sgn;

            bool isNumeric;
            int numericValue;
        public:
            Factor(const std::string&, enum sign s=SYMACHIN_SIGN_POS);
            Factor(const Factor&);
            ~Factor();

            std::string GetName() const { return name; }
            int GetNumericValue() const { return numericValue; }
            int GetNumericValue(const std::string&, enum sign, bool withsign=true) const;
            int GetNumericValue(const Factor&, bool withsign=true) const;
            enum sign GetSign() const { return sgn; }
            bool IsNumber() const { return isNumeric; }
            bool IsNumber(const std::string&) const;
            bool IsZero() const;

            vectorFactorPtr Multiply(const Factor&) const;
            void MultiplyNumeric(const Factor&);
            void Negate();
            bool IsEqual(const Factor&) const;

            std::string ToString() const;

            bool operator==(const Factor &f) const { return IsEqual(f); }
    };
}

#endif/*_SYMACHIN_FACTOR_H*/
