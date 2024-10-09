#include <windows.h>

#include <iostream>

#include "config.h"
#include "utils.h"

int main() {
	HKEY guidKey;
	std::string guidPath = config::GUID_PREFIX_PATH;
	guidPath.append(config::GUID);
	long errorCode = RegCreateKeyEx(HKEY_CLASSES_ROOT,
									guidPath.c_str(), 
									0, 
									NULL,
									REG_OPTION_NON_VOLATILE,
									KEY_WRITE,
									NULL,
									&guidKey,
									NULL);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);
		return -1;
	}

	errorCode = RegSetValueEx(guidKey,
							  NULL, 
							  0, 
							  REG_SZ, 
							  reinterpret_cast<const BYTE*>(config::NAME), 
							  strlen(config::NAME) + 1);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);
		return -1;
	}

	HKEY serverKey;
	errorCode = RegCreateKeyEx(guidKey,
							   "InProcServer32", 
							   0, 
							   NULL,
							   REG_OPTION_NON_VOLATILE,
							   KEY_WRITE,
							   NULL,
							   &serverKey,
							   NULL);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);

		return -1;
	}

	errorCode = RegSetValueEx(serverKey,
							  NULL, 
							  0, 
							  REG_SZ, 
							  reinterpret_cast<const BYTE*>(config::DLL_PATH), 
							  strlen(config::DLL_PATH) + 1);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);
		return -1;
	}

	errorCode = RegSetValueEx(serverKey,
							  "ThreadingModel", 
							  0, 
							  REG_SZ,
							  reinterpret_cast<const BYTE*>(config::THREADING_MODEL), 
							  strlen(config::THREADING_MODEL) + 1);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);
		return -1;
	}

	HKEY shellExtensionKey;
	std::string shellExtensionPath = config::CONTEXT_MENU_HANDLER_PREFIX_PATH;
	shellExtensionPath.append(config::NAME);
	errorCode = RegCreateKeyEx(HKEY_CLASSES_ROOT,
							   shellExtensionPath.c_str(), 
							   0, 
							   NULL,
							   REG_OPTION_NON_VOLATILE,
							   KEY_WRITE,
							   NULL,
							   &shellExtensionKey,
							   NULL);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);
		return -1;
	}

	errorCode = RegSetValueEx(shellExtensionKey,
							  NULL, 
							  0, 
							  REG_SZ,
							  reinterpret_cast<const BYTE*>(config::GUID), 
							  strlen(config::GUID) + 1);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);
		return -1;
	}

	return 0;
}
