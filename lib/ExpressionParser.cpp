/**
 * Implementation of the 'ExpressionParser' class.
 */

#include <string>
#include <vector>
#include "symachin/Expression.h"
#include "symachin/ExpressionParser.h"
#include "symachin/ExpressionParserException.h"

using namespace std;
using namespace symachin;

/**
 * Constructor.
 */
ExpressionParser::ExpressionParser() {}

/**
 * Parse the given string as a mathematical
 * expression. SYMACHIN recognizes five different symbols:
 *
 *   1. PLUS           (+)
 *   2. MINUS          (-)
 *   3. MULTIPLICATION (*)
 *   4. PARANTHESES    ( '(', '[', '{', ')', ']', '}' )
 *   5. OTHER TEXT
 */
ExpressionPtr ExpressionParser::Parse(const string &expr) {
    return Parse(&expr);
}
ExpressionPtr ExpressionParser::Parse(const string *expr) {
    buffer = new string(*expr);
    bufindx = -1;

    operator_stack.clear();
    symbol_stack.clear();

    // Pre-process string
    PreProcessBuffer();

    // Convert string to tokens in Postfix form
    ToPostfix();

    // Now, the output stack is set up as
    // a postfix expression and is ready to
    // be interpreted.
    ExpressionPtr e = ParsePostfix();

    return e;
}

/**
 * Parse an expression in postfix form.
 */
ExpressionPtr ExpressionParser::ParsePostfix() {
    vector<ExpressionPtr> *exprs = new vector<ExpressionPtr>();

    for (
        vector<struct intp_token*>::iterator it = output_queue.begin();
        it != output_queue.end();
        it++
    ) {
        struct intp_token *tkn = *it;

        // If operator...
        if (tkn->type == SYMACHIN_TTYPE_OPERATOR) {
            if (exprs->size() < 2)
                throw ExpressionParserException("Syntax error in expression.");

            ExpressionPtr op2 = exprs->back();
            exprs->pop_back();
            ExpressionPtr op1 = exprs->back();
            exprs->pop_back();

            switch (tkn->text.front()) {
                case '+':
                    op1->Add(*op2);
                    break;
                case '-':
                    if (op1->IsZero()) {
                        op1 = op2;
                        op1->Negate();
                    } else
                        op1->Subtract(*op2);
                    break;
                case '*':
                    op1->Multiply(*op2);
                    break;
                default:
                    throw ExpressionParserException("Unrecognized operator: "+tkn->text);
            }

            exprs->push_back(op1);
        } else {       // Symbol
            Term t(tkn->text);
            ExpressionPtr ep(new Expression(t));
            exprs->push_back(ep);
        }
    }

    if (exprs->size() != 1)
        throw ExpressionParserException("Expression stack does not contain a single element as expected.");

    ExpressionPtr expr = exprs->back();
    exprs->pop_back();

    return expr;
}

/**
 * Pre-process the buffer:
 *   - Make sure that if the first non-ignoreable
 *     character of the buffer is an operator, a
 *     zero is inserted right before it.
 */
void ExpressionParser::PreProcessBuffer() {
    unsigned int i, l = buffer->length();
    for (i = 0; i < l; i++) {
        if (is_ignoreable(buffer->at(i)))
            continue;
        else if (is_operator(buffer->at(i)))
            buffer->insert(0, "0");

        break;
    }
}

/**
 * Convert the buffer string to postfix notation.
 */
