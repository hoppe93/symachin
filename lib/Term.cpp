/**
 * Implementation of the 'Term' class.
 */

#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include "symachin/Factor.h"
#include "symachin/Term.h"

using namespace std;
using namespace symachin;

/**
 * Constructor.
 */
Term::Term(const string &s, enum sign sgn) {
    FactorPtr f(new Factor(s, sgn));
    factors = vectorFactorPtr(new vector<FactorPtr>());
    factors->push_back(f);
}
Term::Term(const Factor &f) {
    FactorPtr fac(new Factor(f));
    factors = vectorFactorPtr(new vector<FactorPtr>());
    factors->push_back(fac);
}
Term::Term(vectorFactorPtr f) {
    factors = f;
}
/**
 * Copy-constructor.
 */
Term::Term(const Term &t) {
    factors = vectorFactorPtr(new vector<FactorPtr>());
    vectorFactorPtr f = t.GetFactors();

    for (vector<FactorPtr>::iterator it = f->begin(); it != f->end(); it++) {
        factors->push_back(FactorPtr(new Factor(*(*it))));
    }
}

/**
 * Destructor.
 */
Term::~Term() { }

/**
 * Checks whether the given term is a factor of
 * this term.
 */
bool Term::ContainsTerm(const Term &t) const {
    const vectorFactorPtr facts = t.GetFactors();
    unsigned int i, n=factors->size();
    bool matched[n] = {false};

    for (vector<FactorPtr>::const_iterator it = facts->begin(); it != facts->end(); it++) {
        const FactorPtr &f1 = *it;
        for (i = 0; i < n; i++) {
            if (matched[i])
                continue;

            FactorPtr &f2 = factors->at(i);
            if (f1->IsEqual(*f2)) {
                matched[i] = true;
                break;
            }
        }

        if (i == n)
            return false;
    }

    return true;
}

/**
 * Return the factors of this term that
 * are not numeric.
 */
vectorFactorPtr Term::GetNonNumericFactors() const {
    vectorFactorPtr nf(new vector<FactorPtr>());
    for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
        FactorPtr &f = *it;
        if (!f->IsNumber()) {
            nf->push_back(FactorPtr(new Factor(*f)));
        }
    }

    return nf;
}

/**
 * Return the numeric factor of this term
 * as an integer.
 */
int Term::GetNumericFactorValue(bool withsign) const {
    for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
        FactorPtr &f = *it;
        if (f->IsNumber()) {
            int v = abs(f->GetNumericValue());
            if (!withsign)
                return v;

            if (GetSign() == SYMACHIN_SIGN_POS)
                return v;
            else
                return -v;
        }
    }

    if (GetSign() == SYMACHIN_SIGN_POS)
        return 1;
    else
        return -1;
}

/**
 * Returns the overall sign of this term.
 */
enum sign Term::GetSign() const {
    enum sign sgn = SYMACHIN_SIGN_POS;
    for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
        FactorPtr &f = *it;
        if (f->GetSign() == SYMACHIN_SIGN_NEG)
            sgn = SIGN_NEGATE(sgn);
    }

    return sgn;
}

/**
 * Check if this term has the given factor.
 */
bool Term::HasFactor(const Factor &f) const {
    for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
        if (f.IsEqual(*(*it)))
            return true;
    }

    return false;
}

/**
 * Check if this term has a numeric factor.
 */
bool Term::HasNumericFactor() const {
    for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
        if ((*it)->IsNumber())
            return true;
    }
    return false;
}

/**
 * Check if this term is zero.
 * (It's zero if any of its factors are zero).
 */
bool Term::IsZero() const {
    for (vector<FactorPtr>::const_iterator it = factors->begin(); it != factors->end(); it++) {
        if ((*it)->IsZero())
            return true;
    }

    return false;
}

/**
 * Multiply the given factor with this term.
 */
void Term::Multiply(const Factor &f) {
    // Check if this term already has a numeric factor,
    // and if so, do numeric multiplication between them
    if (f.IsNumber()) {
        for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
            if ((*it)->IsNumber()) {
                (*it)->MultiplyNumeric(f);
                return;
            }
        }
    }

    FactorPtr fac(new Factor(f));
    factors->push_back(fac);
}

/**
 * Multiply the given term with this term.
 */
void Term::Multiply(const Term &t) {
    vectorFactorPtr f = t.GetFactors();
    for (vector<FactorPtr>::iterator it = f->begin(); it != f->end(); it++) {
        Multiply(*(*it));
    }
}

/**
 * Negate this term.
 */
void Term::Negate() {
    (*factors)[0]->Negate();
}

/**
 * Returns the number of factors of this term.
 */
int Term::NumberOfFactors() const {
    return factors->size();
}

