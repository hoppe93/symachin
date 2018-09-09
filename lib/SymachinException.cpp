
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>
#include "symachin/SymachinException.h"

using namespace std;

/**
 * Constructor.
 */
SymachinException::SymachinException() { }
SymachinException::SymachinException(const string& msg) {
	this->errormsg = msg;
}
const char *SymachinException::what() const throw() {
	return this->errormsg.c_str();
}
const string& SymachinException::whats() const throw() {
	return this->errormsg;
}
