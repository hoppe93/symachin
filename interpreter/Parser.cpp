/**
 * Implementation of the 'Parser' class.
 */

#include <string>
#include <sstream>
#include <vector>
#include "interpreter/Lexer.h"
#include "interpreter/Parser.h"
#include "symachin/ExpressionParserException.h"

using namespace std;
using namespace symachin;

/**
 * Load the given list of tokens and
 * prepare to parse it.
 */
void Parser::LoadTokens(vector<token*> *tokens) {
    tokenlist = tokens;
    tknindex = 0;
}

/**
 * Parse (after lexing) the contents of the given file.
 */
void Parser::ParseFile(const string &fname) {
    Lexer lex;
    vector<token*> *tokens = lex.LexFile(fname);
    Parse(tokens);

    delete tokens;
}

/**
 * Parse (after lexing) the given string.
 */
void Parser::Parse(string &contents) {
    Lexer lex;
    vector<token*> *tokens = lex.Lex(contents, "<string>");
    Parse(tokens);

    delete tokens;
}

/**
 * Parse the given list of tokens.
 */
void Parser::Parse(vector<token*> *tokens) {
    token *tkn;
    LoadTokens(tokens);
    
    try {
        while ((tkn=advance())->type != token::ENDOFSTREAM) {
            // Handle label
            if (tkn->type == token::LABEL) {
                currentlabel = tkn->text;
                tkn = advance();
            }

            switch (tkn->type) {
                // : <expr>
                case token::ASSIGN: {
                    expect(token::EXPRESSION);
                    require_label();

                    ExpressionPtr ep(new Expression(gtkn()->text));
                    assign(currentlabel, ep);
                } break;

                // APPLY <ref> TO <expr>
                case token::APPLY: {
                    string refr;
                    expect(token::REFERENCE);
                    refr = gtkn()->text;

                    expect(token::TO);
                    expect(token::EXPRESSION);
                    ExpressionPtr ep(new Expression(gtkn()->text));

                    require_label();

                    apply_to(currentlabel, refr, ep);
                } break;

                // ASSERT <expr> = <expr>;
                case token::ASSERT: {
                    expect_expression(token::EQUALS);
                    ExpressionPtr ep1(new Expression(gtkn()->text));

                    expect_expression();
                    ExpressionPtr ep2(new Expression(gtkn()->text));

                    this->assert(ep1, ep2);
                } break;

                // DEFINE ... END
                case token::DEFINE: {
                    string wrd;
                    ReplacePtr rep(new Replace());

                    do {
                        expect(token::WORD);
                        wrd = gtkn()->text;

                        expect(token::RARROW);
                        expect(token::EXPRESSION);

                        rep->CreateRule(wrd, gtkn()->text);
                    } while (peek() != token::END);
                    expect(token::END);

                    require_label();
                    define(currentlabel, rep);
                } break;

                // EVAL <expr> WITH ... END
                // EVAL <expr> WITH ... ASSERT <number>
                case token::EVAL: {
                    expect_expression(token::WITH);
                    ExpressionPtr ep(new Expression(gtkn()->text));
                    map<string, double> subst;
                    double otherval = 1.0;

                    do {
                        string sub;
                        if (expect(token::WORD, token::OTHER) != token::OTHER)
                            sub = gtkn()->text;

                        expect(token::EQUALS);
                        expect(token::WORD);

                        string val = gtkn()->text;
                        double dval;
                        if (is_number(val))
                            dval = stod(val);
                        else if (subst.find(val) != subst.end())
                            dval = subst[val];
                        else
                            Error("Unrecognized value on RHS of expression: '%s'.", val.c_str());

                        if (sub.empty())
                            otherval = dval;
                        else
                            subst[sub] = dval;

                        expect(token::ENDSTATEMENT);

                    } while (peek() != token::END && peek() != token::ASSERT);

                    if (expect(token::ASSERT, token::END) == token::ASSERT) {
                        expect(token::WORD);
                        string num = gtkn()->text;

                        if (!is_number(num))
                            Error("Expected numeric value after 'assert'.");

                        double val = stod(num);

                        expect(token::ENDSTATEMENT);

                        evaluate_assert(ep, val, subst, otherval);
                    } else
                        evaluate(ep, subst, otherval);

                } break;

                // GROUP <expr> BY ... END
                case token::GROUP: {
                    expect_expression(token::BY);
                    ExpressionPtr ep(new Expression(gtkn()->text));
                    vector<ExpressionPtr> exprs;
                    vector<string> labels;
                    string sublbl, otherlbl;

                    do {
                        expect(token::LABEL);
                        if (currentlabel.empty())
                            sublbl = gtkn()->text;
                        else
                            sublbl = currentlabel + "." + gtkn()->text;

                        if (peek() == token::OTHER) {
                            expect(token::OTHER);
                            otherlbl = sublbl;
                            expect(token::ENDSTATEMENT);
                        } else {
                            expect(token::EXPRESSION);
                            exprs.push_back(ExpressionPtr(new Expression(gtkn()->text)));
                            labels.push_back(sublbl);
                        }
                    } while (peek() != token::END);

                    group_by(ep, exprs, labels, otherlbl);
                    expect(token::END);
                } break;

                // PRINT <expr>
                case token::PRINT: {
                    print();
                } break;

                // PRINTF <expr>
                case token::PRINTF: {
                    expect(token::EXPRESSION);
                    ExpressionPtr ep(new Expression(gtkn()->text));

                    this->printf(ep);
                } break;

                // PRINTN <expr>
                case token::PRINTN: {
                    expect(token::EXPRESSION);
                    ExpressionPtr ep(new Expression(gtkn()->text));

                    printn(ep);
                } break;

                // REPLACE <word> -> <expr> IN <expr>;
                case token::REPLACE: {
                    expect(token::WORD);
                    string wrd = gtkn()->text;

                    expect(token::RARROW);
                    expect_expression(token::IN);
                    string repl = gtkn()->text;

                    expect_expression();
                    ExpressionPtr ep(new Expression(gtkn()->text));
                    
                    require_label();

                    replace_in(currentlabel, wrd, repl, ep);
                } break;

                default:
                    Error("Unexpected token: %s.", tkn->ToString().c_str());
            }
        
            currentlabel.clear();
        }
    } catch (ExpressionParserException &ex) {
        Error(ex.whats());
    }
}

