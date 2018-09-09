#ifndef _LEXER_EXCEPTION_H
#define _LEXER_EXCEPTION_H

#include <string>
#include <utility>
#include "symachin/SymachinException.h"

/**
 * Parser Exception class.
 */

class ParserException : public SymachinException {
	public:
		template<typename ... Args>
		ParserException(const std::string& msg, Args&& ... args) : SymachinException(msg, std::forward<Args>(args) ...) {}
};

#endif/*_LEXER_EXCEPTION_H*/
