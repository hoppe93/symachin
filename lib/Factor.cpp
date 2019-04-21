/**
 * Implementation of the 'Factor' class.
 */

#include <string>
#include <vector>
#include "symachin/Factor.h"
#include "symachin/FactorException.h"

using namespace std;
using namespace symachin;

/**
 * Constructor.
 */
Factor::Factor(const string &name, enum sign s) {
    this->name = name;
    this->sgn = s;
    this->isNumeric = IsNumber(name);
    if (this->isNumeric)
        this->numericValue = GetNumericValue(name, s);
}
/**
 * Copy-constructor.
 */
Factor::Factor(const Factor &f) {
    this->name = f.GetName();
    this->sgn = f.GetSign();
    this->isNumeric = f.IsNumber();
    if (this->isNumeric)
        this->numericValue = GetNumericValue(f);
}

/**
 * Destructor.
 */
Factor::~Factor() { }

/**
 * Get the numeric value of this factor
 * (throws an error if this factor is not
 * a number).
 */
int Factor::GetNumericValue(const string &name, enum sign s, bool withsign) const {
    int v = stoi(name);
    if (withsign && s == SYMACHIN_SIGN_NEG)
        return -v;
    else
        return v;
}
int Factor::GetNumericValue(const Factor &f, bool withsign) const {
    return GetNumericValue(f.GetName(), f.GetSign(), withsign);
}

/**
 * Check if two factors are equal.
 */
bool Factor::IsEqual(const Factor &f) const {
    return (this->name == f.GetName());
}

/**
 * Check if the given string represents
 * an integral number.
 */
bool Factor::IsNumber(const string &s) const {
    for (string::const_iterator it = s.begin(); it != s.end(); it++) {
        if (*it > '9' || *it < '0')
            return false;
    }

    return true;
}

/**
 * Check if this factor is zero.
 */
bool Factor::IsZero() const {
    return (this->name == "0");
}

/**
 * Multiply two factors.
 */
vectorFactorPtr Factor::Multiply(const Factor &f) const {
    vectorFactorPtr facts(new vector<FactorPtr>());

    if (this->isNumeric && f.IsNumber()) {
        /*unsigned int n = stoi(this->name) * stoi(f.GetName());
        enum sign s = (this->GetSign()==f.GetSign() ? SYMACHIN_SIGN_POS:SYMACHIN_SIGN_NEG);
        FactorPtr f1(new Factor(to_string(n), s));
        facts->push_back(f1);*/
        FactorPtr f1(new Factor(*this));
        f1->MultiplyNumeric(f);
        facts->push_back(f1);
    } else {
        FactorPtr f1(new Factor(*this)),
                  f2(new Factor(f));
        facts->push_back(f1);
        facts->push_back(f2);
    }

    return facts;
}

/**
 * Multiply this factor by the given factor. Note
 * that both factors must be numeric!
 */
void Factor::MultiplyNumeric(const Factor &f) {
    if (!this->isNumeric || !f.IsNumber())
        throw FactorException("Attempting to multiply two non-numeric factors together as numbers.");

    //unsigned int n = stoi(this->name) * stoi(f.GetName());
    unsigned int n = this->numericValue * f.GetNumericValue();
    this->name = to_string(n);
    this->sgn = (this->GetSign()==f.GetSign() ? SYMACHIN_SIGN_POS:SYMACHIN_SIGN_NEG);
    this->numericValue = n;
}

/**
 * Negate this factor (flip the sign).
 */
void Factor::Negate() {
    sgn = (sgn==SYMACHIN_SIGN_POS?SYMACHIN_SIGN_NEG:SYMACHIN_SIGN_POS);
    numericValue = -numericValue;
}

string Factor::ToString() const {
    //if (this->sgn == SYMACHIN_SIGN_POS)
        return GetName();
    /*else
        return "(-"+GetName()+")";*/
}

