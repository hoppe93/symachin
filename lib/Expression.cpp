/**
 * Implementation of the 'Expression' class.
 */

#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include "symachin/Expression.h"
#include "symachin/ExpressionParser.h"

using namespace std;
using namespace symachin;

/**
 * Constructor.
 */
Expression::Expression() {
    terms = nullptr;
}
Expression::Expression(Term &t) {
    TermPtr trm(new Term(t));
    terms = vectorTermPtr(new vector<TermPtr>());
    terms->push_back(trm);
}
Expression::Expression(vectorTermPtr t) {
    terms = t;
}
Expression::Expression(const string &exprstr) {
    terms = Parse(exprstr);
}

/**
 * Destructor.
 */
Expression::~Expression() { }

/**
 * Check if this expression contains the given term.
 */
bool Expression::HasTerm(const Term &t) const {
    for (vector<TermPtr>::iterator it = terms->begin(); it != terms->end(); it++) {
        if (t.IsEqual(*(*it)))
            return true;
    }

    return false;
}

/**
 * Check if the given expression is
 * equal to this expression.
 */
bool Expression::IsEqual(Expression &expr) const {
    ExpressionPtr ep(new Expression(*this));
    ep->Subtract(expr);
    return (ep->IsZero());
}

/**
 * Check if this expression is zero.
 * (The expression is zero if ALL constituting terms
 * are zero).
 */
bool Expression::IsZero() const {
    for (vector<TermPtr>::const_iterator it = terms->begin(); it != terms->end(); it++) {
        if (!(*it)->IsZero())
            return false;
    }

    return true;
}

/**
 * Move all terms out of this expression
 * object, effectively clearing it.
 */
vectorTermPtr Expression::MoveTerms() {
    vectorTermPtr t = terms;
    terms = nullptr;
    return t;
}

/**
 * Negate this expression.
 */
void Expression::Negate() {
    for (vector<TermPtr>::iterator it = terms->begin(); it != terms->end(); it++) {
        (*it)->Negate();
    }
}

/**
 * Add the given factor as a term to this expression.
 */
void Expression::Add(Factor &f) {
    Term t(f);
    Add(t);
}

/**
 * Add the given term to this expression.
 */
void Expression::Add(Term &t) {
    Add(this->terms, t);
}

/**
 * Add the given terms to this expression.
 */
void Expression::Add(vectorTermPtr t) {
    for (vector<TermPtr>::iterator it = t->begin(); it != t->end(); it++)
        Add(*(*it));

    // The below is faster, but would require a separate
    // implementation of cancellation
    /*terms->reserve(terms->size() + t->size());
    terms->insert(terms->end(), t->begin(), t->end());*/
}

/**
 * Add the terms of the given expression to this expression.
 */
void Expression::Add(Expression &e) {
    Add(e.GetTerms());
}

/**
 * Subtract the given factor from this expression.
 */
void Expression::Subtract(Factor &f) {
    Term t(f);
    Subtract(t);
}

/**
 * Subtract the given term from this expression.
 */
void Expression::Subtract(Term &t) {
    TermPtr trm(new Term(t));
    trm->Negate();

    // Take advantage of the logic for
    // cancellation of terms
    Add(*trm);
}

/**
 * Subtract the given terms from this expression.
 */
void Expression::Subtract(vectorTermPtr t) {
    for (vector<TermPtr>::iterator it = t->begin(); it != t->end(); it++) {
        Subtract(*(*it));
    }
}

/**
 * Subtract the given expression from this expression.
 */
void Expression::Subtract(Expression &e) {
    Subtract(e.GetTerms());
}

/**
 * Multiply the given factor with this expression.
 */
void Expression::Multiply(Factor &f) {
    if (f.IsZero())
        terms->clear();
    else {
        for (vector<TermPtr>::iterator it = terms->begin(); it != terms->end(); it++) {
            (*it)->Multiply(f);
        }
    }
}

/**
 * Multiply the given term with this expression.
 */
void Expression::Multiply(Term &t) {
    if (t.IsZero())
        terms->clear();
    else {
        for (vector<TermPtr>::iterator it = terms->begin(); it != terms->end(); it++) {
            (*it)->Multiply(t);
        }
    }
}

/**
 * Multiply the given terms with this expression.
 */
void Expression::Multiply(vector<TermPtr> &t) {
    terms = Expression::Multiply(*terms, t);
}

/**
 * Multiply the given expression with this expression.
 */
void Expression::Multiply(Expression &e) {
    Multiply(*(e.GetTerms()));
}

/**
 * Convert the given string to an expression and multiply
 * it with this expression.
 */
void Expression::Multiply(const string &s) {
    ExpressionPtr ep(new Expression(Parse(s)));
    Multiply(*(ep->GetTerms()));
}

/**
 * Group this expression according to the list
 * of given factors.
 */
vectorExpressionPtr Expression::GroupBy(const vector<TermPtr> &exprs) const {
    vectorExpressionPtr expr(new vector<ExpressionPtr>());

    // First, make a copy of the terms to group
    vectorTermPtr t(new vector<TermPtr>(*terms));

    for (vector<TermPtr>::const_iterator it = exprs.begin(); it != exprs.end(); it++) {
        const TermPtr &ft = *it;
        vectorTermPtr group(new vector<TermPtr>());

        for (unsigned int i = 0; i < t->size(); i++) {
            if (t->at(i)->ContainsTerm(*ft)) {
                TermPtr trm(new Term(*(t->at(i))));
                trm->RemoveTerm(*(*it));
                group->push_back(trm);
                t->erase(std::remove(t->begin(), t->end(), t->at(i)), t->end());
                i--;
            }
        }

        if (group->size() == 0)
            group->push_back(TermPtr(new Term("0")));

        ExpressionPtr e(new Expression(group));
        expr->push_back(e);
    }

    if (t->size() > 0) {
        ExpressionPtr e(new Expression(t));
        expr->push_back(e);
    }

    return expr;
}

