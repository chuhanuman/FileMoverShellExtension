#include <fstream>

#include "windows.h"

#include "contextMenuFactory.h"
#include "../config.h"

long referenceCount = 0;
HINSTANCE hInstance;

extern "C" __declspec(dllexport)
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvreserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		hInstance = hinstDll;
	}
	
	return true;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
	//std::ofstream fout("C:\\Users\\chuha\\source\\repos\\FileMoverShellExtension\\out\\build\\x64-debug\\log.txt", std::ios::app);
	//fout << "In DllGetClassObject\n";
	//fout.close();

	CLSID clsid;
	if (CLSIDFromString(config::GUID_W, &clsid) != S_OK) {
		return E_UNEXPECTED;
	}

	if (!IsEqualIID(rclsid, clsid)) {
		return CLASS_E_CLASSNOTAVAILABLE;
	}

	ContextMenuFactory* factory = new ContextMenuFactory;
	HRESULT result = factory->QueryInterface(riid, ppv);
	if (result != S_OK) {
		delete factory;
	}

	return result;
}

STDAPI DllCanUnloadNow() {
	return referenceCount == 0 ? S_OK : S_FALSE;
}