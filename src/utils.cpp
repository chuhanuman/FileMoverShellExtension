#include "utils.h"

#include <windows.h>

#include <iostream>

void printWindowsError(const long errorCode) {
	constexpr int bufferSize = 1024;
	char buffer[bufferSize];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
					  nullptr, 
					  errorCode,
					  LANG_USER_DEFAULT,
					  buffer,
					  bufferSize,
					  nullptr);
	std::cout << buffer << '\n';
}