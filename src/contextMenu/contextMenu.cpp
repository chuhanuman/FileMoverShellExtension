#include "contextMenu.h"

#include <shlwapi.h>

HRESULT ContextMenu::QueryInterface(REFIID riid, void** ppvObject) {
	if (riid == IID_IUnknown || riid == IID_IShellExtInit) {
		*ppvObject = static_cast<IShellExtInit*>(this);
	} else if (riid == IID_IContextMenu) {
		*ppvObject = static_cast<IContextMenu*>(this);
	} else {
		*ppvObject = nullptr;
		return E_POINTER;
	}

	AddRef();
	return S_OK;
}

ULONG ContextMenu::AddRef() {
	return InterlockedIncrement(&referenceCount);
}

ULONG ContextMenu::Release() {
	const long result = InterlockedDecrement(&referenceCount);

	if (result == 0) {
		delete this;
	}

	return result;
}

HRESULT ContextMenu::Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pDataObject, HKEY hRegKey) {
	if (dataObject != nullptr) {
		dataObject->Release();
	}
	dataObject = pDataObject;

	return S_OK;
}

HRESULT ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi) {
	const bool hasUnicode = (lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX)) && (lpcmi->fMask & CMIC_MASK_UNICODE);

	if (!hasUnicode && HIWORD(lpcmi->lpVerb)) {
		if (StrCmpI(lpcmi->lpVerb, testVerb) != 0) {
			return E_FAIL;
		}
	} else if(hasUnicode && HIWORD(reinterpret_cast<CMINVOKECOMMANDINFOEX*>(lpcmi)->lpVerbW)) {
		if (StrCmpIW(reinterpret_cast<CMINVOKECOMMANDINFOEX*>(lpcmi)->lpVerbW, testVerbW) != 0) {
			return E_FAIL;
		}
	} else if (LOWORD(lpcmi->lpVerb) != testOffset) {
		return E_FAIL;
	}

	MessageBox(lpcmi->hwnd, testVerb, testVerb, MB_OK);

	return S_OK;
}

HRESULT ContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
	if (uFlags & CMF_DEFAULTONLY) {
		return S_OK;
	}

	InsertMenu(hMenu, 
			   indexMenu, 
			   MF_BYPOSITION | MF_STRING,
			   idCmdFirst + testOffset,
			   testVerb);
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast<USHORT>(testOffset + 1));
}

HRESULT ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) {
	//Always returns error to this as it should never be called
	return -1;
}