/**
 * Convert this expression into a string.
 *
 * formatted: If true, print slightly better formatted text.
 */
string Expression::ToString(bool formatted) {
    string s;

    if (terms->size() > 0) {
        TermPtr t = terms->at(0);
        if (t->GetSign() == SYMACHIN_SIGN_NEG)
            s = "-";
        s += t->ToString(formatted);
    }

    if (terms->size() > 1) {
        for (vector<TermPtr>::iterator it = terms->begin()+1; it != terms->end(); it++) {
            if ((*it)->GetSign() == SYMACHIN_SIGN_POS)
                s += "  +  " + (*it)->ToString(formatted);
            else
                s += "  -  " + (*it)->ToString(formatted);
        }
    }

    return s;
}

/**
 * Evaluate this expression numerically with the given
 * table of numeric substitutions.
 * 
 * subst: Table of numeric substitutions to make.
 * other: Value to assign to tokens not found in table 'subst'.
 */
double Expression::Evaluate(const map<string, double>& subst, const double other) const {
    double total = 0.0;

    for (vector<TermPtr>::const_iterator it = terms->begin(); it != terms->end(); it++)
        total += (*it)->Evaluate(subst, other);

    return total;
}

/********************
 * STATIC FUNCTIONS *
 ********************/
/**
 * Add the given term to the given expression.
 */
void Expression::Add(vectorTermPtr trms, const Term &t) {
    if (t.IsZero())
        return;

    // Check if a proportional term
    // already exists, and if so, merge them
    for (vector<TermPtr>::iterator it = trms->begin(); it != trms->end(); it++) {
        if (t.IsProportional(*it)) {
            int ne = (*it)->GetNumericFactorValue();
            int nt = t.GetNumericFactorValue();
            int sum = ne+nt;

            FactorPtr nf;
            if (sum > 0)
                nf = FactorPtr(new Factor(to_string(sum)));
            else if (sum < 0)
                nf = FactorPtr(new Factor(to_string(-sum), SYMACHIN_SIGN_NEG));
            else {  // Pre-factor is zero => cancel term
                trms->erase(it);
                return;
            }

            vectorFactorPtr nfac = (*it)->GetNonNumericFactors();
            if (abs(sum) != 1)
                nfac->push_back(nf);

            (*it)->ReplaceFactors(nfac);

            if ((sum > 0 && (*it)->GetSign() == SYMACHIN_SIGN_NEG) ||
                (sum < 0 && (*it)->GetSign() == SYMACHIN_SIGN_POS)) {
                (*it)->Negate();
            }

            return;
        }
    }

    // Otherwise, just append the term
    TermPtr trm(new Term(t));
    trms->push_back(trm);
}

/**
 * Multiply two expressions together.
 */
vectorTermPtr Expression::Multiply(const vector<TermPtr> &t1, const vector<TermPtr> &t2) {
    vectorTermPtr newTerms(new vector<TermPtr>());

    for (vector<TermPtr>::const_iterator it = t2.begin(); it != t2.end(); it++) {
        for (vector<TermPtr>::const_iterator jt = t1.begin(); jt != t1.end(); jt++) {
            TermPtr trm(new Term(*(*jt)));
            trm->Multiply(*(*it));

            Add(newTerms, *trm);
        }
    }

    return newTerms;
}
vectorTermPtr Expression::Multiply(const Term &t1, const vector<TermPtr> &t2) {
    vector<TermPtr> t;
    t.push_back(TermPtr(new Term(t1)));

    return Multiply(t, t2);
}
vectorTermPtr Expression::Multiply(const vector<TermPtr> &t1, const Term &t2) {
    vector<TermPtr> t;
    t.push_back(TermPtr(new Term(t2)));

    return Multiply(t1, t);
}
ExpressionPtr Expression::Multiply(const vector<TermPtr> &t1, const Expression &e2) {
    return ExpressionPtr(new Expression(
        Expression::Multiply(t1, *(e2.GetTerms()))
    ));
}
ExpressionPtr Expression::Multiply(const Expression &e1, const vector<TermPtr> &t2) {
    return ExpressionPtr(new Expression(
        Expression::Multiply(*(e1.GetTerms()), t2)
    ));
}
ExpressionPtr Expression::Multiply(const Expression &e1, const Expression &e2) {
    return ExpressionPtr(new Expression(
        Expression::Multiply(*(e1.GetTerms()), *(e2.GetTerms()))
    ));
}

/**
 * Parse the given string as a mathematical expression
 * using the 'ExpressionParser' class.
 */
vectorTermPtr Expression::Parse(const std::string &s) {
    ExpressionParser intp;

    ExpressionPtr expr = intp.Parse(s);
    vectorTermPtr terms = expr->MoveTerms();

    return terms;
}

long Expression::CountMaxUsage() {
    FactorPtr ptr;
    long maxcount = 0;

    for (vector<TermPtr>::iterator it = terms->begin(); it != terms->end(); it++) {
        vectorFactorPtr factors = (*it)->GetFactors();
        for (vector<FactorPtr>::iterator jt = factors->begin(); jt != factors->end(); jt++) {
            if (jt->use_count() > maxcount) {
                ptr = *jt;
                maxcount = jt->use_count();
            }
        }
    }

    return maxcount;
}
