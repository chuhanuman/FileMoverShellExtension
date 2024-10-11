#ifndef CONTEXT_MENU_FACTORY_H
#define CONTEXT_MENU_FACTORY_H

#include "shobjidl_core.h"

class ContextMenuFactory : public IClassFactory {
public:
	ContextMenuFactory();

	~ContextMenuFactory();

	HRESULT QueryInterface(REFIID riid, void** ppvObject) override;

	ULONG AddRef() override;

	ULONG Release() override;

	HRESULT CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override;

	HRESULT LockServer(BOOL fLock) override;
private:
	long referenceCount = 0;
};

#endif // CONTEXT_MENU_FACTORY_H