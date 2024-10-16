#ifndef CONTEXT_MENU_H
#define CONTEXT_MENU_H

#include <string>
#include <vector>

#include "shlobj_core.h"
#include "shobjidl_core.h"

class ContextMenu : public IShellExtInit,
                    public IContextMenu {
public:
	HRESULT QueryInterface(REFIID riid, void** ppvObject) override;

	ULONG AddRef() override;

	ULONG Release() override;

	HRESULT Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pDataObject, HKEY hRegKey) override;

	HRESULT QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags) override;

	HRESULT InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi) override;

	HRESULT GetCommandString(UINT_PTR idCmd, UINT uType, UINT* pReserved, CHAR* pszName, UINT cchMax) override;
private:
	void loadDataObject();

	void loadFiles(const LPIDA cida);

	void findFolders();

	HMENU createSubmenu(UINT idCmdFirst, UINT idCmdLast);

	long referenceCount = 0;
	IDataObject* dataObject = nullptr;
	std::string parentFolderPath;
	std::string destinationParentFolderPath;
	std::vector<std::string> folders;
	std::vector<std::string> files;
};

#endif // CONTEXT_MENU_H