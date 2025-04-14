#include <string>
#include <iostream>
#include <stdarg.h>

namespace MyEngine {
	static class Logger {
	public:
		static void Log(std::string message, ...) {
			va_list args;
			va_start(args, message);
			printf(message.c_str(), args);
		}
	};
}