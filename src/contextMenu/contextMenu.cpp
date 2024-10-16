#include "contextMenu.h"

#include <fstream>

#include "shlwapi.h"


#include "..\config.h"

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

	loadDataObject();

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

	const auto source = new char[MAX_PATH * files.size() + 1];
	size_t sourceLength = 0;
	for (const auto& filePath : files) {
		strncpy_s(source + sourceLength, MAX_PATH, filePath.c_str(), filePath.size());
		sourceLength += filePath.size();

		source[sourceLength] = '\0';
		sourceLength++;
	}
	source[sourceLength] = '\0';
	source[sourceLength + 1] = '\0';
	sourceLength += 2;

	std::string destinationPath = destinationParentFolderPath;
	destinationPath.push_back('\\');
	destinationPath.append(folders.at(offset));
	char destination[MAX_PATH + 1];
	strncpy_s(destination, MAX_PATH, destinationPath.c_str(), destinationPath.size());
	destination[destinationPath.size()] = '\0';
	destination[destinationPath.size() + 1] = '\0';

	SHFILEOPSTRUCT fileOperation = {
		lpcmi->hwnd,
		FO_MOVE,
		source,
		destination,
		FOF_ALLOWUNDO,
		false,
		INVALID_HANDLE_VALUE,
		nullptr
	};

	const int result = SHFileOperation(&fileOperation);
	delete[] source;
	if (result != 0) {
		const std::string message = "SHFileOperation Error Code " + std::to_string(result);
		MessageBox(lpcmi->hwnd, message.c_str(), NULL, MB_OK);

		return E_FAIL;
	}

	return S_OK;
}

HRESULT ContextMenu::GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) {
	//Always returns an error
	return -1;
}

void ContextMenu::loadDataObject() {
	parentFolderPath.clear();
	destinationParentFolderPath.clear();
	folders.clear();
	files.clear();

	const auto clipFormat = static_cast<CLIPFORMAT>(RegisterClipboardFormat(CFSTR_SHELLIDLIST));
	FORMATETC dataFormat{clipFormat, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	STGMEDIUM stgMedium;
	if (dataObject->GetData(&dataFormat, &stgMedium) != S_OK) {
		return;
	}

	const auto cida = static_cast<LPIDA>(GlobalLock(stgMedium.hGlobal));
	if (cida != NULL) {
		loadFiles(cida);

		if (!parentFolderPath.empty()) {
			findFolders();
		}

		GlobalUnlock(stgMedium.hGlobal);
	}

	ReleaseStgMedium(&stgMedium);
}

void ContextMenu::loadFiles(const LPIDA cida) {
	char path[MAX_PATH];
	bool success;

	const auto fileList = reinterpret_cast<LPBYTE>(cida);
	PCIDLIST_ABSOLUTE parentIdList = nullptr;
	for (unsigned int i = 0; i < cida->cidl + 1; i++) {
		auto itemIdList = reinterpret_cast<PCIDLIST_ABSOLUTE>(fileList + cida->aoffset[i]);
		if (i == 0) {
			parentIdList = itemIdList;
		} else {
			itemIdList = ILCombine(parentIdList, itemIdList);
		}

		success = SHGetPathFromIDList(itemIdList, path);
		if (!success) {
			return;
		}

		if (i == 0) {
			parentFolderPath = path;
		} else {
			files.emplace_back(path);
		}
	}
}

void ContextMenu::findFolders() {
	char path[MAX_PATH];
	bool success;
	long errorCode;

	errorCode = SHGetFolderPath(0, CSIDL_APPDATA, NULL, 0, path);
	if (errorCode != ERROR_SUCCESS) {
		return;
	}

	std::string settingsPath(path);
	settingsPath.append("\\");
	settingsPath.append(config::PROGRAM_NAME);
	settingsPath.append("\\");
	settingsPath.append(config::SETTINGS_FILENAME);

	std::ifstream fin(settingsPath);
	std::string setting;
	if (fin.fail()) {
		setting = config::DEFAULT_SETTING;
	} else {
		fin >> setting;
	}
	fin.close();

	std::string searchPath = parentFolderPath;
	searchPath.append(setting);

	destinationParentFolderPath = searchPath.substr(0, searchPath.find_last_of("/\\"));

	WIN32_FIND_DATAA fileData;
	HANDLE searcher = FindFirstFile(searchPath.c_str(), &fileData);
	success = (searcher != INVALID_HANDLE_VALUE);

	while (success) {
		if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			//Ignores . and .. folders
			bool ignore = fileData.cFileName[0] == '.' && (fileData.cFileName[1] == '\0' || (fileData.cFileName[1] == '.' && fileData.cFileName[2] == '\0'));
			if (!ignore) {
				folders.emplace_back(fileData.cFileName);
			}
		}

		success = FindNextFile(searcher, &fileData);
	}
	FindClose(searcher);
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