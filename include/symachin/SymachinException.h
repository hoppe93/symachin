#ifndef _SYMACHIN_EXCEPTION_H
#define _SYMACHIN_EXCEPTION_H

#include <exception>
#include <string>
#include <utility>

class SymachinException : public std::exception {
	public:
        SymachinException();
		SymachinException(const std::string&);

		/**
		 * Implementation of constructor with variable
		 * number of arguments.
		 */
		template<typename ... Args>
		SymachinException(const std::string& msg, Args&& ... args) {
            ConstructErrorMessage(msg, std::forward<Args>(args) ...);
		}
        template<typename ... Args>
        void ConstructErrorMessage(const std::string& msg, Args&& ... args) {
			#define SYMACHINEXCEPTION_BUFFER_SIZE 1000
			int n;
			char *buffer;

			buffer = (char*)malloc(sizeof(char)*SYMACHINEXCEPTION_BUFFER_SIZE);

			n = snprintf(buffer, SYMACHINEXCEPTION_BUFFER_SIZE, msg.c_str(), std::forward<Args>(args) ...);
			if (n < 0) {	// Encoding error. Just save error message unformatted.
				this->errormsg = msg;
			} else if (n >= SYMACHINEXCEPTION_BUFFER_SIZE) {
				buffer = (char*)realloc(buffer, sizeof(char)*(n+1));
				n = snprintf(buffer, n, msg.c_str(), std::forward<Args>(args) ...);
				this->errormsg = buffer;
			} else this->errormsg = buffer;
        }

		virtual const char* what() const throw();
		virtual const std::string& whats() const throw();
	private:
		std::string errormsg;
};

#endif/*_SYMACHIN_EXCEPTION_H*/
