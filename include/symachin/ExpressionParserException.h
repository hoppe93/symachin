#ifndef _EXPRESSION_PARSER_EXCEPTION_H
#define _EXPRESSION_PARSER_EXCEPTION_H

#include <string>
#include <utility>
#include "symachin/SymachinException.h"

/**
 * Parser Exception class.
 */

class ExpressionParserException : public SymachinException {
	public:
		template<typename ... Args>
		ExpressionParserException(const std::string& msg, Args&& ... args) : SymachinException(msg, std::forward<Args>(args) ...) {}
};

#endif/*_EXPRESSION_PARSER_EXCEPTION_H*/
