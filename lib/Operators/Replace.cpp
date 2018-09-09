/**
 * Implementation of the 'Replace' operator.
 */

#include <string>
#include <vector>
#include "symachin/Expression.h"
#include "symachin/Operators/Replace.h"
#include "symachin/SymachinException.h"

using namespace std;
using namespace symachin;

/**
 * Constructor.
 */
Replace::Replace() {}

/**
 * Destructor.
 */
Replace::~Replace() {}

/**
 * Create a replacement rule.
 */
void Replace::CreateRule(const Factor &from, const vectorTermPtr to) {
    struct replace_rule *r = new struct replace_rule;
    r->from.reset(new Factor(from));

    // Make sure there's no ambiguity in the rules...
    for (vector<struct replace_rule*>::iterator it = rules.begin(); it != rules.end(); it++) {
        struct replace_rule *rule = *it;
        if (rule->from->IsEqual(from))
            throw SymachinException("A rule for the factor '%s' has already been defined.", from.GetName().c_str());
    }
    
    // Create the rule
    if (to == nullptr) {
        r->to = nullptr;
    } else {
        r->to = vectorTermPtr(new vector<TermPtr>());
        for (vector<TermPtr>::const_iterator it = to->begin(); it != to->end(); it++) {
            TermPtr tptr(new Term(*(*it)));
            r->to->push_back(tptr);
        }
    }

    rules.push_back(r);
}
void Replace::CreateRule(const std::string &from, const std::string &toexpr) {
    Factor f(from);
    if (toexpr == "")
        CreateRule(f, nullptr);
    else {
        vectorTermPtr t = Expression::Parse(toexpr);
        CreateRule(f, t);
    }
}

/**
 * Replace factors in the given expression according
 * to the set of rules defined in this object.
 *
 * Note that any term that doesn't contain any of the
 * factors which have rules defined for them, a "zero"
 * rule is automatically defined which eliminates the term.
 */
ExpressionPtr Replace::Operate(const Expression &expr) const {
    ExpressionPtr e;
    vectorTermPtr terms = expr.GetTerms();
    vectorTermPtr newTerms = vectorTermPtr(new vector<TermPtr>());

    for (vector<TermPtr>::const_iterator it = terms->begin(); it != terms->end(); it++) {
        TermPtr trm = *it;

        // Apply operator to this term
        for (vector<struct replace_rule*>::const_iterator jt = rules.begin(); jt != rules.end(); jt++) {
            struct replace_rule *rule = *jt;

            // "rule->to = nullptr" means the rule is to
            // replace "from" with 0.
            if (rule->to == nullptr)
                continue;

            // If this term has the factor "from"
            if (trm->HasFactor(*(rule->from))) {
                vectorTermPtr res = Expression::Multiply(*trm, *(rule->to));

                for (vector<TermPtr>::iterator kt = res->begin(); kt != res->end(); kt++) {
                    TermPtr tp = *kt;
                    tp->RemoveFactor(*(rule->from));
                }

                // Insert terms
                newTerms->insert(newTerms->end(), res->begin(), res->end());

                // Break out of loop to avoid applying
                // more rules to the same term
                break;
            }
        }
    }

    return ExpressionPtr(new Expression(newTerms));
}

