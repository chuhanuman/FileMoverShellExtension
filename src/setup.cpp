#include <iostream>

#include "windows.h"
#include "shlobj_core.h"

#include "config.h"
#include "utils.h"

int main() {
	char temp[MAX_PATH];
	long errorCode = SHGetFolderPath(0, CSIDL_APPDATA, NULL, 0, temp);
	if (errorCode != ERROR_SUCCESS) {
		printWindowsError(errorCode);
		return -1;
	}

	std::string settingsPath(temp);
	settingsPath.append("\\");
	settingsPath.append(config::NAME);
	bool success = CreateDirectory(settingsPath.c_str(), NULL);
	if (!success && GetLastError() != ERROR_ALREADY_EXISTS) {
		printWindowsError(GetLastError());
		return -1;
	}

	settingsPath.append("\\");
	settingsPath.append(config::SETTINGS_FILENAME);
	HANDLE settingsFile = CreateFile(settingsPath.c_str(),
									 GENERIC_WRITE,
									 0,
									 NULL,
									 CREATE_ALWAYS,
									 FILE_ATTRIBUTE_NORMAL,
									 NULL);
	if (settingsFile == INVALID_HANDLE_VALUE) {
		printWindowsError(GetLastError());
		return -1;
	}

	success = WriteFile(settingsFile,
					    config::DEFAULT_SETTING,
						strlen(config::DEFAULT_SETTING),
						NULL,
						NULL);
	if (!success) {
		printWindowsError(GetLastError());
		return -1;
	}

	if (CloseHandle(settingsFile) == 0) {
		printWindowsError(GetLastError());
		return -1;
	}

	HKEY guidKey;
	std::string guidPath = config::GUID_PREFIX_PATH;
	guidPath.append(config::GUID);
	errorCode = RegCreateKeyEx(HKEY_CLASSES_ROOT,
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
