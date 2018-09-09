#ifndef _FACTOR_EXCEPTION_H
#define _FACTOR_EXCEPTION_H

#include <string>
#include <utility>
#include "symachin/SymachinException.h"

/**
 * Factor Exception class.
 */

class FactorException : public SymachinException {
	public:
		template<typename ... Args>
		FactorException(const std::string& msg, Args&& ... args) : SymachinException(msg, std::forward<Args>(args) ...) {}
};

#endif/*_FACTOR_EXCEPTION_H*/