/**********************
 * INTERNAL ROUTINES *
 *********************/
/**
 * Advance the token reader.
 * Returns the type of the reached token.
 */
token *Parser::advance() {
    if (tknindex >= tokenlist->size())
        return &endtkn;
    
    return tokenlist->at(tknindex++);
}

/**
 * Advance the token reader and throw
 * an error if the next token is not
 * of the expected type.
 */
ttype Parser::expect(ttype t) {
    if (t == token::EXPRESSION)
        return expect_expression();

    if (tknindex >= tokenlist->size())
        Error("Unexpected end of token stream.");
    token *tkn = tokenlist->at(tknindex);

    if (tkn->type != t)
        Error("Unexpected token '%s'.", tkn->ToString().c_str());

    tknindex++;
    return tkn->type;
}

/**
 * Expect the following sequence of tokens to
 * be part of an expression. Read them as such
 * and return a single 'EXPRESSION' token.
 *
 * statend: Token to use to end statement (default: ENDSTATEMENT)
 */
ttype Parser::expect_expression(ttype statend) {
    ttype t;
    token *rtkn = new token;
    rtkn->type = token::EXPRESSION;

    vector<token*>::iterator bt = tokenlist->begin()+tknindex;
    long _tknindex = tknindex;

    rtkn->filename = (*bt)->filename;
    rtkn->charpos = (*bt)->charpos;
    rtkn->line = (*bt)->line;

    while (
        (t=expect(
            token::WORD, token::REFERENCE, token::PLUS,
            token::MINUS, token::MULTIPLICATION,
            token::LPAR, token::RPAR, statend
        )) != statend
    ) {
        token *tkn = gtkn();

        if (t == token::REFERENCE) {   // Insert referenced expression
            if (expressions.count(tkn->text) == 0)
                Error("Label '%s' has not been defined.", tkn->text.c_str());

            ExpressionPtr ep = expressions[tkn->text];
            string s = ep->ToString();
            string q;
            if (s[0] == '-')
                q = "(0" + s + ")";
            else
                q = "(" + s + ")";

            rtkn->text += q;
        } else  // Just append token contents
            rtkn->text += tkn->text;

        // This token will be erased later, so delete
        // the pointer to it now.
        delete tkn;
    }

    // Token will be erased next, so delete now
    if (t == statend)
        delete gtkn();

    // Erase old tokens
    tokenlist->erase(bt, tokenlist->begin()+tknindex);

    // Insert 'rtkn' into tokenlist
    tokenlist->insert(bt, rtkn);
    tknindex = _tknindex+1;
    
    return token::EXPRESSION;
}

/**
 * Get the current token.
 */
token *Parser::gtkn() const {
    if (tknindex > 0 && tknindex <= tokenlist->size())
        return tokenlist->at(tknindex-1);
    else
        Error("Internal error: Attempted to access token without active stream. Perhaps you forgot a final ';' or 'end'?");
    return nullptr;
}

/**
 * Returns 'true' if the given string is a valid
 * C++ floating-point number.
 *
 * s: String to check if valid number.
 */
bool Parser::is_number(const string& s) const {
    double d;
    stringstream sstr(s);

    return !((sstr >> noskipws >> d).rdstate() ^ ios_base::eofbit);
}

/**
 * Peek at the type of the next token
 * in the token list.
 */
ttype Parser::peek() const {
    if (tknindex >= tokenlist->size())
        return token::ENDOFSTREAM;
    
    return tokenlist->at(tknindex)->type;
}

/**
 * Make sure that there is a label
 * ready to be assigned to.
 */
void Parser::require_label() const {
    if (currentlabel.empty())
        Error("No label for assignment declared.");
}