/**
 * Check if the two terms are equal.
 * If 'numericEquality' is true, the stricter condition
 * for equality that ALL factors (including numeric ones)
 * must be present in both terms.
 *
 * If 'numericEquality' is false, all numeric factors are
 * ignored, thus making this function check if the two
 * terms are proportional to each other.
 */
bool Term::IsEqual(const Term &t, bool numericEquality) const {
    if (numericEquality && t.NumberOfFactors() != NumberOfFactors())
        return false;

    vectorFactorPtr iFac = factors;
    vectorFactorPtr jFac = t.GetFactors();
    bool matched[t.NumberOfFactors()] = {false};
    int iHasNumber=0, jHasNumber=0;
    unsigned int i, j, ni = iFac->size(),
                       nj = jFac->size();
    if (ni < nj) {
        iFac = t.GetFactors();
        jFac = factors;

        ni = iFac->size();
        nj = jFac->size();
    }

    // Count number of numeric terms in each term
    if (!numericEquality) {
        for (i = 0; i <  ni; i++) {
            FactorPtr &f = iFac->at(i);
            if (f->IsNumber())
                iHasNumber++;
        }
        for (j = 0; j < nj; j++) {
            FactorPtr &f = jFac->at(j);
            if (f->IsNumber())
                jHasNumber++;
        }
    }

    for (i = 0; i < ni; i++) {
        FactorPtr &f1 = iFac->at(i);
        if (!numericEquality && f1->IsNumber())
            continue;

        for (j = 0; j < nj; j++) {
            // If this term has already been
            // matched, skip it
            if (matched[j])
                continue;

            FactorPtr &f2 = jFac->at(j);

            // If we're not considering numbers,
            // and this is a number, mark it as matched
            // to prevent further i iterations from
            // testing it.
            if (!numericEquality && f2->IsNumber()) {
                matched[j] = true;
                continue;
            } else if (f1->IsEqual(*f2)) {
                matched[j] = true;
                break;
            }
        }

        // Factor i was not present in t?
        if (j == nj)
            return false;
    }

    if (!numericEquality) {
        return ((ni-iHasNumber) == (nj-jHasNumber));
    } else
        return true;
}

/**
 * Remove one occurence of the given factor
 * from this term.
 */
void Term::RemoveFactor(const Factor &f) {
    for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
        if ((*it)->IsEqual(f)) {
            enum sign sgn = (*it)->GetSign();
            factors->erase(remove(factors->begin(), factors->end(), *it), factors->end());

            // If this was the last factor, push
            // back a factor of unity
            if (factors->size() == 0)
                factors->push_back(FactorPtr(new Factor("1")));

            if (sgn != f.GetSign())
                Negate();
            break;
        }
    }
}

/**
 * Remove the factors of the given term from this term.
 */
void Term::RemoveTerm(const Term &t) {
    vectorFactorPtr fac = t.GetFactors();
    for (vector<FactorPtr>::iterator it = fac->begin(); it != fac->end(); it++) {
        FactorPtr &f = *it;
        RemoveFactor(*f);
    }
}

/**
 * Replace the current list of factors with
 * a copy of the given list of factors.
 * The old list of factors is destroyed.
 */
void Term::ReplaceFactors(vectorFactorPtr newfacts) {
    this->factors = newfacts;
}

/**
 * Convert this term to a string.
 *
 * formatted: If true, return a slightly better formatted string.
 */
string Term::ToString(bool formatted) const {
    string s;

    if (formatted) {
        return ToStringFormatted();
    } else {
        if (factors->size() > 0)
            s = factors->at(0)->ToString();

        if (factors->size() > 1) {
            for (vector<FactorPtr>::iterator it = factors->begin()+1; it != factors->end(); it++) {
                s += " * " + (*it)->ToString();
            }
        }
    }

    return s;
}

string Term::ToStringFormatted() const {
    string s;
    bool nonTrivialNumericFactor = false;

    // Count occurences of factors
    map<string, unsigned int> occ;
    string num;

    for (vector<FactorPtr>::iterator it = factors->begin(); it != factors->end(); it++) {
        if ((*it)->IsNumber()) {
            num = (*it)->ToString();
            nonTrivialNumericFactor = ((*it)->GetNumericValue() != 1);
        } else if (occ.find((*it)->ToString()) != occ.end())
            occ[(*it)->ToString()] += 1;
        else
            occ[(*it)->ToString()] = 1;
    }

    if (nonTrivialNumericFactor || occ.size() == 0)
        s = num;

    for (map<string, unsigned int>::iterator it = occ.begin(); it != occ.end(); it++) {
        if (nonTrivialNumericFactor || it != occ.begin())
            s += " * ";

        s += it->first;

        if (it->second != 1)
            s += "^" + to_string(it->second);
    }

    return s;
}

