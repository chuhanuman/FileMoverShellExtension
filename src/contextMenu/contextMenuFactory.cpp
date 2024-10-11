#include "contextMenuFactory.h"

#include "contextMenu.h"

extern long globalReferenceCount;

ContextMenuFactory::ContextMenuFactory() {
	InterlockedIncrement(&globalReferenceCount);
}

ContextMenuFactory::~ContextMenuFactory() {
	InterlockedDecrement(&globalReferenceCount);
}

HRESULT ContextMenuFactory::QueryInterface(const IID& riid, void** ppvObject) {
	if (riid == IID_IUnknown || riid == IID_IClassFactory) {
		*ppvObject = static_cast<IClassFactory*>(this);
	} else {
		*ppvObject = nullptr;
		return E_POINTER;
	}

	AddRef();
	return S_OK;
}

ULONG ContextMenuFactory::AddRef() {
	return InterlockedIncrement(&referenceCount);
}

ULONG ContextMenuFactory::Release() {
	const long result = InterlockedDecrement(&referenceCount);

	if (result == 0) {
		delete this;
	}

	return result;
}

HRESULT ContextMenuFactory::CreateInstance(IUnknown* pUnkOuter, const IID& riid, void** ppvObject) {
	if (pUnkOuter != nullptr) {
		return CLASS_E_NOAGGREGATION;
	}

	ContextMenu* contextMenu = new ContextMenu;
	HRESULT result = contextMenu->QueryInterface(riid, ppvObject);
	if (result != S_OK) {
		delete contextMenu;
	}

	return result;
}

HRESULT ContextMenuFactory::LockServer(BOOL fLock) {
	return S_OK; //Never locks server in memory
}