void ExpressionParser::ToPostfix() {
    struct intp_token *tkn;
    while (!eoe() && (tkn=next())!=nullptr) {
        switch (tkn->type) {
            case SYMACHIN_TTYPE_SYMBOL:
                output_queue.push_back(tkn);
                break;
            case SYMACHIN_TTYPE_OPERATOR: {
                while (
                    operator_stack.size() > 0 &&
                    operator_stack.back()->precedence >= tkn->precedence &&
                    operator_stack.back()->type != SYMACHIN_TTYPE_LPAR
                ) {
                    output_queue.push_back(operator_stack.back());
                    operator_stack.pop_back();
                }

                operator_stack.push_back(tkn);
            } break;
            case SYMACHIN_TTYPE_LPAR:
                operator_stack.push_back(tkn);
                break;
            case SYMACHIN_TTYPE_RPAR: {
                while (
                    operator_stack.size() > 0 &&
                    operator_stack.back()->type != SYMACHIN_TTYPE_LPAR
                ) {
                    output_queue.push_back(operator_stack.back());
                    operator_stack.pop_back();
                }

                if (operator_stack.size() == 0)
                    throw ExpressionParserException("Mismatched parenthesis in expression.");

                // Pop left parenthesis
                struct intp_token *tok = operator_stack.back();
                operator_stack.pop_back();
                delete tok;
                delete tkn;
            } break;
            default:
                throw ExpressionParserException("Unrecognized token type.");
        }
    }

    while (operator_stack.size() > 0) {
        if (operator_stack.back()->type == SYMACHIN_TTYPE_LPAR)
            throw ExpressionParserException("Mismatched parenthesis in expression.");
        output_queue.push_back(operator_stack.back());
        operator_stack.pop_back();
    }
}

/**********************
 * INTERNAL FUNCTIONS *
 **********************/
/**
 * End-of-expression reached?
 */
bool ExpressionParser::eoe() const { return (bufindx >= (signed)buffer->size()); }

/**
 * Check if the given character is ignorable.
 */
bool ExpressionParser::is_ignoreable(char c) const {
    switch (c) {
        case ' ': case '\t': case '\n': return true;
        default: return false;
    }
}

/**
 * Check if the given character classifies as an operator.
 * Returns the operator precedence level.
 */
int ExpressionParser::is_operator(char c) const {
    switch (c) {
        case '+': return 1;
        case '-': return 2;
        case '*': return 3;
        default:
            return 0;
    }
}

/**
 * Check if the given character is a parenthesis.
 * Returns either 'SYMACHIN_TTYPE_LPAR' or 
 * 'SYMACHIN_TTYPE_RPAR' if the character is a left
 * or right parenthesis, respectively, and returns
 * 'SYMACHIN_TTYPE_SYMBOL' otherwise.
 */
enum intp_ttype ExpressionParser::is_parenthesis(char c) const {
    switch (c) {
        case '(':
        case '[':
        case '{':
            return SYMACHIN_TTYPE_LPAR;
        case ')':
        case ']':
        case '}':
            return SYMACHIN_TTYPE_RPAR;
        default:
            return SYMACHIN_TTYPE_SYMBOL;
    }
}


/**
 * Get the next token in the stream.
 */
struct intp_token *ExpressionParser::next() {
    string s;
    char c;
    struct intp_token *tok = new struct intp_token;
    int p;
    tok->precedence = 0;

    for (bufindx++; !eoe(); bufindx++) {
        c = buffer->at(bufindx);
        if (is_ignoreable(c)) {
            if (s.length() == 0)
                continue;
            else {
                break;
            }
        } else if ((p=is_operator(c)) > 0) {
            if (s.length() > 0) {
                bufindx--;
                break;
            } else {
                tok->type = SYMACHIN_TTYPE_OPERATOR;
                tok->precedence = p;
                tok->text = c;
                return tok;
            }
        } else if ((tok->type=is_parenthesis(c)) != SYMACHIN_TTYPE_SYMBOL) {
            if (s.length() > 0) {
                bufindx--;
                break;
            } else {
                if (tok->type == SYMACHIN_TTYPE_LPAR)
                    tok->text = '(';
                else if (tok->type == SYMACHIN_TTYPE_RPAR)
                    tok->text = ')';
                return tok;
            }
        } else
            s += c;
    }

    tok->type = SYMACHIN_TTYPE_SYMBOL;
    tok->text = s;

    if (s == "") return nullptr;
    else return tok;
}

