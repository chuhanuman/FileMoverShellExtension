#include "contextMenu.h"

#include <fstream>

#include "shlobj_core.h"
#include "shlwapi.h"

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

HRESULT ContextMenu::QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) {
	if (uFlags & CMF_DEFAULTONLY) {
		return S_OK;
	}

	findFolders();

	if (folders.empty()) {
		return S_OK;
	}

	HMENU submenu = createSubmenu(idCmdFirst, idCmdLast);

	InsertMenu(hMenu, 
			   indexMenu, 
			   MF_BYPOSITION | MF_POPUP | MF_STRING,
			   reinterpret_cast<UINT_PTR>(submenu),
			   "Move to");
	return MAKE_HRESULT(SEVERITY_SUCCESS, 0, static_cast<USHORT>(folders.size()));
}

HRESULT ContextMenu::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi) {
	const bool hasUnicode = (lpcmi->cbSize == sizeof(CMINVOKECOMMANDINFOEX)) && (lpcmi->fMask & CMIC_MASK_UNICODE);

	if (!hasUnicode && HIWORD(lpcmi->lpVerb)) {
		return E_FAIL;
	} else if(hasUnicode && HIWORD(reinterpret_cast<CMINVOKECOMMANDINFOEX*>(lpcmi)->lpVerbW)) {
		return E_FAIL;
	}

	const unsigned short offset = LOWORD(lpcmi->lpVerb);

	if (offset >= folders.size()) {
		return E_FAIL;
	}

	const char* filename = folders.at(offset).c_str();

	MessageBox(lpcmi->hwnd, filename, filename, MB_OK);

	return S_OK;
}

HRESULT ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) {
	//Always returns an error
	return -1;
}

void ContextMenu::findFolders() {
	folders.clear();

	char temp[MAX_PATH];
	long errorCode = SHGetFolderPath(0, CSIDL_APPDATA, NULL, 0, temp);
	if (errorCode != ERROR_SUCCESS) {
		return;
	}

	folders.push_back("a");
	folders.push_back("b");
	folders.push_back("c");

	std::ofstream fout("C:\\Users\\chuha\\source\\repos\\FileMoverShellExtension\\out\\build\\x64-debug\\log.txt", std::ios::app);
	fout << "In findFolders\n";
	for (const std::string& folder : folders) {
		fout << "Folder: " << folder << '\n';
	}
	fout.close();
}

HMENU ContextMenu::createSubmenu(UINT idCmdFirst, UINT idCmdLast) {
	HMENU submenu = CreatePopupMenu();

	for (unsigned int i = 0; i < folders.size(); i++) {
		if (idCmdFirst + i > idCmdLast) {
			break;
		}

		InsertMenu(submenu, 
				   -1,
				   MF_BYPOSITION | MF_STRING,
				   idCmdFirst + i,
				   folders.at(i).c_str());
	}

	return submenu;
}