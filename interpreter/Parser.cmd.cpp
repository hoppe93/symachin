/**
 * Implementation of parser commands.
 */

#include <iostream>
#include <string>
#include <vector>
#include "symachin/Expression.h"
#include "symachin/Operators/Replace.h"
#include "interpreter/Parser.h"

using namespace std;
using namespace symachin;

/**
 * Apply the rule of the given name to the
 * expression of the given name.
 *
 * nrule: Name of rule to apply.
 * nexpr: Name of expression to apply rule to.
 */
void Parser::apply_to(const string &label, const string &nrule, ExpressionPtr &expr) {
    if (rules.count(nrule) == 0)
        Error("No rule named '%s' defined.");

    ReplacePtr rule = rules[nrule];
    ExpressionPtr rexpr = rule->Operate(*expr);
    assign(label, rexpr);
}

/**
 * Assert that the two expressions are equal.
 */
void Parser::assert(ExpressionPtr &e1, ExpressionPtr &e2) {
    if (!e1->IsEqual(*e2))
        Error("Assertion failed. The two expressions are NOT equal.");
}

/**
 * Assign the given expression to the given label.
 *
 * label: Name to assign to expression.
 * expr:  Expression to store.
 */
void Parser::assign(const string &label, ExpressionPtr &expr) {
    // If an expression with the same,
    // label exists overwrite it
    if (expressions.count(label) > 0)
        expressions[label] = expr;
    // Do not permit rules and expressions
    // to have overlapping names
    else if (rules.count(label) > 0)
        Error("A rule with the label '%s' has already been defined.", label.c_str());
    // Create a new expression
    else
        expressions.insert({label, expr});
}

/**
 * Define a new replacement rule.
 *
 * label: Name to assign to rule.
 * rep:   Replacement rule.
 */
void Parser::define(const string &label, ReplacePtr &rep) {
    if (expressions.count(label) > 0)
        Error("An expression with the label '%s' has already been defined.", label.c_str());
    else if (rules.count(label) > 0)
        rules[label] = rep;
    else
        rules.insert({label, rep});
}

/**
 * Evaluate the given expression numerically
 * with the given substitutions.
 *
 * expr:  Expression to evaluate.
 * subst: Numeric substitutions to make.
 * other: Value to assign to tokens not in 'subst'.
 */
void Parser::evaluate(ExpressionPtr &expr, map<string, double> &subst, double other) {
    double d = expr->Evaluate(subst, other);

    cout << d << endl;
}

/**
 * Evaluate the given expression numerically
 * with the given substitutions, and assert that
 * it is equal to the given value.
 *
 * expr:  Expression to evaluate.
 * val:   Value that the expression is expected to evaluate to.
 * subst: Numeric substitutions to make.
 * other: Value to assign to tokens not in 'subst'.
 */
void Parser::evaluate_assert(ExpressionPtr &expr, double val, map<string, double> &subst, double other) {
    double d = expr->Evaluate(subst, other);

    if (d != val)
        Error("Assertion failed. The expression did NOT evaluate to the expected value. Evaluated: %f, expected: %f.", d, val);
}

/**
 * Group the given expression by the given factor.
 *
 * expr:     Expression to group.
 * factors:  List of factors to group by.
 * labels:   List of labels to store the grouped expressions in.
 * otherlbl: Label for the 'other' terms (those not explicitly grouped).
 */
void Parser::group_by(ExpressionPtr &expr, vector<ExpressionPtr> &terms, vector<string> &labels, const string &otherlbl) {
    unsigned long i;
    vector<TermPtr> tp;
    for (vector<ExpressionPtr>::const_iterator it = terms.begin(); it != terms.end(); it++) {
        const ExpressionPtr &ep = *it;
        // Convert expressions to list of terms
        if (ep->NumberOfTerms() != 1)
            Error("The expressions to group by must not consist of multiple terms.");

        tp.push_back(ep->GetTerms()->front());
    }

    vectorExpressionPtr grouped = expr->GroupBy(tp);

    for (i = 0; i < labels.size(); i++) {
        assign(labels.at(i), grouped->at(i));
    }

    if (!otherlbl.empty()) {
        if (grouped->size() > i)
            assign(otherlbl, grouped->at(i));
        else {
            ExpressionPtr ep(new Expression("0"));
            assign(otherlbl, ep);
        }
    }
}

/**
 * Print the coming sequence of tokens.
 */
void Parser::print() {
    while (peek() != token::ENDSTATEMENT) {
        if (expect(
                token::WORD, token::REFERENCE, token::PLUS,
                token::MINUS, token::MULTIPLICATION,
                token::LPAR, token::RPAR
            ) == token::REFERENCE
        ) {
            token *tkn = gtkn();
            if (expressions.count(tkn->text) == 0)
                Error("No expression with the label '%s' has been defined.", tkn->text.c_str());

            cout << expressions[tkn->text]->ToString() << " ";
        } else
            cout << gtkn()->text << " ";
    }

    expect(token::ENDSTATEMENT);
    cout << endl;
}

/**
 * Print the expression in a slightly better formatted way.
 */
void Parser::printf(ExpressionPtr &ep) {
    string s = ep->ToString(true);
    cout << s << endl;
}

/**
 * Print the number of terms in the given expression.
 */
void Parser::printn(ExpressionPtr &ep) {
    unsigned int n = ep->NumberOfTerms();
    if (n == 1 && ep->GetTerms()->front()->IsZero()) {
        cout << "0\n";
    } else {
        cout << ep->NumberOfTerms() << endl;
    }
}

/**
 * Replace single factor in expression.
 *
 * label: Label to assign result to.
 * fac:   Factor to replace.
 * repl:  (string) Expression to replace the factor with.
 * expr:  Expression to do the replacement in.
 */
void Parser::replace_in(
    const string &label, const string &fac, const string &repl, ExpressionPtr expr
) {
    ReplacePtr rp(new Replace());

    rp->CreateRule(fac, repl);
    ExpressionPtr rexpr = rp->Operate(*expr);
    assign(label, rexpr);
}

