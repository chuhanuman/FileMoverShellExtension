#include "windows.h"

#include "contextMenuFactory.h"
#include "../config.h"

long globalReferenceCount = 0;

extern "C" __declspec(dllexport)
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpvreserved) {
	return true;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) {
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
	return globalReferenceCount == 0 ? S_OK : S_FALSE;
}